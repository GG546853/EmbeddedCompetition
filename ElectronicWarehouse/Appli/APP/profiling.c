#include "profiling.h"

#if PROFILING_ENABLE

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <string.h>

#if HYPERRAM_BENCH_ENABLE
#include "dcmipp.h"
#include "imx335.h"
#include "rgblcd.h"
#include "app_x-cube-ai.h"
#endif
#if HYPERRAM_BENCH_ENABLE || LTDC_STATS_ENABLE
#include "ltdc.h"
#endif

/* SystemCoreClock is the Cortex-M55 core clock (600 MHz on STM32N6). */
#define CYCLES_PER_US       ((uint32_t)(SystemCoreClock / 1000000u))
#define PROF_PRINT_DIV      2u    /* StatsTask calls every 1 s -> 2 s window */
#define PROF_WFE_SHORT_CYC  100u  /* < 100 cycles since WFE return = spurious */

typedef struct {
    uint64_t sum;
    uint64_t max;
    uint32_t count;
} prof_stat_t;

typedef struct {
    prof_stat_t init;
    prof_stat_t deinit;
    prof_stat_t runblock;
    prof_stat_t wfe;
    prof_stat_t post;
    uint64_t    wall_cycles;
    uint32_t    runs;
    uint32_t    wfe_short;
    uint32_t    ep_sw;
    uint32_t    ep_hw;
    uint32_t    ep_hybrid;
    uint32_t    ep_blob;
    uint32_t    ep_internal;
    prof_stat_t ept_sw;
    prof_stat_t ept_hw;
    prof_stat_t ept_hyb;
    prof_stat_t ept_in;
} prof_net_stat_t;

static prof_stat_t      s_stage[PROF_STAGE_N];
static prof_net_stat_t  s_net[PROF_NET_N];
static uint32_t         s_iter;
static uint32_t         s_print_div;
static uint32_t         s_p1_frames;
static uint32_t         s_p2_frames;
static uint32_t         s_p1_prev;
static uint32_t         s_p2_prev;

