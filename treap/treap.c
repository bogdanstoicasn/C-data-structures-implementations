#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <stdio.h>

#define MAX_NODES 1000
#define MAX_PRIORITY 1000

typedef struct Node {
	void *data;
	int priority;

	struct Node *left, *right;
} Node;
typedef struct {
	Node *root;
	int(*compar)(void*, void*);
} Treap_tree;

int compar(void* value1, void* value2) {

    int *a = (int*)value1;
    int *b = (int*)value2;
    if ((*a) > (*b)) {
        return 1;
    } else if ((*a) < (*b)) {
        return -1;
    }
    return 0;
}

// Creeaza structura Treap-ului
Treap_tree* treap_create(int (*cmp)(void*, void*)) {
	srand(time(NULL));
	Treap_tree *tree = malloc(sizeof(Treap_tree));
	tree->compar = cmp;
	if (tree == NULL) {
		return NULL;
	}
	tree->root = NULL;
	return tree;
}
void node_free(Node **node) {

	if (!(*node)) {
		return;
	}
	node_free(&(*node)->left);
	node_free(&(*node)->right);

	free((*node)->data);
	free(*node);
}

void treap_free(Treap_tree *tree) {
	if (tree->root != NULL) {
		node_free(&tree->root);
	}
	free(tree);
}
/* Creeaza un nod
 * @param1: Valoarea ce trebuie pusa in nod.
 * @param2: Numarul de octeti pe care scrie valoarea.
 */
Node* node_create(void *value, int data_size) {
	Node *node = malloc(sizeof(Node));
	if (node == NULL) {
		return NULL;
	}
	node->data = malloc(data_size);

	// Copiere octet cu octet din value, in nodul curent.
    for (int i = 0; i < data_size; ++i) {
        *(char *)(node->data + i) = *(char *)(value + i);
    }

    // Nodurile frunze au inaltimea 0.
	node->priority = rand() % MAX_PRIORITY;
	node->left = NULL;
	node->right = NULL;

	return node;
}

int max(int a, int b) {
	if (a > b) {
		return a;
	}
	return b;
}
// Nodurile NULL au prioritatea -1 pentru a pastra proprietatea de max-heap.
int priority(Node *node) {
	if (node == NULL) {
		return -1;
	}
	return node->priority;
}
/* Rotire dreapta
 * Pentru a nu fi nevoie sa mentinem pointer catre nodul parinte,
 * se vor folosi pointeri la noduri
 *
 *      node  			    lson
 *     /    \			   /    \
 *   lson    y    --->    x    node
 *   /  \     		           /   \
 *  x   lrson                lrson  y
 */
void rotate_right(Node **node) {
    /* Rotates right the treap tree */
    Node *aux = (*node)->left;

    (*node)->left = aux->right;
    
    aux->right = (*node);
    
    (*node) = aux;
}
/* Rotire stanga
 * Pentru a nu fi nevoie sa mentinem pointer catre nodul parinte,
 * se vor folosi pointeri la noduri
 *
 *     node  			    rson
 *    /    \			   /    \
 *   x     rson    --->  node    y
 *         /   \		 /   \
 *       rlson  y       x   rlson
 */
void rotate_left(Node **node) {
    /* Rotates left the treap tree */
    Node *aux = (*node)->right;
    
    (*node)->right = aux->left;
    
    aux->left = (*node);
    
    (*node) = aux;
}
/* Inserare in Treap
 *
 * @param1: Nodul radacina al subarborelui din parcurgerea recursiva.
 * @param2: Valoare de adaugat in Treap.
 * @param3: Numarul de octeti pe care se scrie valoarea.
 * @param4: Functia de comparare pentru datele din Treap.
 */
void treap_insert(Node **node, void *value, int data_size, int (*compar)(void*, void*)) {
    if (*node == NULL) {
        *node = node_create(value, data_size);
        return;
    }

    if (compar(value, (*node)->data) < 0) {
        treap_insert(&(*node)->left, value, data_size, compar);
        if (priority((*node)->left) > priority(*node))
            rotate_right(node);
    } else {
        treap_insert(&(*node)->right, value, data_size, compar);
        if (priority((*node)->right) > priority(*node))
            rotate_left(node);
    }
}

/* Stergere din Treap
 *
 * @param1: Nodul radacina al subarborelui din parcurgerea recursiva.
 * @param2: Valoare de adaugat in Treap.
 * @param3: Numarul de octeti pe care se scrie valoarea.
 * @param4: Functia de comparare pentru datele din Treap.
 */
void treap_delete(Node **node, void *value, int data_size, int (*compar)(void*, void*)) {
    if (*node == NULL) {
        return;
    }

    if (compar(value, (*node)->data) < 0) {
        treap_delete(&(*node)->left, value, data_size, compar);
    } else if (compar(value, (*node)->data) > 0) {
        treap_delete(&(*node)->right, value, data_size, compar);
    } else {
        if ((*node)->left == NULL && (*node)->right == NULL) {
            free((*node)->data);
            free(*node);
            *node = NULL;
        } else if ((*node)->left == NULL) {
            Node *aux = *node;
            *node = (*node)->right;
            free(aux->data);
            free(aux);
        } else if ((*node)->right == NULL) {
            Node *aux = *node;
            *node = (*node)->left;
            free(aux->data);
            free(aux);
        } else {
            if (priority((*node)->left) > priority((*node)->right)) {
                rotate_right(node);
                treap_delete(node, value, data_size, compar);
            } else {
                rotate_left(node);
                treap_delete(node, value, data_size, compar);
            }
        }
    }
}

