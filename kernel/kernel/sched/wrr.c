#include "sched.h"

#include <linux/slab.h>
static void switched_to_wrr(struct rq *rq, struct task_struct *p)
{
	/* Do nothing */
}

static void set_curr_task_wrr(struct rq *rq)
{
	/* Do nothing */
}
static void put_prev_task_wrr(struct rq *rq, struct task_struct *p)
{
	/* Do nothiing */
}

static void 
check_preempt_curr_wrr(struct rq *rq, struct task_struct *p, int flags)
{
	/* Do nothiing */
	/* Since there is no priority in wrr scheduler, we don't have to 
	 * schedule the new task and preempt the exsiting one.
	 */
}

static inline struct task_struct *wrr_task_of(struct sched_wrr_entity *wrr_se)
{
	return container_of(wrr_se, struct task_struct, wrr);
}

static struct task_struct *pick_next_task_wrr(struct rq *rq)
{
	struct task_struct *p;
	struct wrr_rq *wrr_rq;
	struct sched_wrr_entity *next = NULL;

	wrr_rq = &rq->wrr;

	next = list_entry(wrr_rq->queue.next, 
			  struct sched_wrr_entity, run_list);
	p = wrr_task_of(next);
	
	return p;
}

static void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
	struct sched_wrr_entity *wrr_se = &p->wrr;	
	struct wrr_rq *wrr_rq = &rq->wrr;

	if (wrr_se == NULL) 
		return;

	list_del_init(&wrr_se->run_list);	
	wrr_rq->wrr_nr_running--;
	dec_nr_running(rq);
}

static void
enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags)
{
	struct sched_wrr_entity *wrr_se = &p->wrr;
	struct wrr_rq *wrr_rq = &rq->wrr;
	
	if (wrr_se == NULL) 
		return;

	if (flags & ENQUEUE_HEAD)
		list_add(&wrr_se->run_list, &wrr_rq->queue);
	else
		list_add_tail(&wrr_se->run_list, &wrr_rq->queue);
	wrr_rq->wrr_nr_running++;
	inc_nr_running(rq);
}

static void requeue_task_wrr(struct rq *rq, struct task_struct *p, int head)
{
	struct sched_wrr_entity *wrr_se = &p->wrr;
	struct wrr_rq *wrr_rq = &rq->wrr;
	
	if (wrr_se == NULL) 
		return;
	
	list_move_tail(&wrr_se->run_list, &wrr_rq->queue);
}

static void task_tick_wrr(struct rq *rq, struct task_struct *p, int queued)
{
	struct sched_wrr_entity *wrr_se = &p->wrr;

	if (wrr_se == NULL) 
		return;

	if (--p->wrr.time_slice)
		return;
	
	if (wrr_se->weight > 1)
		wrr_se->weight--;
	wrr_se->time_slice = wrr_se->weight * 10;

	if (wrr_se->run_list.prev != wrr_se->run_list.next) {
		requeue_task_wrr(rq, p, 0);
		set_tsk_need_resched(p);
		return;
	}
}

/*
 * Update the current task's runtime statistics. Skip current tasks that
 * are not in our scheduling class.
 */
const struct sched_class wrr_sched_class = {
	.next			= &fair_sched_class,
	.enqueue_task		= enqueue_task_wrr,
	.dequeue_task		= dequeue_task_wrr,

	.check_preempt_curr	= check_preempt_curr_wrr,

	.pick_next_task		= pick_next_task_wrr,
	.put_prev_task		= put_prev_task_wrr,

#ifdef CONFIG_SMP
	/*
	.select_task_rq		= select_task_rq_rt,

	.set_cpus_allowed       = set_cpus_allowed_rt,
	.rq_online              = rq_online_rt,
	.rq_offline             = rq_offline_rt,
	.pre_schedule		= pre_schedule_rt,
	.post_schedule		= post_schedule_rt,
	.task_woken		= task_woken_rt,
	.switched_from		= switched_from_rt,
	*/
#endif

	.set_curr_task          = set_curr_task_wrr,
	.task_tick		= task_tick_wrr,

	.switched_to		= switched_to_wrr,
};
