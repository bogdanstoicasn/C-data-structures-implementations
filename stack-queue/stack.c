#include <stdio.h>
#include <stdlib.h>
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

typedef struct ll_node_t ll_node_t;
struct ll_node_t
{
    void* data;
    ll_node_t* next;
};

typedef struct linked_list_t linked_list_t;
struct linked_list_t
{
    ll_node_t* head;
    unsigned int data_size;
    unsigned int size;
};

linked_list_t*
ll_create(unsigned int data_size)
{
    linked_list_t* ll;

    ll = malloc(sizeof(linked_list_t));
    DIE(ll == NULL, "linked_list malloc");

    ll->head = NULL;
    ll->data_size = data_size;
    ll->size = 0;

    return ll;
}


void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    ll_node_t *prev, *curr;
    ll_node_t* new_node;

    if (list == NULL) {
        return;
    }

    if (n > list->size) {
        n = list->size;
    } else if (n < 0) {
        return;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(ll_node_t));
    DIE(new_node == NULL, "new_node malloc");
    new_node->data = malloc(list->data_size);
    DIE(new_node->data == NULL, "new_node->data malloc");
    memcpy(new_node->data, new_data, list->data_size);

    new_node->next = curr;
    if (prev == NULL) {
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    list->size++;
}

ll_node_t*
ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    ll_node_t *prev, *curr;

    if (list == NULL) {
        return NULL;
    }

    if (list->head == NULL) {
        return NULL;
    }

    if (n > list->size - 1) {
        n = list->size - 1;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL) {
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }

    list->size--;

    return curr;
}


unsigned int
ll_get_size(linked_list_t* list)
{
    if (list == NULL) {
        return -1;
    }

    return list->size;
}


void
ll_free(linked_list_t** pp_list)
{
    ll_node_t* currNode;

    if (pp_list == NULL || *pp_list == NULL) {
        return;
    }

    while (ll_get_size(*pp_list) > 0) {
        currNode = ll_remove_nth_node(*pp_list, 0);
        free(currNode->data);
        free(currNode);
    }

    free(*pp_list);
    *pp_list = NULL;
}

void
ll_print_int(linked_list_t* list)
{
    ll_node_t* curr;

    if (list == NULL) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%d ", *((int*)curr->data));
        curr = curr->next;
    }

    printf("\n");
}

void
ll_print_string(linked_list_t* list)
{
    ll_node_t* curr;

    if (list == NULL) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%s ", (char*)curr->data);
        curr = curr->next;
    }

    printf("\n");
}


/* ---------------------------------- TODO: Stack implementation ----------------------------------------*/

struct Stack {
    linked_list_t *list;
};

void init_stack(struct Stack *stack, unsigned int data_size) {
    stack->list = ll_create(data_size);
}

int get_size_stack(struct Stack *stack) {
    return stack->list->size;
}

int is_empty_stack(struct Stack *stack) {
    
    return get_size_stack(stack) == 0;
}

void* peek_stack(struct Stack *stack) {
    if (stack == NULL || stack->list == NULL) {
        return NULL;
    }

    ll_node_t *node = stack->list->head;
    while (node->next != NULL) {
        node = node->next;
    }
    return node->data;
}

void pop_stack(struct Stack *stack) {
    struct ll_node_t *node;

    if (stack == NULL || stack->list == NULL) {
        return;
    }

    node = ll_remove_nth_node(stack->list, stack->list->size - 1);
    free(node);
}

void push_stack(struct Stack *stack, void *new_data) {
    ll_add_nth_node(stack->list, stack->list->size, new_data);
}

void clear_stack(struct Stack *stack) {
    struct ll_node_t *node;
    while (!is_empty_stack(stack)) { //mod
        node = ll_remove_nth_node(stack->list, 0);
        free(node);
    }
}

void purge_stack(struct Stack *stack) {
    clear_stack(stack);
    free(stack->list);
}

int main() {
    struct Stack *st = malloc(sizeof(struct Stack));
    int numbers[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int crt_val, test_number;

    scanf("%d", &test_number);
    init_stack(st, sizeof(int));
    push_stack(st, &numbers[10]);

    if (test_number == 0) {
        /* Test push & size */
        printf("%d\n", get_size_stack(st));

    } else if (test_number == 1) {
        /* Test peek */
        crt_val = *(int *)peek_stack(st);
        printf("%d\n", crt_val);

    } else if (test_number == 2) {
        /* Test pop */
        crt_val = *(int *)peek_stack(st);
        pop_stack(st);
        printf("%d %d\n", crt_val, get_size_stack(st));

    } else if (test_number == 3) {
        pop_stack(st);
        push_stack(st, &numbers[9]);
        push_stack(st, &numbers[8]);
        push_stack(st, &numbers[7]);

        /* Test multiple pushes */
        printf("%d %d\n", get_size_stack(st), *(int *)peek_stack(st));

    } else if (test_number == 4) {
        pop_stack(st);
        push_stack(st, &numbers[9]);
        push_stack(st, &numbers[8]);
        push_stack(st, &numbers[7]);

        /* Test multiple pops */
        while (!is_empty_stack(st)) {
            printf("%d ", *(int *)peek_stack(st));
            pop_stack(st);
        }
        printf("%d\n", get_size_stack(st));
    }

    purge_stack(st);
    free(st);
    return 0;
}
