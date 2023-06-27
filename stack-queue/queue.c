#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}							\
	} while (0)

#define MAX_STRING_SIZE 64


/* ---------------------------------------- TODO: QUEUE IMPLEMENTATION ----------------------------------------------*/


typedef struct queue_t queue_t;
struct queue_t
{
	/* Dimensiunea maxima a cozii */
	unsigned int max_size;
	/* Dimensiunea cozii */
	unsigned int size;
	/* Dimensiunea in octeti a tipului de date stocat in coada */
	unsigned int data_size;
	/* Indexul de la care se vor efectua operatiile de front si dequeue */
	unsigned int read_idx;
	/* Indexul de la care se vor efectua operatiile de enqueue */
	unsigned int write_idx;
	/* Bufferul ce stocheaza elementele cozii */
	void **buff;
};

queue_t *
q_create(unsigned int data_size, unsigned int max_size)
{
	queue_t *queue =  malloc(sizeof(queue_t));
    queue->max_size = max_size;
    queue->size = 0;
    queue->data_size = data_size;
    queue->read_idx = 0;
    queue->write_idx = 0;
    queue->buff = malloc(sizeof(void*) * queue->max_size);
    for (int i = 0 ; i < max_size; i++) {
        queue->buff[i] = malloc(data_size);
    }
	return queue;
}

/*
 * Functia intoarce numarul de elemente din coada al carei pointer este trimis
 * ca parametru.
 */
unsigned int
q_get_size(queue_t *q)
{
	return q->size;
}

/*
 * Functia intoarce 1 daca coada este goala si 0 in caz contrar.
 */
unsigned int
q_is_empty(queue_t *q)
{
	if (q->size == 0)
        return 1;

	return 0;
}

/* 
 * Functia intoarce primul element din coada, fara sa il elimine.
 */
void *
q_front(queue_t *q)
{
    return q->buff[q->read_idx];
}

/*
 * Functia scoate un element din coada. Se va intoarce 1 daca operatia s-a
 * efectuat cu succes (exista cel putin un element pentru a fi eliminat) si
 * 0 in caz contrar.
 */
int
q_dequeue(queue_t *q)
{

    if (q->size == 0) {
        return 0;
    }
    free(q->buff[q->read_idx]);
    q->read_idx = (q->read_idx + 1) % q->max_size;
    q->size--;

    return 1;

}

/* 
 * Functia introduce un nou element in coada. Se va intoarce 1 daca
 * operatia s-a efectuat cu succes (nu s-a atins dimensiunea maxima) 
 * si 0 in caz contrar.
 */
int enqueue(queue_t *queue, void *data)
{
    if (queue->size == queue->max_size) {
        return 0;
    }

    memcpy(queue->buff[queue->write_idx], data, queue->data_size);

    queue->write_idx = (queue->write_idx + 1) % queue->max_size;

    queue->size++;

    return 1;
}


/*
 * Functia elimina toate elementele din coada primita ca parametru.
 */
void
q_clear(queue_t *q)
{
	while (q->size > 0) {
        q_dequeue(q);
    }
}

/*
 * Functia elibereaza toata memoria ocupata de coada.
 */
void
q_free(queue_t *q)
{
    while (q->size > 0) {
        void *data = q_dequeue(q);
        free(data);
    }
    free(q->buff);
    free(q);
}


int main() {
    int numbers[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int crt_val, test_number;

    queue_t *q = q_create(sizeof(int), 11);
    q_enqueue(q, &numbers[5]);

    scanf("%d", &test_number);
    if (test_number == 0) {
        /* Test enqueue & size */
        printf("%d\n", q_get_size(q));

    } else if (test_number == 1) {
        /* Test front */
        crt_val = *(int *)q_front(q);
        printf("%d\n", crt_val);

    } else if (test_number == 2) {
         /* Test dequeue */
        crt_val = *(int *)q_front(q);
        q_dequeue(q);
        printf("%d %d\n", crt_val, q_get_size(q));

    } else if (test_number == 3) {
        q_dequeue(q);
        q_enqueue(q, &numbers[2]);
        q_enqueue(q, &numbers[3]);
        q_enqueue(q, &numbers[4]);

        /* Test multiple pushes */
        printf("%d %d\n", q_get_size(q), *(int *)q_front(q));

    } else if (test_number == 4) {
        q_dequeue(q);
        q_enqueue(q, &numbers[2]);
        q_enqueue(q, &numbers[3]);
        q_enqueue(q, &numbers[4]);

        /* Test multiple pops */
        while (!q_is_empty(q)) {
            printf("%d ", *(int *)q_front(q));
            q_dequeue(q);
        }
        printf("%d\n", q_get_size(q));
    }

    free(q);
    return 0;
}