static uint32_t prof_lock(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

static void prof_unlock(uint32_t primask)
{
    __set_PRIMASK(primask);
}

static void stat_add(prof_stat_t *st, uint64_t cycles)
{
    st->sum += cycles;
    st->count++;
    if (cycles > st->max) st->max = cycles;
}

static uint32_t stat_avg_us(const prof_stat_t *st)
{
    if (st->count == 0u) return 0u;
    return (uint32_t)(st->sum / st->count / CYCLES_PER_US);
}

static uint32_t stat_max_us(const prof_stat_t *st)
{
    return (uint32_t)(st->max / CYCLES_PER_US);
}

/* Total time of all calls belonging to one inference (not per-call average). */
static uint32_t stat_total_per_run_us(const prof_stat_t *st, uint32_t runs)
{
    if (runs == 0u) return 0u;
    return (uint32_t)(st->sum / runs / CYCLES_PER_US);
}

static uint32_t per_run_count(uint32_t count, uint32_t runs)
{
    if (runs == 0u) return 0u;
    return count / runs;
}

static uint32_t net_wall_avg_us(const prof_net_stat_t *n)
{
    if (n->runs == 0u) return 0u;
    return (uint32_t)(n->wall_cycles / n->runs / CYCLES_PER_US);
}

uint64_t profiling_now(void)
{
    return getRunTimeCounterValue();
}

/* ========================================================================== */
/*              LTDC FIFO-underrun counters (measurement only)                */
/* ========================================================================== */

#if LTDC_STATS_ENABLE

static volatile uint32_t s_fu_events;
static volatile uint32_t s_fuw_events;
static volatile uint32_t s_fu_poll;
static volatile uint32_t s_fuw_poll;
static uint8_t           s_ltdc_mon_inited;

static void ltdc_mon_take(void)
{
    const uint32_t mask = LTDC_ISR_FUIF | LTDC_ISR_FUWIF;
    uint32_t f = (LTDC->ISR & mask) | (LTDC->ISR2 & mask);

    if (f & LTDC_ISR_FUIF)  s_fu_events++;
    if (f & LTDC_ISR_FUWIF) s_fuw_events++;
    LTDC->ICR  = mask;
    LTDC->ICR2 = mask;
}

/* We install our own error handlers instead of calling HAL_LTDC_IRQHandler:
 * the HAL handler disables the FU interrupt and forces State=ERROR. */
void LTDC_LO_IRQHandler(void)     { ltdc_mon_take(); }
void LTDC_LO_ERR_IRQHandler(void) { ltdc_mon_take(); }
void LTDC_UP_IRQHandler(void)     { ltdc_mon_take(); }
void LTDC_UP_ERR_IRQHandler(void) { ltdc_mon_take(); }

static void ltdc_mon_init(void)
{
    if (s_ltdc_mon_inited) return;
    s_ltdc_mon_inited = 1u;

    /* FU is already enabled by HAL_LTDC_Init; add the warning threshold too. */
    hltdc.Instance->IER  |= LTDC_IER_FUWIE;
    hltdc.Instance->IER2 |= LTDC_IER_FUWIE;

    /* Only the error vectors are enabled: the global vectors also carry the
     * panel line/transfer-error interrupts, and we do not want to shadow them. */
    HAL_NVIC_SetPriority(LTDC_LO_ERR_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(LTDC_LO_ERR_IRQn);
    HAL_NVIC_SetPriority(LTDC_UP_ERR_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(LTDC_UP_ERR_IRQn);
}

#endif /* LTDC_STATS_ENABLE */

/* ========================================================================== */
/*                HyperRAM (XSPI1) bandwidth / latency bench                  */
/* ========================================================================== */

#if HYPERRAM_BENCH_ENABLE

/* EXTRAM_LVGL: 2 MB at 0x90600000, empty in the current map. */
#define BENCH_ADDR        0x90600000UL
#define BENCH_DST_ADDR    0x90700000UL
#define BENCH_REGION_SIZE (2u * 1024u * 1024u)
#define BENCH_MAX_SAMPLES 16u
#define BENCH_STRIDE_BYTES 64u

static volatile uint32_t s_bench_sink;
static int               s_bench_ok;
static uint32_t          s_chase_sram[4096] __attribute__((aligned(64)));  /* 16 KB */

static uint32_t bench_mbps_x100(uint32_t bytes, uint64_t cycles)
{
    if (cycles == 0u) return 0u;
    return (uint32_t)(((uint64_t)bytes * CYCLES_PER_US * 100ULL) / cycles);
}

static void bench_sort(uint64_t *v, uint32_t n)
{
    for (uint32_t i = 1u; i < n; i++) {
        uint64_t x = v[i];
        uint32_t j = i;
        while (j > 0u && v[j - 1u] > x) { v[j] = v[j - 1u]; j--; }
        v[j] = x;
    }
}

static void bench_report_bw(const char *tag, uint32_t bytes, const uint64_t *cyc, uint32_t n)
{
    uint64_t s[BENCH_MAX_SAMPLES];
    uint32_t best, med, worst;

    if (n == 0u) return;
    for (uint32_t i = 0; i < n; i++) s[i] = cyc[i];
    bench_sort(s, n);

    best  = bench_mbps_x100(bytes, s[0]);
    med   = bench_mbps_x100(bytes, s[n / 2u]);
    worst = bench_mbps_x100(bytes, s[n - 1u]);

    printf("[BENCH] %s: %lu B x%lu | best %lu.%02lu med %lu.%02lu worst %lu.%02lu MB/s\r\n",
           tag, (unsigned long)bytes, (unsigned long)n,
           (unsigned long)(best / 100u),  (unsigned long)(best % 100u),
           (unsigned long)(med / 100u),   (unsigned long)(med % 100u),
           (unsigned long)(worst / 100u), (unsigned long)(worst % 100u));
}

static void bench_report_us(const char *tag, const uint64_t *cyc, uint32_t n, uint32_t cfg)
{
    uint64_t s[BENCH_MAX_SAMPLES];
    uint32_t best, med, worst;

    if (n == 0u) return;
    for (uint32_t i = 0; i < n; i++) s[i] = cyc[i];
    bench_sort(s, n);

    best  = (uint32_t)(s[0] / CYCLES_PER_US);
    med   = (uint32_t)(s[n / 2u] / CYCLES_PER_US);
    worst = (uint32_t)(s[n - 1u] / CYCLES_PER_US);

    printf("[BENCH] %s cfg=%lu: best %lu med %lu worst %lu us (n=%lu)\r\n",
           tag, (unsigned long)cfg,
           (unsigned long)best, (unsigned long)med, (unsigned long)worst,
           (unsigned long)n);
}

static uint32_t bench_clamp_bytes(uint32_t kb, uint32_t limit)
{
    uint32_t bytes = kb * 1024u;
    if (bytes == 0u || bytes > limit) bytes = limit;
    return bytes & ~3u;
}

static void bench_read_run(uint32_t kb, uint32_t iters, const char *tag)
{
    uint32_t bytes = bench_clamp_bytes(kb, BENCH_REGION_SIZE);
    volatile uint32_t *p = (volatile uint32_t *)BENCH_ADDR;
    uint32_t words = bytes / 4u;
    uint64_t cyc[BENCH_MAX_SAMPLES];

    if (iters == 0u) iters = 1u;
    if (iters > BENCH_MAX_SAMPLES) iters = BENCH_MAX_SAMPLES;

    for (uint32_t it = 0; it < iters; it++) {
        uint32_t acc = 0;
        SCB_InvalidateDCache_by_Addr((uint32_t *)p, (int32_t)bytes);
        uint64_t t0 = profiling_now();
        for (uint32_t i = 0; i < words; i++) acc += p[i];
        uint64_t t1 = profiling_now();
        s_bench_sink = acc;
        cyc[it] = t1 - t0;
    }
    bench_report_bw(tag, bytes, cyc, iters);
}

static void bench_write_run(uint32_t kb, uint32_t iters, const char *tag)
{
    uint32_t bytes = bench_clamp_bytes(kb, BENCH_REGION_SIZE);
    volatile uint32_t *p = (volatile uint32_t *)BENCH_ADDR;
    uint32_t words = bytes / 4u;
    uint64_t cyc[BENCH_MAX_SAMPLES];

    if (iters == 0u) iters = 1u;
    if (iters > BENCH_MAX_SAMPLES) iters = BENCH_MAX_SAMPLES;

    for (uint32_t it = 0; it < iters; it++) {
        uint64_t t0 = profiling_now();
        for (uint32_t i = 0; i < words; i++) p[i] = 0xA5A5A5A5u;
        uint64_t t1 = profiling_now();
        cyc[it] = t1 - t0;
        SCB_CleanDCache_by_Addr((uint32_t *)p, (int32_t)bytes);
        __DSB();
    }
    bench_report_bw(tag, bytes, cyc, iters);
}

static void bench_copy_run(uint32_t kb, uint32_t iters, const char *tag)
{
    uint32_t bytes = bench_clamp_bytes(kb, BENCH_REGION_SIZE / 2u);
    volatile uint32_t *src = (volatile uint32_t *)BENCH_ADDR;
    volatile uint32_t *dst = (volatile uint32_t *)BENCH_DST_ADDR;
    uint32_t words = bytes / 4u;
    uint64_t cyc[BENCH_MAX_SAMPLES];

    if (iters == 0u) iters = 1u;
    if (iters > BENCH_MAX_SAMPLES) iters = BENCH_MAX_SAMPLES;

    for (uint32_t it = 0; it < iters; it++) {
        SCB_InvalidateDCache_by_Addr((uint32_t *)src, (int32_t)bytes);
        uint64_t t0 = profiling_now();
        for (uint32_t i = 0; i < words; i++) dst[i] = src[i];
        uint64_t t1 = profiling_now();
        cyc[it] = t1 - t0;
        SCB_CleanDCache_by_Addr((uint32_t *)dst, (int32_t)bytes);
        __DSB();
    }
    bench_report_bw(tag, bytes, cyc, iters);
}

static void bench_chain_build(volatile uint32_t *base, uint32_t nodes)
{
    const uint32_t mask = nodes - 1u;
    const uint32_t stride_words = BENCH_STRIDE_BYTES / 4u;

    for (uint32_t k = 0; k < nodes; k++) {
        uint32_t cur = (uint32_t)((k * 1103515245u + 12345u) & mask);
        uint32_t nxt = (uint32_t)(((k + 1u) * 1103515245u + 12345u) & mask);
        base[cur * stride_words] = (uint32_t)&base[nxt * stride_words];
    }
    __DSB();
}

static uint64_t bench_chase_run(volatile uint32_t *base, uint32_t nodes, uint32_t steps)
{
    const uint32_t mask = nodes - 1u;
    uint32_t bytes = nodes * BENCH_STRIDE_BYTES;

    bench_chain_build(base, nodes);
    SCB_CleanDCache_by_Addr((uint32_t *)base, (int32_t)bytes);
    SCB_InvalidateDCache_by_Addr((uint32_t *)base, (int32_t)bytes);

    volatile uint32_t *p =
        (volatile uint32_t *)((uint32_t)base + ((12345u & mask) * BENCH_STRIDE_BYTES));

    uint64_t t0 = profiling_now();
    for (uint32_t i = 0; i < steps; i++) p = (volatile uint32_t *)(*p);
    uint64_t t1 = profiling_now();
    s_bench_sink = (uint32_t)p;
    return t1 - t0;
}

static void bench_chase_report(const char *tag, uint64_t cycles, uint32_t steps)
{
    uint32_t ns_x10;
    if (steps == 0u) return;
    ns_x10 = (uint32_t)((cycles * 10000ULL) / ((uint64_t)CYCLES_PER_US * steps));
    printf("[BENCH] %s: %lu steps | %lu.%01lu ns/step\r\n",
           tag, (unsigned long)steps,
           (unsigned long)(ns_x10 / 10u), (unsigned long)(ns_x10 % 10u));
}

static int bench_layer_on(uint32_t layer)
{
    return (LTDC_LAYER(&hltdc, layer)->CR & LTDC_LxCR_LEN) != 0u;
}

static void bench_set_layers(int l0, int l1)
{
    if (l0) __HAL_LTDC_LAYER_ENABLE(&hltdc, LTDC_LAYER_1);
    else    __HAL_LTDC_LAYER_DISABLE(&hltdc, LTDC_LAYER_1);
    if (l1) __HAL_LTDC_LAYER_ENABLE(&hltdc, LTDC_LAYER_2);
    else    __HAL_LTDC_LAYER_DISABLE(&hltdc, LTDC_LAYER_2);
    __HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(&hltdc);
    vTaskDelay(pdMS_TO_TICKS(30));
}

void profiling_bench_info(void)
{
    uint32_t ic4 = RCC->IC4CFGR;
    uint32_t div = ((ic4 & RCC_IC4CFGR_IC4INT) >> RCC_IC4CFGR_IC4INT_Pos) + 1u;
    uint32_t sel = (ic4 & RCC_IC4CFGR_IC4SEL) >> RCC_IC4CFGR_IC4SEL_Pos;
    uint32_t khz = (div != 0u) ? (1200000u / div) : 0u;  /* PLL1 = 1200 MHz (sys.c) */
    uint32_t mbs_x10 = (khz * 2u) / 100u;
    uint32_t ccsidr = SCB->CCSIDR;
    uint32_t lsz = 4u << (ccsidr & 7u);

    printf("[BENCH] clock: SystemCoreClock=%lu Hz cycles/us=%lu\r\n",
           (unsigned long)SystemCoreClock, (unsigned long)CYCLES_PER_US);
    printf("[BENCH] RCC IC4CFGR=0x%08lX sel=%lu div=%lu -> XSPI1 %lu.%03lu MHz"
           " | DDR 8-bit => %lu.%01lu MB/s\r\n",
           (unsigned long)ic4, (unsigned long)sel, (unsigned long)div,
           (unsigned long)(khz / 1000u), (unsigned long)(khz % 1000u),
           (unsigned long)(mbs_x10 / 10u), (unsigned long)(mbs_x10 % 10u));
    printf("[BENCH] XSPI1 CR=0x%08lX DCR1=0x%08lX DCR2=0x%08lX DCR3=0x%08lX DCR4=0x%08lX\r\n",
           (unsigned long)XSPI1->CR, (unsigned long)XSPI1->DCR1,
           (unsigned long)XSPI1->DCR2, (unsigned long)XSPI1->DCR3,
           (unsigned long)XSPI1->DCR4);
    printf("[BENCH] XSPI1 CCR=0x%08lX TCR=0x%08lX WCCR=0x%08lX WTCR=0x%08lX HLCR=0x%08lX\r\n",
           (unsigned long)XSPI1->CCR, (unsigned long)XSPI1->TCR,
           (unsigned long)XSPI1->WCCR, (unsigned long)XSPI1->WTCR,
           (unsigned long)XSPI1->HLCR);
    printf("[BENCH] cache: CCSIDR=0x%08lX line=%lu B D-cache=%d (MPU is not configured)\r\n",
           (unsigned long)ccsidr, (unsigned long)lsz,
           (SCB->CCR & SCB_CCR_DC_Msk) ? 1 : 0);
    printf("[BENCH] regions: bench=0x%08lX..0x%08lX, dst=0x%08lX\r\n",
           (unsigned long)BENCH_ADDR, (unsigned long)(BENCH_ADDR + BENCH_REGION_SIZE),
           (unsigned long)BENCH_DST_ADDR);
}

void profiling_bench_canary(void)
{
    const uint32_t step = 64u * 1024u;
    uint32_t points = 0u;
    int ok = 1;

    for (uint32_t off = 0; off < BENCH_REGION_SIZE; off += step) {
        volatile uint32_t *q = (volatile uint32_t *)(BENCH_ADDR + off);
        uint32_t save = *q;

        *q = 0x5A5AA5A5u;
        __DSB();
        SCB_CleanInvalidateDCache_by_Addr((uint32_t *)q, 4);
        if (*q != 0x5A5AA5A5u) ok = 0;

        *q = save;
        __DSB();
        SCB_CleanInvalidateDCache_by_Addr((uint32_t *)q, 4);
        points++;
    }

    s_bench_ok = ok;
    printf("[BENCH] canary 0x%08lX+%luKB: %s (%lu points)\r\n",
           (unsigned long)BENCH_ADDR,
           (unsigned long)(BENCH_REGION_SIZE / 1024u),
           ok ? "OK" : "FAILED", (unsigned long)points);
}

void profiling_bench_read(uint32_t kb, uint32_t iters)
{
    bench_read_run(kb, iters, "read");
}

void profiling_bench_write(uint32_t kb, uint32_t iters)
{
    if (!s_bench_ok) { printf("[BENCH] write refused: run 'bench canary' first\r\n"); return; }
    bench_write_run(kb, iters, "write");
}

void profiling_bench_copy(uint32_t kb, uint32_t iters)
{
    if (!s_bench_ok) { printf("[BENCH] copy refused: run 'bench canary' first\r\n"); return; }
    bench_copy_run(kb, iters, "copy");
}

void profiling_bench_chase(uint32_t steps)
{
    uint64_t cyc;

    if (steps == 0u) steps = 8192u;

    if (s_bench_ok) {
        cyc = bench_chase_run((volatile uint32_t *)BENCH_ADDR, 16384u, steps);
        bench_chase_report("chase hyperram", cyc, steps);
    } else {
        printf("[BENCH] chase hyperram skipped: run 'bench canary' first\r\n");
    }

    cyc = bench_chase_run(s_chase_sram, 256u, steps);
    bench_chase_report("chase sram(cache-hot)", cyc, steps);
}

void profiling_bench_matrix(void)
{
    int l0 = bench_layer_on(LTDC_LAYER_1);
    int l1 = bench_layer_on(LTDC_LAYER_2);

    printf("[BENCH] matrix start: l0=%d l1=%d camera=on (1024 KB x5 each)\r\n", l0, l1);

    bench_read_run(1024u, 5u, "C4 l0+l1+cam");

    imx335_stop_capture();
    vTaskDelay(pdMS_TO_TICKS(50));

    bench_set_layers(1, 1);
    bench_read_run(1024u, 5u, "C3 l0+l1");
    bench_set_layers(1, 0);
    bench_read_run(1024u, 5u, "C1 l0");
    bench_set_layers(0, 1);
    bench_read_run(1024u, 5u, "C2 l1");
    bench_set_layers(0, 0);
    bench_read_run(1024u, 5u, "C0 none");

    bench_set_layers(l0, l1);
    imx335_start_capture((uint32_t)g_ltdc_framebuf);
    printf("[BENCH] matrix done, restored l0=%d l1=%d camera=on\r\n", l0, l1);
}

void profiling_bench_infer(uint32_t iters, uint32_t cfg)
{
    ai_result_t result;
    uint64_t cyc[BENCH_MAX_SAMPLES];
    int l0 = bench_layer_on(LTDC_LAYER_1);
    int l1 = bench_layer_on(LTDC_LAYER_2);

    if (iters == 0u) iters = 10u;
    if (iters > BENCH_MAX_SAMPLES) iters = BENCH_MAX_SAMPLES;

    if (cfg == 1u) {
        imx335_stop_capture();
        bench_set_layers(0, 0);
    } else if (cfg == 2u) {
        bench_set_layers(1, 1);
    }
    vTaskDelay(pdMS_TO_TICKS(50));

    for (uint32_t i = 0; i < iters; i++) {
        memset(&result, 0, sizeof(result));
        uint64_t t0 = profiling_now();
        MX_X_CUBE_AI_Process_User(&result);
        uint64_t t1 = profiling_now();
        cyc[i] = t1 - t0;
    }
    bench_report_us("infer", cyc, iters, cfg);

    bench_set_layers(l0, l1);
    if (cfg == 1u) imx335_start_capture((uint32_t)g_ltdc_framebuf);
}

void profiling_bench_reid(uint32_t iters, uint32_t cfg)
{
    float embedding[FACE_EMBEDDING_DIM];
    uint64_t cyc[BENCH_MAX_SAMPLES];
    int l0 = bench_layer_on(LTDC_LAYER_1);
    int l1 = bench_layer_on(LTDC_LAYER_2);

    if (iters == 0u) iters = 5u;
    if (iters > BENCH_MAX_SAMPLES) iters = BENCH_MAX_SAMPLES;

    if (cfg == 1u) {
        imx335_stop_capture();
        bench_set_layers(0, 0);
    } else if (cfg == 2u) {
        bench_set_layers(1, 1);
    }
    vTaskDelay(pdMS_TO_TICKS(50));

    for (uint32_t i = 0; i < iters; i++) {
        uint64_t t0 = profiling_now();
        ai_face_reid_run(embedding);
        uint64_t t1 = profiling_now();
        cyc[i] = t1 - t0;
    }
    bench_report_us("reid", cyc, iters, cfg);

    bench_set_layers(l0, l1);
    if (cfg == 1u) imx335_start_capture((uint32_t)g_ltdc_framebuf);
}

#endif /* HYPERRAM_BENCH_ENABLE */


void profiling_reset(void)
{
    uint32_t primask = prof_lock();

    memset(s_stage, 0, sizeof(s_stage));
    memset(s_net, 0, sizeof(s_net));
    s_iter = 0u;
    s_print_div = 0u;
    s_p1_prev = s_p1_frames;
    s_p2_prev = s_p2_frames;

#if LTDC_STATS_ENABLE
    s_fu_events = 0u;
    s_fuw_events = 0u;
    s_fu_poll = 0u;
    s_fuw_poll = 0u;
#endif

    prof_unlock(primask);

#if LTDC_STATS_ENABLE
    ltdc_mon_init();
#endif
}

void profiling_iter(void)
{
    uint32_t primask = prof_lock();
    s_iter++;
    prof_unlock(primask);
}

void profiling_stage(prof_stage_t stage, uint64_t cycles)
{
    if ((uint32_t)stage >= (uint32_t)PROF_STAGE_N) return;

    uint32_t primask = prof_lock();
    stat_add(&s_stage[stage], cycles);
    prof_unlock(primask);
}

void profiling_pipe_frame(uint32_t pipe)
{
    uint32_t primask = prof_lock();
    if (pipe == 1u) {
        s_p1_frames++;
    } else if (pipe == 2u) {
        s_p2_frames++;
    }
    prof_unlock(primask);
}

void profiling_npu_init(prof_net_t net, uint64_t cycles)
{
    if ((uint32_t)net >= (uint32_t)PROF_NET_N) return;

    uint32_t primask = prof_lock();
    stat_add(&s_net[net].init, cycles);
    prof_unlock(primask);
}

void profiling_npu_deinit(prof_net_t net, uint64_t cycles)
{
    if ((uint32_t)net >= (uint32_t)PROF_NET_N) return;

    uint32_t primask = prof_lock();
    stat_add(&s_net[net].deinit, cycles);
    prof_unlock(primask);
}

void profiling_npu_runblock(prof_net_t net, uint64_t cycles)
{
    if ((uint32_t)net >= (uint32_t)PROF_NET_N) return;

    uint32_t primask = prof_lock();
    stat_add(&s_net[net].runblock, cycles);
    prof_unlock(primask);
}

void profiling_npu_post(prof_net_t net, uint64_t cycles)
{
    if ((uint32_t)net >= (uint32_t)PROF_NET_N) return;

    uint32_t primask = prof_lock();
    stat_add(&s_net[net].post, cycles);
    prof_unlock(primask);
}

void profiling_wfe(prof_net_t net, uint64_t cycles)
{
    if ((uint32_t)net >= (uint32_t)PROF_NET_N) return;

    uint32_t primask = prof_lock();
    stat_add(&s_net[net].wfe, cycles);
    if (cycles < PROF_WFE_SHORT_CYC) s_net[net].wfe_short++;
    prof_unlock(primask);
}

void profiling_npu_wall(prof_net_t net, uint64_t cycles)
{
    if ((uint32_t)net >= (uint32_t)PROF_NET_N) return;

    uint32_t primask = prof_lock();
    s_net[net].wall_cycles += cycles;
    s_net[net].runs++;
    prof_unlock(primask);
}

void profiling_epoch(prof_net_t net, int is_sw, int is_hybrid, int is_blob, int is_internal)
{
    if ((uint32_t)net >= (uint32_t)PROF_NET_N) return;

    uint32_t primask = prof_lock();
    prof_net_stat_t *n = &s_net[net];

    if (is_sw) {
        n->ep_sw++;
    } else {
        n->ep_hw++;
    }
    if (is_hybrid)   n->ep_hybrid++;
    if (is_blob)     n->ep_blob++;
    if (is_internal) n->ep_internal++;

    prof_unlock(primask);
}

/* Per-epoch wall time, exclusive classification: SW, then hybrid, then internal,
 * everything else counts as pure HW. Mirrors the runtime flags used above. */
void profiling_epoch_time(prof_net_t net, int is_sw, int is_hybrid, int is_internal, uint64_t cycles)
{
    if ((uint32_t)net >= (uint32_t)PROF_NET_N) return;

    uint32_t primask = prof_lock();
    prof_net_stat_t *n = &s_net[net];
    prof_stat_t *st;

    if (is_sw) {
        st = &n->ept_sw;
    } else if (is_hybrid) {
        st = &n->ept_hyb;
    } else if (is_internal) {
        st = &n->ept_in;
    } else {
        st = &n->ept_hw;
    }
    stat_add(st, cycles);

    prof_unlock(primask);
}

static void print_net(const char *tag, const prof_net_stat_t *n)
{
    printf("[PROF %s] n=%lu wall=%luus | init=%lu deinit=%lu call=%lu wfe=%lu post=%lu us/frame"
           " | rcalls=%lu wfecalls=%lu short=%lu /frame"
           " | maxcall=%lu maxwfe=%lu"
           " | ep sw=%lu hw=%lu hyb=%lu bl=%lu in=%lu /frame"
           " | ept sw=%lu hw=%lu hyb=%lu in=%lu us/frame\r\n",
           tag,
           (unsigned long)n->runs,
           (unsigned long)net_wall_avg_us(n),
           (unsigned long)stat_total_per_run_us(&n->init, n->runs),
           (unsigned long)stat_total_per_run_us(&n->deinit, n->runs),
           (unsigned long)stat_total_per_run_us(&n->runblock, n->runs),
           (unsigned long)stat_total_per_run_us(&n->wfe, n->runs),
           (unsigned long)stat_total_per_run_us(&n->post, n->runs),
           (unsigned long)per_run_count(n->runblock.count, n->runs),
           (unsigned long)per_run_count(n->wfe.count, n->runs),
           (unsigned long)per_run_count(n->wfe_short, n->runs),
           (unsigned long)stat_max_us(&n->runblock),
           (unsigned long)stat_max_us(&n->wfe),
           (unsigned long)per_run_count(n->ep_sw, n->runs),
           (unsigned long)per_run_count(n->ep_hw, n->runs),
           (unsigned long)per_run_count(n->ep_hybrid, n->runs),
           (unsigned long)per_run_count(n->ep_blob, n->runs),
           (unsigned long)per_run_count(n->ep_internal, n->runs),
           (unsigned long)stat_total_per_run_us(&n->ept_sw, n->runs),
           (unsigned long)stat_total_per_run_us(&n->ept_hw, n->runs),
           (unsigned long)stat_total_per_run_us(&n->ept_hyb, n->runs),
           (unsigned long)stat_total_per_run_us(&n->ept_in, n->runs));
}

void profiling_print_window(void)
{
    prof_stat_t stage[PROF_STAGE_N];
    prof_net_stat_t net[PROF_NET_N];
    uint32_t iter;
    uint32_t p1;
    uint32_t p2;

    uint32_t primask = prof_lock();

    s_print_div++;
    if (s_print_div < PROF_PRINT_DIV) {
        prof_unlock(primask);
        return;
    }
    s_print_div = 0u;

    memcpy(stage, s_stage, sizeof(stage));
    memcpy(net, s_net, sizeof(net));
    iter = s_iter;
    p1 = s_p1_frames - s_p1_prev;
    p2 = s_p2_frames - s_p2_prev;
    s_p1_prev = s_p1_frames;
    s_p2_prev = s_p2_frames;

    memset(s_stage, 0, sizeof(s_stage));
    memset(s_net, 0, sizeof(s_net));
    s_iter = 0u;

    prof_unlock(primask);

    printf("[PROF AI] it=%lu p1=%lu p2=%lu | wait %lu/%lu cin %lu/%lu cout %lu/%lu pre %lu/%lu"
           " infer %lu/%lu cmd %lu/%lu reid %lu/%lu draw %lu/%lu dstop %lu/%lu tail %lu/%lu"
           " crop %lu/%lu fcrun %lu/%lu us(avg/max)\r\n",
           (unsigned long)iter,
           (unsigned long)p1,
           (unsigned long)p2,
           (unsigned long)stat_avg_us(&stage[PROF_WAIT]),
           (unsigned long)stat_max_us(&stage[PROF_WAIT]),
           (unsigned long)stat_avg_us(&stage[PROF_CACHE_IN]),
           (unsigned long)stat_max_us(&stage[PROF_CACHE_IN]),
           (unsigned long)stat_avg_us(&stage[PROF_CACHE_OUT]),
           (unsigned long)stat_max_us(&stage[PROF_CACHE_OUT]),
           (unsigned long)stat_avg_us(&stage[PROF_PRE]),
           (unsigned long)stat_max_us(&stage[PROF_PRE]),
           (unsigned long)stat_avg_us(&stage[PROF_NPU_F]),
           (unsigned long)stat_max_us(&stage[PROF_NPU_F]),
           (unsigned long)stat_avg_us(&stage[PROF_CMD]),
           (unsigned long)stat_max_us(&stage[PROF_CMD]),
           (unsigned long)stat_avg_us(&stage[PROF_REID]),
           (unsigned long)stat_max_us(&stage[PROF_REID]),
           (unsigned long)stat_avg_us(&stage[PROF_DRAW]),
           (unsigned long)stat_max_us(&stage[PROF_DRAW]),
           (unsigned long)stat_avg_us(&stage[PROF_DRAW_STOP]),
           (unsigned long)stat_max_us(&stage[PROF_DRAW_STOP]),
           (unsigned long)stat_avg_us(&stage[PROF_TAIL]),
           (unsigned long)stat_max_us(&stage[PROF_TAIL]),
           (unsigned long)stat_avg_us(&stage[PROF_CROP]),
           (unsigned long)stat_max_us(&stage[PROF_CROP]),
           (unsigned long)stat_avg_us(&stage[PROF_FC_RUN]),
           (unsigned long)stat_max_us(&stage[PROF_FC_RUN]));

    print_net("f ", &net[PROF_NET_F]);
    print_net("fc", &net[PROF_NET_FC]);

#if LTDC_STATS_ENABLE
    {
        const uint32_t mask = LTDC_ISR_FUIF | LTDC_ISR_FUWIF;
        uint32_t seen = (LTDC->ISR & mask) | (LTDC->ISR2 & mask);
        uint32_t fu_ev, fuw_ev, fu_poll, fuw_poll;

        primask = prof_lock();
        if (seen & LTDC_ISR_FUIF)  s_fu_poll++;
        if (seen & LTDC_ISR_FUWIF) s_fuw_poll++;
        fu_ev    = s_fu_events;
        fuw_ev   = s_fuw_events;
        fu_poll  = s_fu_poll;
        fuw_poll = s_fuw_poll;
        s_fu_events = 0u;
        s_fuw_events = 0u;
        s_fu_poll = 0u;
        s_fuw_poll = 0u;
        prof_unlock(primask);

        LTDC->ICR  = mask;
        LTDC->ICR2 = mask;

        printf("[PROF LTDC] fu_ev=%lu fuw_ev=%lu | fu_poll=%lu fuw_poll=%lu /window\r\n",
               (unsigned long)fu_ev, (unsigned long)fuw_ev,
               (unsigned long)fu_poll, (unsigned long)fuw_poll);
    }
#endif
}

#endif /* PROFILING_ENABLE */