void* get_key(Node *node, void *value, int data_size, int (*compar)(void*, void*)) {
    if (node == NULL) {
        return NULL;
    }
    if (compar(value, node->data) < 0)
        return get_key(node->left, value, data_size, compar);
    if (compar(value, node->data) > 0)
        return get_key(node->right, value, data_size, compar);

    return node->data;
}

/* Verifica daca un arbore respecta proprietatile unui treap
 *
 * @param1: Nodul curent in parcurgerea recursiva.
 * @param2: Functia de comparare a datelor din fiecare nod.
 * @return: Daca arborele e Treap, vom returna numarul de noduri al arborelui,
 * 			altfel, vom returna o valoare negativa.
 */
int check_treap(Node *node, int (*compar)(void*, void*)) {
	if (node == NULL) {
		return 0;
	} else if (node->left == NULL && node->right == NULL) {
		return 1;
	} else if (node->left == NULL) {
		if (priority(node) >= priority(node->right)
			&& compar(node->data, node->right->data) <= 0) {
			return 1 + check_treap(node->right, compar);
		}
		return INT_MIN;
	} else if (node->right == NULL) {
		if (priority(node) >= priority(node->left)
			&& compar(node->data, node->left->data) >= 0) {
			return 1 + check_treap(node->left, compar);
		}
		printf("%d %d %d\n", priority(node), priority(node->left), priority(node->right));
		return -1;
	} else {
		if (priority(node) >= priority(node->left) && priority(node) >= priority(node->right)
			&& compar(node->data, node->left->data) >= 0
			&& compar(node->data, node->right->data) <= 0) {
			return 1 + check_treap(node->left, compar) + check_treap(node->right, compar);
		}
		printf("%d %d %d\n", priority(node), priority(node->left), priority(node->right));
		return -1;
	}
}

/* Obtinerea cheilor sortate crescator.
 *
 * @param1: Nodul curent in parcurgerea recursiva.
 * @param2: Array-ul prin care se intorc cheile sortate crescator.
 * @param3: Numarul de chei adaugate in array.
 */
void ascending_nodes(Node *node, int* keys, int *num_keys) {
    if (node == NULL) {
        return;
    }
    // inorder traversal
    ascending_nodes(node->left, keys, num_keys);
    
    keys[*num_keys] = *(int*)node->data;
    
    (*num_keys)++; // increment num_keys
    
    ascending_nodes(node->right, keys, num_keys);
}
int main() {
	Treap_tree *tree = treap_create((int (*)(void*, void*))(compar));

	int task, no_inserts, no_deletes, no_finds;
    int value;

    scanf("%d", &task);
    scanf("%d", &no_inserts);

    if (task == 1) {
        printf("%s\n", "------------- TASK 1 -------------");
    } else if (task == 2) {
        printf("%s\n", "------------- TASK 2 -------------");
    } else if (task == 3) {
        printf("%s\n", "------------- TASK 3 -------------");
    } else {
        printf("%s\n", "------------- TASK 4 -------------");
    }

    // ------------- TASK 1 -------------
    for (int i = 1; i <= no_inserts; ++i) {
	   	scanf("%d", &value);
   	   	treap_insert(&tree->root, &value, sizeof(int), tree->compar);
   	   	// Used for checker.
        if (task == 1) {
            if (check_treap(tree->root, tree->compar) == i) {
                printf("Correct insertion. Treap has %d nodes.\n",
                        check_treap(tree->root, tree->compar));
            } else {
                printf("%s\n", "Wrong insertion.");
            }
        }
    }

    // ------------- TASK 2 ------------
    if (task >= 2) {
        scanf("%d", &no_finds);
        for (int i = 1; i <= no_finds; ++i) {
            scanf("%d", &value);
            if (task == 2) {
                if (get_key(tree->root, &value, sizeof(int), tree->compar)) {
                    printf("%d %s\n", value, "is in Treap.");
                } else {
                    printf("%d %s\n", value, "is NOT in Treap.");
                }
            }
        }
    }

    // ------------- TASK 3 -------------
    if (task >= 3) {
        scanf("%d", &no_deletes);
        for (int i = 1; i <= no_deletes; ++i) {
            scanf("%d", &value);
            if (task == 3) {
                treap_delete(&tree->root, &value, sizeof(int), tree->compar);
                // Used for checker.
        	   	if (1) {
        	   		printf("Correct deletion. Treap has %d nodes.\n",
                            check_treap(tree->root, tree->compar));
        	   	} else {
        	   		printf("%s\n", "Wrong deletion.");
        	   	}
            }
        }
    }


    // ------------- TASK 4 -------------
    if (task == 4) {
        int keys[MAX_NODES] = {0};
        int num_keys = 0;
        ascending_nodes(tree->root, keys, &num_keys);
        printf("Ascending keys: ");
        for (int i = 0; i < num_keys; ++i) {
            printf("%d ", keys[i]);
        }
    }
    treap_free(tree);
	return 0;
}
