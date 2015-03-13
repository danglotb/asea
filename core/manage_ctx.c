#include "manage_ctx.h"


static struct ctx_s *ctx_wait_hda = NULL;

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
int create_ctx(int stack_size, func_t *f, void *args, int n_core) {
	struct ctx_s *new = malloc(sizeof(struct ctx_s));
	irq_disable();
	/* malloc failed */	
	if(!new)
		return 0;
	/* 1rst element */ 
	if (head[n_core] == NULL) {
		head[n_core] = new;
		head[n_core]->next_ctx = head[n_core];
		head[n_core]->assigned_core = n_core;
		return _init_ctx(head[n_core], stack_size, f, args);
	} else {
		new->next_ctx = head[n_core]->next_ctx;
		head[n_core]->next_ctx = new;
		new->assigned_core = n_core;
		return _init_ctx(new, stack_size, f, args);
	}
}

void init_ring_weight() {

}

/* cree un contexte assigné à un core, l'index de ce core est retourné */
/* on prend par défault NULL pour les argument et 16634 en taille de stack */
int add_ctx_to_assigned_core(func_t *f, unsigned int weight) {
	unsigned int i, max = -1;
	for (i = 0 ; i < NUMBER_CORE ; i++) {
		if (assigned_weight[i].weight > max || max == -1)
			max = i;
	}
	assigned_weight[i].weight += weight;
	create_ctx(16634, f, NULL, max);
	return max;
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

void _switch_to_init_ctx() {
	unsigned int n_core = _in(CORE_ID);
	asm( "movl %0,%%esp" "\n\t" "movl %1,%%ebp"
			:
			: "r" (init_ctx[n_core].esp), "r" (init_ctx[n_core].ebp)
	   );
}

void _save_init_ctx() {
	unsigned int n_core = _in(CORE_ID);
	asm( "movl %%esp,%0" "\n\t" "movl %%ebp,%1"
				: "=r" (init_ctx[n_core].esp), "=r" (init_ctx[n_core].ebp)
	);
}


/* appel caché a yield, et init les interruptions */
void start_sched() {
	irq_enable();
	_yield();
}

void _switch_to_ctx(struct ctx_s *ctx ){
	static int n_core;
	n_core = _in(CORE_ID);

	assert(ctx->ctx_magic == CTX_MAGIC);
	irq_disable();

	/* init variable contexte courant (contexte appelant) */
	if (current_ctx[n_core] != NULL) {
		/* Sauvegarde de l'état du contexte courrant */
		asm( "movl %%esp,%0" "\n\t" "movl %%ebp,%1"
				: "=r" (current_ctx[n_core]->esp), "=r" (current_ctx[n_core]->ebp)
	   	);
	}

	current_ctx[n_core] = ctx;


	/* Changement des registres et donc du contexte courant */
	asm( "movl %0,%%esp" "\n\t" "movl %1,%%ebp"
			:
			: "r" (ctx->esp), "r" (ctx->ebp)
	   );

	irq_enable();

	if (current_ctx[n_core]->status == READY) {
		current_ctx[n_core]->status = ACTIVATED;
		current_ctx[n_core]->f(current_ctx[n_core]->args);
		current_ctx[n_core]->status = TERMINATED;
		_yield();
	}
	return;
}

/* appel caché du switch_to_ctx */
void _yield() {
	static int n_core;
	n_core = _in(CORE_ID);
	if (head[n_core] == NULL) return;
	if (current_ctx[n_core] == NULL) {	
		_switch_to_ctx(head[n_core]);
	} else {

		/* si il n'y a qu'un seul ctx on ne fait rien */
		if (current_ctx[n_core]->next_ctx == current_ctx[n_core])return;

		/* traitement des ctx termines */
		while (current_ctx[n_core]->status == TERMINATED) {
			struct ctx_s *tmp, *pred;
			if (current_ctx[n_core] == head[n_core])
				head[n_core] = current_ctx[n_core]->next_ctx;
			tmp = current_ctx[n_core];
			current_ctx[n_core] = current_ctx[n_core]->next_ctx;

			if(tmp->next_ctx == current_ctx[n_core]) {
				free(tmp->stack);
				free(tmp);
				exit(EXIT_SUCCESS);
			}
	
			/* seek of pred */
			pred = tmp->next_ctx;
			while (pred->next_ctx != tmp)
				pred = pred->next_ctx;
			pred->next_ctx = tmp->next_ctx;

			free(tmp->stack);
			free(tmp);
		}

		/* detection d'interblocage */
		if (current_ctx[n_core]->next_ctx->status == BLOCKED) {
			struct ctx_s *tmp = current_ctx[n_core]->next_ctx;
			while (tmp->status == BLOCKED) { 
				tmp = current_ctx[n_core]->next_ctx;
				if (tmp == current_ctx[n_core])
					exit(EXIT_FAILURE);
			}
			_switch_to_ctx(tmp);
		}
		/* sinon on switch simplement sur le ctx suivant */
		_switch_to_ctx(current_ctx[n_core]->next_ctx);
	}
}

void sem_down(struct sem_s *sem) {
	int n_core = _in(CORE_ID);
	irq_disable();
	sem->value--;
	if(sem->value < 0) {
		current_ctx[n_core]->status = BLOCKED;
		if (sem->head_wait != NULL) {
			struct ctx_s *tmp;
			tmp = sem->head_wait->next_wait;
			while (tmp->next_wait != NULL)
				tmp = tmp->next_wait;
			tmp->next_wait = current_ctx[n_core];
		} else 
			sem->head_wait = current_ctx[n_core];

		irq_enable();
		_yield();
	}
	irq_enable();
}

void mtx_lock(struct mtx_s *mutex) {
	sem_down(&(mutex->sem));
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
	sem_up(&(mutex->sem));
}


void irq_disable() {
	_mask(0xF);
}

void irq_enable() {
	_mask(0);
}

void hda_request() {
	int n_core = _in(CORE_ID);
	ctx_wait_hda = current_ctx[n_core];
	current_ctx[n_core]->status = HDA_WAIT;
	_yield();	
}

void hda_end_request() {
	if (ctx_wait_hda != NULL) {
		ctx_wait_hda->status = ACTIVATED;
		//_switch_to_ctx(ctx_wait_hda);
	}
}