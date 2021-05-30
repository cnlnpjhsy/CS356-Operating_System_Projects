/*
 * Weighted Round-Robin Scheduling Class 
 * (mapped to the SCHED_WRR policy)
 */

#include "sched.h"

#include <linux/slab.h>

void init_wrr_rq(struct wrr_rq *wrr_rq)
{
    printk("> init_wrr_rq\n");
    INIT_LIST_HEAD(&wrr_rq->queue);
    wrr_rq->wrr_nr_running = 0;
}

void free_wrr_sched_group(struct task_group *tg) {}

int alloc_wrr_sched_group(struct task_group *tg, struct task_group *parent)
{
    return 1;
}

static void update_curr_wrr(struct rq *rq)
{
    // printk("> update_curr_wrr\n");
    struct task_struct *curr = rq->curr;
    struct wrr_rq *wrr_rq = &rq->wrr;
    u64 delta_exec;

    if (curr->sched_class != &wrr_sched_class)
        return;
    // printk(">> update_curr_wrr Check1\n");

    /* runtime for this time */
    delta_exec = rq->clock_task - curr->se.exec_start;
    if (unlikely((s64)delta_exec < 0))
        delta_exec = 0;

    schedstat_set(curr->se.statistics.exec_max,
                  max(curr->se.statistics.exec_max, delta_exec));
    
    /* total runtime */
    curr->se.sum_exec_runtime += delta_exec;
    /* update start time */
    curr->se.exec_start = rq->clock_task;

    /* rt(wrr)_bandwidth_enabled() == false; thus return */
    // printk(">> update_curr_wrr done.\n");
}

static int wrr_is_foreground(struct task_struct *p)
{
    // printk("> wrr_is_foreground, the string is:");
    struct task_group *tg = p->sched_task_group;
    char *path = task_group_path(tg);
    // printk("%s\n", path);
    if (path[1] != 'b') /* background: "/bg_non_interactive" */
        return 1;
    else                /* foreground: "/" */
        return 0;
}

static void
enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
    printk("> enqueue_task_wrr\n");
    struct sched_wrr_entity *wrr_se = &p->wrr;
    struct wrr_rq *wrr_rq = &rq->wrr;
    struct list_head *queue = &wrr_rq->queue;

    if(flags & ENQUEUE_WAKEUP)
        wrr_se->timeout = 0;

    if(flags & ENQUEUE_HEAD){
        // printk(">> enqueue head\n");
        list_add(&wrr_se->run_list, queue);
    }
    else{
        // printk(">> enqueue tail\n");
        list_add_tail(&wrr_se->run_list, queue);
    }

    wrr_rq->wrr_nr_running++;
    inc_nr_running(rq);
}

static void
dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
    printk("> dequeue_task_wrr\n");
    struct sched_wrr_entity *wrr_se = &p->wrr;
    struct wrr_rq *wrr_rq = &rq->wrr;

    update_curr_wrr(rq);

    list_del_init(&wrr_se->run_list);

    wrr_rq->wrr_nr_running--;
    dec_nr_running(rq);
}

static void
yield_task_wrr(struct rq *rq)
{
    printk("> yield_task_wrr\n");
    struct task_struct *p = rq->curr;
    struct sched_wrr_entity *wrr_se = &p->wrr;
    struct wrr_rq *wrr_rq = &rq->wrr;
    struct list_head *queue = &wrr_rq->queue;

    if(!list_empty(&wrr_se->run_list))
        list_move_tail(&wrr_se->run_list, queue);
}

static void
check_preempt_curr_wrr(struct rq *rq, struct task_struct *p, int flags)
{
    printk("> check_preempt_curr_wrr\n");
    if (p->prio < rq->curr->prio){
        resched_task(rq->curr);
		return;
    }
}

static struct task_struct *pick_next_task_wrr(struct rq *rq)
{
    // printk("> pick_next_task_wrr\n");
    struct sched_wrr_entity *wrr_se;
    struct task_struct *p;
    struct wrr_rq *wrr_rq = &rq->wrr;
    struct list_head *queue = &wrr_rq->queue;

    if(!wrr_rq->wrr_nr_running)
        return NULL;

    wrr_se = list_entry(queue->next, struct sched_wrr_entity, run_list);

