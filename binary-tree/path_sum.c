#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define MAX_NODES 500
#define BUF_SIZ 512

#define DIE(assertion, call_description)  \
    do                                    \
    {                                     \
        if (assertion)                    \
        {                                 \
            fprintf(stderr, "(%s, %d): ", \
                    __FILE__, __LINE__);  \
            perror(call_description);     \
            exit(errno);                  \
        }                                 \
    } while (0)

typedef struct b_node_t b_node_t;
struct b_node_t
{
    /* left child */
    b_node_t *left;
    /* right child */
    b_node_t *right;

    /* data contained by the node */
    void *data;
};

typedef struct b_tree_t b_tree_t;
struct b_tree_t
{
    /* root of the tree */
    b_node_t *root;

    /* size of the data contained by the nodes */
    size_t data_size;
};

/* Helper queue data structure definitions */
typedef struct queue_t queue_t;
queue_t *q_create(unsigned int data_size, unsigned int max_size);
unsigned int q_get_size(queue_t *q);
unsigned int q_is_empty(queue_t *q);
void *q_front(queue_t *q);
int q_dequeue(queue_t *q);
int q_enqueue(queue_t *q, void *new_data);
void q_clear(queue_t *q);
void q_free(queue_t *q);

/**
 * Helper function to create a node
 * @data: the data to be added in the node
 * @data_size: data's size
 */
static b_node_t *__b_node_create(void *data, size_t data_size) {
    b_node_t *b_node;

    b_node = malloc(sizeof(*b_node));
    DIE(b_node == NULL, "b_node malloc");

    b_node->left = b_node->right = NULL;

    b_node->data = malloc(data_size);
    DIE(b_node->data == NULL, "b_node->data malloc");
    memcpy(b_node->data, data, data_size);

    return b_node;
}


b_tree_t *b_tree_create(size_t data_size) {
    b_tree_t *christmas_tree = malloc(sizeof(b_tree_t));
    
    christmas_tree->data_size = data_size;
    
    christmas_tree->root = NULL;
    return christmas_tree;
}

/**
 * Insert data based on a width traversal, using the first available child
 * (left to right).
 * The current skel uses an iterative solution based on a queue, if you want to
 * use a recursive approach, feel free to create a helper function.
 */
void b_tree_insert(b_tree_t *b_tree, void *data) {
    queue_t *q;
    b_node_t *b_node, *b_node_tmp;
    b_node_t **b_node_tmp_addr;
    b_node = __b_node_create(data, b_tree->data_size);
    if (!b_tree->root) {
        b_tree->root = b_node;
        return;
    }

    q = q_create(sizeof(b_node_t*), MAX_NODES);
    q_enqueue(q, &b_tree->root);  
    while (!q_is_empty(q)) {
        b_node_tmp_addr = q_front(q);
        b_node_tmp = *b_node_tmp_addr; 
        q_dequeue(q);
        if (b_node_tmp->left == NULL) {
            b_node_tmp->left = b_node;
            break;
        } else q_enqueue(q, &b_node_tmp->left);
        if (b_node_tmp->right == NULL) {
            b_node_tmp->right = b_node;
            break;
        } else q_enqueue(q, &b_node_tmp->right);
        //printf("%d\n", *(int*)(b_node_tmp->left->data));
    }

    q_free(q);
}


/**
 * Free the left and the right subtree of a node, its data and itself.
 * @b_node: the node which has to free its children and itself
 * @free_data: function used to free the data contained by a node
 */
static void __b_tree_free(b_node_t *b_node, void (*free_data)(void *)) {
    if (!b_node)
        return;
    
    // Free left and right subtrees recursively //
    __b_tree_free(b_node->left, free_data);
    __b_tree_free(b_node->right, free_data);
    
    if (free_data)
        free_data(b_node->data);
    
    free(b_node);
}


void b_tree_free(b_tree_t *b_tree, void (*free_data)(void *)) {
    __b_tree_free(b_tree->root, free_data);
    free(b_tree);
}

void read_tree(b_tree_t *b_tree) {
    int i, N, data;
    char *token;
    char buf[BUF_SIZ];
    const char delim[2] = " ";

    fgets(buf, BUF_SIZ, stdin);
    sscanf(buf, "%d\n", &N);

    fgets(buf, BUF_SIZ, stdin);
    for (i = 0; i < N; ++i)
    {
        if (i == 0) {
            token = strtok(buf, delim);
        } else {
            token = strtok(NULL, delim);
        }

        data = atoi(token);
        b_tree_insert(b_tree, &data);
    }
}
int __is_leaf(b_node_t *b_node) {
    return b_node != NULL && b_node->left == NULL && b_node->right == NULL;
}

int has_path_sum(b_node_t *root, int target_sum) {
    if (!root) {
        return 0;
    }

    if (__is_leaf(root) && *(int*)root->data == target_sum) {
        return 1;
    }

    return has_path_sum(root->left, target_sum - *(int *)root->data) ||
           has_path_sum(root->right, target_sum - *(int *)root->data);
}




int main(void) {
    b_tree_t *binary_tree;
    int target_sum = 0;

    binary_tree = b_tree_create(sizeof(int));
    read_tree(binary_tree);

    scanf("%d", &target_sum);
    printf("%d\n", has_path_sum(binary_tree->root, target_sum));

    b_tree_free(binary_tree, free);

    return 0;
}

struct queue_t {
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

queue_t *q_create(unsigned int data_size, unsigned int max_size) {
    queue_t *q = calloc(1, sizeof(*q));
    DIE(!q, "calloc queue failed");

    q->data_size = data_size;
    q->max_size = max_size;

    q->buff = malloc(max_size * sizeof(*q->buff));
    DIE(!q->buff, "malloc buffer failed");

    return q;
}

unsigned int q_get_size(queue_t *q) {
    return !q ? 0 : q->size;
}

unsigned int q_is_empty(queue_t *q) {
    return !q ? 1 : !q->size;
}

void *q_front(queue_t *q) {
    if (!q || !q->size)
        return NULL;

    return q->buff[q->read_idx];
}

int q_dequeue(queue_t *q) {
    if (!q || !q->size)
        return 0;

    free(q->buff[q->read_idx]);

    q->read_idx = (q->read_idx + 1) % q->max_size;
    --q->size;
    return 1;
}

int q_enqueue(queue_t *q, void *new_data) {
    void *data;
    if (!q || q->size == q->max_size)
        return 0;

    data = malloc(q->data_size);
    DIE(!data, "malloc data failed");
    memcpy(data, new_data, q->data_size);

    q->buff[q->write_idx] = data;
    q->write_idx = (q->write_idx + 1) % q->max_size;
    ++q->size;

    return 1;
}

void q_clear(queue_t *q) {
    unsigned int i;
    if (!q || !q->size)
        return;

    for (i = q->read_idx; i != q->write_idx; i = (i + 1) % q->max_size)
        free(q->buff[i]);

    q->read_idx = 0;
    q->write_idx = 0;
    q->size = 0;
}

void q_free(queue_t *q) {
    if (!q)
        return;

    q_clear(q);
    free(q->buff);
    free(q);
}