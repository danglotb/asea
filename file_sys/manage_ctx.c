#include "manage_ctx.h"

static struct ctx_s *current_ctx;

struct ctx_s *queue_hda = NULL;

/* initialisation de contexte */
int _init_ctx (struct ctx_s *pctx, int stack_size, func_t *f, void *args) {
	pctx->stack = (unsigned char*)malloc(stack_size);
	if (!pctx->stack)
		return 0;
	pctx->ebp = &pctx->stack[stack_size - 4];
	pctx->esp = &pctx->stack[stack_size - 4];
	pctx->status = READY;
	pctx->ctx_magic = CTX_MAGIC;
	pctx->f = f;
	pctx->args = args;
	pctx->next_wait = NULL;
	irq_enable();
	return 1;
}

/* creation caché des contextes (utilisations de init_ctx) */
int create_ctx(int stack_size, func_t *f, void *args) {
	struct ctx_s *new = malloc(sizeof(struct ctx_s));
	irq_disable();
	/* malloc failed */	
	if(!new)
		return 0;
	/* 1rst element */ 
	if (head == NULL) {
		head = new;
		head->next_ctx = head;
		return _init_ctx(head, stack_size, f, args);
	} else {
		new->next_ctx = head->next_ctx;
		head->next_ctx = new;
		return _init_ctx(new, stack_size, f, args);
	}

}

/* initialisation d'un semaphore  Exercice 12 */
void sem_init(struct sem_s *sem, unsigned int val) {
	sem->sem_magic = SEM_MAGIC;	
	sem->value = val;
	sem->head_wait = NULL;//pas de tache qui attend sur le semaphore
}

/* initilisation d'un mutex (appel de sem_init()) */
void mtx_init(struct mtx_s *mutex) {
	sem_init(&(mutex->sem), 1);
	mutex->owner = NULL;
}


void _switch_to_ctx(struct ctx_s *ctx ){
	assert(ctx->ctx_magic == CTX_MAGIC);
	irq_disable();
	while (ctx->status == TERMINATED) {
		if (ctx->next_ctx == ctx)
			exit(EXIT_SUCCESS);
		if (ctx == head)
			head = ctx->next_ctx;
		current_ctx->next_ctx = ctx->next_ctx;
		free(ctx->stack);
		free(ctx);
		ctx = current_ctx->next_ctx;
	}

	/* init variable contexte courant (contexte appelant) */
	if (current_ctx != NULL) {
		/* Sauvegarde de l'état du contexte courrant */
		asm( "movl %%esp,%0" "\n\t" "movl %%ebp,%1"
				: "=r" (current_ctx->esp), "=r" (current_ctx->ebp)
	   	);
	}

	current_ctx = ctx;

	/* Changement des registres et donc du contexte courant */
	asm( "movl %0,%%esp" "\n\t" "movl %1,%%ebp"
			:
			: "r" (ctx->esp), "r" (ctx->ebp)
	   );
	irq_enable();
	if (current_ctx->status == BLOCKED || current_ctx->status == HDA_WAIT) {
		struct ctx_s *tmp = current_ctx;
		while (tmp->status == BLOCKED || tmp->status == HDA_WAIT) { 
			tmp = current_ctx->next_ctx;
			if (tmp == current_ctx)
				exit(EXIT_FAILURE);
		}
		_switch_to_ctx(tmp);
	} else if (current_ctx->status == READY) {
		current_ctx->status = ACTIVATED;
		/*irq_enable();*/
		current_ctx->f(current_ctx->args);
		current_ctx->status = TERMINATED;
		_yield();
	}
	return;
}

/* appel caché du switch_to_ctx */
void _yield() {
	if (head == NULL) return;
	if (current_ctx == NULL) {	
		_switch_to_ctx(head);
	} else {
		if (current_ctx->next_ctx == current_ctx) return;
		_switch_to_ctx(current_ctx->next_ctx);
	} 
}

/* appel caché a yield, et init les interruptions */
void start_sched() {
/*	setup_irq(TIMER_IRQ, _yield);
	start_hw();
*/	irq_enable();
	_yield();
}

void sem_down(struct sem_s *sem) {
	irq_disable();
	sem->value--;
	if(sem->value < 0) {
		current_ctx->status = BLOCKED;
		if (sem->head_wait != NULL) {
			struct ctx_s *tmp;
			tmp = sem->head_wait->next_wait;
			while (tmp->next_wait != NULL)
				tmp = tmp->next_wait;
			tmp->next_wait = current_ctx;
		} else 
			sem->head_wait = current_ctx;
		irq_enable();
		_yield();
	}
	irq_enable();
}

void mtx_lock(struct mtx_s *mutex) {
	sem_down(&(mutex->sem));
//	if (current_ctx->status != BLOCKED)
//		mutex->owner = current_ctx;
}

void sem_up(struct sem_s *sem){
	irq_disable();
	sem->value++;
	if(sem->head_wait != NULL) { 
		sem->head_wait->status = ACTIVATED;
		sem->head_wait = sem->head_wait->next_wait;
	}
	irq_enable();
}

void mtx_unlock(struct mtx_s *mutex) {
//	if (mutex->owner == current_ctx) {
		sem_up(&(mutex->sem));
//		mutex->owner = mutex->sem->head_wait;
//	}
}


void irq_disable() {
	/*_out(TIMER_PARAM, 32+8);
*/	_mask(0xF);
}

void irq_enable() {
	/*_out(TIMER_PARAM, 64+32+8);*/
	_mask(0);
}

void hda_request() {
	if (queue_hda == NULL) {
		queue_hda = current_ctx;
		queue_hda->next_ctx = queue_hda;
	} else {
		struct ctx_s *tmp = queue_hda;
		while (tmp->next_ctx != queue_hda) 
			tmp = tmp->next_ctx;
		tmp->next_ctx = current_ctx;
		current_ctx->next_ctx = queue_hda;
	}
	current_ctx->status = HDA_WAIT;
	_yield();	
}