    p = container_of(wrr_se, struct task_struct, wrr);
    if(!p)
        return NULL;
    p->se.exec_start = rq->clock_task;
    // printk(">> pick_next_task_wrr done.\n");

    return p;
}

static void
put_prev_task_wrr(struct rq *rq, struct task_struct *p)
{
    // printk("> put_prev_task_wrr\n");
    update_curr_wrr(rq);
    p->se.exec_start = 0;
}

#ifdef CONFIG_SMP
static int
select_task_rq_wrr(struct task_struct *p, int sd_flag, int flags) {}

static void set_cpus_allowed_wrr(struct task_struct *p,
				const struct cpumask *new_mask) {}

static void rq_online_wrr(struct rq *rq) {}

static void rq_offline_wrr(struct rq *rq) {}

static void pre_schedule_wrr(struct rq *rq, struct task_struct *prev) {}

static void post_schedule_wrr(struct rq *rq) {}

static void task_woken_wrr(struct rq *rq, struct task_struct *p) {}

#endif

static void switched_from_wrr(struct rq *rq, struct task_struct *p) {}

static void set_curr_task_wrr(struct rq *rq)
{
    printk("> set_curr_task_wrr\n");
    struct task_struct *p = rq->curr;

    p->se.exec_start = rq->clock_task;
}

static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued)
{
    // printk("> task_tick_wrr\n");
    struct sched_wrr_entity *wrr_se = &p->wrr;
    struct wrr_rq *wrr_rq = &rq->wrr;
    struct list_head *queue = &wrr_rq->queue;

    update_curr_wrr(rq);

    if (p->policy != SCHED_WRR)
        return;

    if(--p->wrr.time_slice)
        return;

    if(wrr_is_foreground(p))
        p->wrr.time_slice = WRR_FG_TIMESLICE;
    else
        p->wrr.time_slice = WRR_BG_TIMESLICE;

    if (wrr_se->run_list.prev != wrr_se->run_list.next){
        // printk(">> task_tick_wrr 2\n");
        /* Requeue to the end of queue */
        list_move_tail(&wrr_se->run_list, queue);
        set_tsk_need_resched(p);
    }
}

static unsigned int 
get_rr_interval_wrr(struct rq *rq, struct task_struct *task)
{
    // printk("> get_rr_interval_wrr\n");
    if (task->policy == SCHED_WRR)
    {
        if(wrr_is_foreground(task))
            return WRR_FG_TIMESLICE;
        else
            return WRR_BG_TIMESLICE;
    }
    else
        return 0;
}

static void
prio_changed_wrr(struct rq *rq, struct task_struct *p, int oldprio) {}

static void
switched_to_wrr(struct rq *rq, struct task_struct *p)
{
    printk("> switched_to_wrr\n");
    if (p->on_rq && rq->curr != p && p->prio < rq->curr->prio){
        printk(">> switched_to_wrr resched\n");
        resched_task(rq->curr);
    }       
}

const struct sched_class wrr_sched_class = {
    .next = &fair_sched_class,                    /* Required */
    .enqueue_task = enqueue_task_wrr,             /* Required */
    .dequeue_task = dequeue_task_wrr,             /* Required */
    .yield_task = yield_task_wrr,                 /* Required */
    .check_preempt_curr = check_preempt_curr_wrr, /* Required */

    .pick_next_task = pick_next_task_wrr, /* Required */
    .put_prev_task = put_prev_task_wrr,   /* Required */

#ifdef CONFIG_SMP
    .select_task_rq = select_task_rq_wrr, /* Never need impl */
    .set_cpus_allowed = set_cpus_allowed_wrr,
    .rq_online = rq_online_wrr,         /* Never need impl */
    .rq_offline = rq_offline_wrr,       /* Never need impl */
    .pre_schedule = pre_schedule_wrr,   /* Never need impl */
    .post_schedule = post_schedule_wrr, /* Never need impl */
    .task_woken = task_woken_wrr,       /* Never need impl */
#endif
    .switched_from = switched_from_wrr, /* Never need impl */

    .set_curr_task = set_curr_task_wrr, /* Required */
    .task_tick = task_tick_wrr,         /* Required */

    .get_rr_interval = get_rr_interval_wrr,

    .prio_changed = prio_changed_wrr, /* Never need impl */
    .switched_to = switched_to_wrr,   /* Never need impl */
};

/* 5/29: Debug complete */

/* 5/17: wrr.c complete */