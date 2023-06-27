#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX_STRING_SIZE	256

/* --- LINKED LIST SUPPORT START --- */

typedef struct ll_node_t
{
    void* data;
    struct ll_node_t* next;
} ll_node_t;

typedef struct linked_list_t
{
    ll_node_t* head;
    unsigned int data_size;
    unsigned int size;
} linked_list_t;

linked_list_t *
ll_create(unsigned int data_size)
{
    linked_list_t* ll;
    
    ll = malloc(sizeof(*ll));
    
    ll->head = NULL;
    ll->data_size = data_size;
    ll->size = 0;
    
    return ll;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Daca n >= nr_noduri - 1, se returneaza
 * ultimul nod.
 */
ll_node_t* get_nth_node(linked_list_t* list, unsigned int n)
{
    if (!list) {
        return NULL;
    }
    
    unsigned int len = list->size - 1;
    unsigned int i;
    ll_node_t* node = list->head;
    
    n = MIN(n, len);
    
    for (i = 0; i < n; ++i) {
        node = node->next;
    }
    
    return node;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Daca n >= nr_noduri, noul nod se adauga la finalul listei. Daca
 * n < 0, eroare.
 */
void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    ll_node_t *prev, *curr;
    ll_node_t* new_node;
    
    if (!list) {
        return;
    }
    
    /* n >= list->size inseamna adaugarea unui nou nod la finalul listei. */
    if (n > list->size) {
        n = list->size;
    }
    
    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }
    
    new_node = malloc(sizeof(*new_node));
    new_node->data = malloc(list->data_size);
    memcpy(new_node->data, new_data, list->data_size);
    
    new_node->next = curr;
    if (prev == NULL) {
        /* Adica n == 0. */
        list->head = new_node;
    } else {
        prev->next = new_node;
    }
    
    list->size++;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Daca n >= nr_noduri - 1, se elimina nodul de
 * la finalul listei. Daca n < 0, eroare. Functia intoarce un pointer spre acest
 * nod proaspat eliminat din lista. Este responsabilitatea apelantului sa
 * elibereze memoria acestui nod.
 */
ll_node_t *
ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    ll_node_t *prev, *curr;
    
    if (!list || !list->head) {
        return NULL;
    }
    
    /* n >= list->size - 1 inseamna eliminarea nodului de la finalul listei. */
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
        /* Adica n == 0. */
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }
    
    list->size--;
    
    return curr;
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int
ll_get_size(linked_list_t* list)
{
    if (!list) {
        return -1;
    }
    
    return list->size;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista si actualizeaza la
 * NULL valoarea pointerului la care pointeaza argumentul (argumentul este un
 * pointer la un pointer).
 */
void
ll_free(linked_list_t** pp_list)
{
    ll_node_t* currNode;
    
    if (!pp_list || !*pp_list) {
        return;
    }
    
    while (ll_get_size(*pp_list) > 0) {
        currNode = ll_remove_nth_node(*pp_list, 0);
        free(currNode->data);
        currNode->data = NULL;
        free(currNode);
        currNode = NULL;
    }
    
    free(*pp_list);
    *pp_list = NULL;
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza int-uri. Functia afiseaza toate valorile int stocate in nodurile
 * din lista inlantuita separate printr-un spatiu.
 */
void
ll_print_int(linked_list_t* list)
{
    ll_node_t* curr;
    
    if (!list) {
        return;
    }
    
    curr = list->head;
    while (curr != NULL) {
        printf("%d ", *((int*)curr->data));
        curr = curr->next;
    }
    
    printf("\n");
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza string-uri. Functia afiseaza toate string-urile stocate in
 * nodurile din lista inlantuita, separate printr-un spatiu.
 */
void
ll_print_string(linked_list_t* list)
{
    ll_node_t* curr;
    
    if (!list) {
        return;
    }
    
    curr = list->head;
    while (curr != NULL) {
        printf("%s ", (char*)curr->data);
        curr = curr->next;
    }
    
    printf("\n");
}

/* Graful este ORIENTAT */

typedef struct
{
	linked_list_t** neighbors; /* Listele de adiacenta ale grafului */
	int nodes;                 /* Numarul de noduri din graf. */
} list_graph_t;

/**
 * Initializeaza graful cu numarul de noduri primit ca parametru si aloca
 * memorie pentru lista de adiacenta a grafului.
 */
list_graph_t*
lg_create(int nodes)
{
	list_graph_t *graph = malloc(sizeof(list_graph_t));

    graph->nodes = nodes;

    graph->neighbors = malloc(nodes * sizeof(linked_list_t*));

    for (int i = 0; i < graph->nodes; ++i) {
        graph->neighbors[i] = ll_create(sizeof(ll_node_t));
    }

    return graph;
}

/* Adauga o muchie intre nodurile primite ca parametri */
void
lg_add_edge(list_graph_t* graph, int src, int dest)
{
    ll_add_nth_node(graph->neighbors[src], ((linked_list_t*)graph->neighbors[src])->size, &dest);
}

/* Returneaza 1 daca exista muchie intre cele doua noduri, 0 in caz contrar */
int
lg_has_edge(list_graph_t* graph, int src, int dest)
{
	linked_list_t *lisss = graph->neighbors[src];
    ll_node_t *node = lisss->head;
    while (node) {
        int nr = *(int*) node->data;
        if (nr == dest)
            return 1;
        node = node->next;
    }
    return 0;
}

/* Elimina muchia dintre nodurile primite ca parametri */
void
lg_remove_edge(list_graph_t* graph, int src, int dest)
{
	ll_remove_nth_node(graph->neighbors[src], dest);
}

/* Elibereaza memoria folosita de lista de adiacenta a grafului */
void
lg_free(list_graph_t* graph)
{
	for (int i = 0; i < graph->nodes; ++i) {
        ll_free(&graph->neighbors[i]);
    }
    free(graph->neighbors);
    free(graph);
}

/* Printeaza lista de adiacenta a grafului
 */
void
lg_print_graph(list_graph_t* graph)
{
	for (int i = 0; i < graph->nodes; ++i) {
        printf("%d: ", i);
        ll_print_int(graph->neighbors[i]);
    }
}

int main()
{

	list_graph_t *lg = NULL;

	while (1) {
		char command[MAX_STRING_SIZE];
		int nr1, nr2;
		int nr_nodes;

		scanf("%s", command);

		if (strncmp(command, "create_lg", 9) == 0) {
			scanf("%d", &nr_nodes);
			lg = lg_create(nr_nodes);
		}

		if (strncmp(command, "add_edge", 8) == 0) {
			if (lg != NULL) {
				scanf("%d %d", &nr1, &nr2);
				lg_add_edge(lg, nr1, nr2);
			} else {
				printf("Create a graph first!\n");
				exit(0);
			}
		}

		if (strncmp(command, "remove_edge", 11) == 0) {
			if (lg != NULL) {
				scanf("%d %d", &nr1, &nr2);
				lg_remove_edge(lg, nr1, nr2);
			} else {
				printf("Create a graph first!\n");
				exit(0);
			}
		}

		if (strncmp(command, "print_graph", 11) == 0) {
			if (lg != NULL) {
				lg_print_graph(lg);
			} else {
				printf("Create a graph first!\n");
				exit(0);
			}
		}

		if (strncmp(command, "has_edge", 5) == 0) {
			if (lg != NULL) {
				int flag;
				scanf("%d %d", &nr1, &nr2);
				flag = lg_has_edge(lg, nr1, nr2);
				if (flag == 1) {
					printf("Has edge\n");
				}
				else if (flag == 0) {
					printf("No edge\n");
				}
			} else {
				printf("Create a graph first!\n");
				exit(0);
			}
		}

		if (strncmp(command, "free", 4) == 0) {
			if (lg != NULL) {
				lg_free(lg);
			} else {
				printf("Create a graph first!\n");
				exit(0);
			}
			break;
		}
	}

	return 0;
}