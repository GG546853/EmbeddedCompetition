#ifndef PROFILING_H
#define PROFILING_H

#include <stdint.h>

#ifndef PROFILING_ENABLE
#define PROFILING_ENABLE 1
#endif

typedef enum {
    PROF_WAIT = 0,
    PROF_CACHE_IN,
    PROF_PRE,
    PROF_CACHE_OUT,
    PROF_NPU_F,
    PROF_CMD,
    PROF_REID,
    PROF_DRAW,
    PROF_DRAW_STOP,
    PROF_TAIL,
    PROF_STAGE_N
} prof_stage_t;

typedef enum {
    PROF_NET_F = 0,
    PROF_NET_FC,
    PROF_NET_N
} prof_net_t;

#if PROFILING_ENABLE

uint64_t profiling_now(void);
uint32_t profiling_tick_now(void);
void profiling_reset(void);
void profiling_iter(void);
void profiling_stage(prof_stage_t stage, uint64_t cycles);
void profiling_pipe_frame(uint32_t pipe);
void profiling_npu_init(prof_net_t net, uint64_t cycles);
void profiling_npu_deinit(prof_net_t net, uint64_t cycles);
void profiling_npu_runblock(prof_net_t net, uint64_t cycles);
void profiling_npu_post(prof_net_t net, uint64_t cycles);
void profiling_wfe(prof_net_t net, uint64_t cycles);
void profiling_npu_wall(prof_net_t net, uint32_t ticks);
void profiling_epoch(prof_net_t net, int is_sw, int is_hybrid, int is_blob, int is_internal);
void profiling_print_window(void);

#else

#define profiling_now()                  (0ULL)
#define profiling_tick_now()             (0U)
#define profiling_reset()                ((void)0)
#define profiling_iter()                 ((void)0)
#define profiling_stage(s, c)            ((void)(s), (void)(c))
#define profiling_pipe_frame(p)          ((void)(p))
#define profiling_npu_init(n, c)         ((void)(n), (void)(c))
#define profiling_npu_deinit(n, c)       ((void)(n), (void)(c))
#define profiling_npu_runblock(n, c)     ((void)(n), (void)(c))
#define profiling_npu_post(n, c)         ((void)(n), (void)(c))
#define profiling_wfe(n, c)              ((void)(n), (void)(c))
#define profiling_npu_wall(n, t)         ((void)(n), (void)(t))
#define profiling_epoch(n, sw, hy, bl, in) \
    ((void)(n), (void)(sw), (void)(hy), (void)(bl), (void)(in))
#define profiling_print_window()         ((void)0)

#endif

#endif /* PROFILING_H */
