#include "profiling.h"

#if PROFILING_ENABLE

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <string.h>

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
    uint64_t    wall_ticks;
    uint32_t    runs;
    uint32_t    wfe_short;
    uint32_t    ep_sw;
    uint32_t    ep_hw;
    uint32_t    ep_hybrid;
    uint32_t    ep_blob;
    uint32_t    ep_internal;
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
    return (uint32_t)((n->wall_ticks * (1000000u / configTICK_RATE_HZ)) / n->runs);
}

uint64_t profiling_now(void)
{
    return getRunTimeCounterValue();
}

uint32_t profiling_tick_now(void)
{
    return (uint32_t)xTaskGetTickCount();
}

void profiling_reset(void)
{
    uint32_t primask = prof_lock();

    memset(s_stage, 0, sizeof(s_stage));
    memset(s_net, 0, sizeof(s_net));
    s_iter = 0u;
    s_print_div = 0u;
    s_p1_prev = s_p1_frames;
    s_p2_prev = s_p2_frames;

    prof_unlock(primask);
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

void profiling_npu_wall(prof_net_t net, uint32_t ticks)
{
    if ((uint32_t)net >= (uint32_t)PROF_NET_N) return;

    uint32_t primask = prof_lock();
    s_net[net].wall_ticks += ticks;
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

static void print_net(const char *tag, const prof_net_stat_t *n)
{
    printf("[PROF %s] n=%lu wall=%luus | init=%lu deinit=%lu call=%lu wfe=%lu post=%lu us/frame"
           " | rcalls=%lu wfecalls=%lu short=%lu /frame"
           " | maxcall=%lu maxwfe=%lu"
           " | ep sw=%lu hw=%lu hyb=%lu bl=%lu in=%lu /frame\r\n",
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
           (unsigned long)per_run_count(n->ep_internal, n->runs));
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
           " infer %lu/%lu cmd %lu/%lu reid %lu/%lu draw %lu/%lu dstop %lu/%lu tail %lu/%lu us(avg/max)\r\n",
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
           (unsigned long)stat_max_us(&stage[PROF_TAIL]));

    print_net("f ", &net[PROF_NET_F]);
    print_net("fc", &net[PROF_NET_FC]);
}

#endif /* PROFILING_ENABLE */
