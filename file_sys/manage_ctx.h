#ifndef _MANAGE_CTX_H
#define _MANAGE_CTX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hardware_config.h"
#include "include/hardware.h"

#define CTX_MAGIC 0x42
#define SEM_MAGIC 0x23

#define NUMBER_CORE 4;

typedef void (func_t) (void*);

struct ctx_s {
	void *ebp;
	void *esp;
	unsigned int ctx_magic;
	func_t *f;
	void *args;
	unsigned char *stack;
	int status;
	struct ctx_s *next_ctx;
	struct ctx_s *next_wait;
	unsigned int assigned_core;
};

struct sem_s {
	unsigned sem_magic;
	int value;
	struct ctx_s *head_wait;
};

struct mtx_s {
	struct sem_s sem;
	struct ctx_s *owner;
};

struct queue_hda_s {
	struct ctx_s *queue_head;
	struct queue_hda_s *queue_next;
};

struct pool_ctx_s {
	struct ctx_s ctx;
	unsigned int charge;
};

struct pool_ctx_s *pool [NUMBER_CORE];

struct ctx_s *head [NUMBER_CORE];

enum status {READY,ACTIVATED,TERMINATED,BLOCKED,HDA_WAIT};

#define dump_sp() \
	do { 	\
		void *desp, *debp; \
		asm("movl %%esp , %0", "\n\t" , "movl %%ebp , %1" \
			: "=r" (deps) , "=r" (debp) 		\
			);					\
		printf("ESP : %p , EBP : %p", desp, debp);	\
	} while (0);

int _init_ctx (struct ctx_s *pctx, int stack_size, func_t f, void *args);
int create_ctx(int stack_size, func_t f, void *args, int n_core);
void sem_init(struct sem_s *sem, unsigned int val);
void mtx_init(struct mtx_s *mutex);

void _switch_to_ctx(struct ctx_s *ctx);
void _yield();
void start_sched();

void sem_down(struct sem_s *sem);
void mtx_lock(struct mtx_s *mutex);
void sem_up(struct sem_s *sem);
void mtx_unlock(struct mtx_s *mutex);

void irq_disable();
void irq_enable();

void hda_request();
void hda_end_request();

#endif