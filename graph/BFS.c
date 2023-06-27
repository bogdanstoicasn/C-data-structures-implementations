#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX_STRING_SIZE	256

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
	linked_list_t* ll = calloc(1, sizeof(*ll));
	ll->data_size = data_size;

	return ll;
}

static ll_node_t*
get_nth_node(linked_list_t* list, unsigned int n)
{
	unsigned int len = list->size - 1;
	unsigned int i;
	ll_node_t* node = list->head;

	n = MIN(n, len);

	for (i = 0; i < n; ++i)
		node = node->next;

	return node;
}

static ll_node_t*
create_node(const void* new_data, unsigned int data_size)
{
	ll_node_t* node = calloc(1, sizeof(*node));
	node->data = malloc(data_size);
	memcpy(node->data, new_data, data_size);

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
	ll_node_t *new_node, *prev_node;

	if (!list)
		return;

	new_node = create_node(new_data, list->data_size);

	if (!n || !list->size) {
		new_node->next = list->head;
		list->head = new_node;
	} else {
		prev_node = get_nth_node(list, n - 1);
		new_node->next = prev_node->next;
		prev_node->next = new_node;
	}

	++list->size;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Daca n >= nr_noduri - 1, se elimina nodul de
 * la finalul listei. Daca n < 0, eroare. Functia intoarce un pointer spre acest
 * nod proaspat eliminat din lista. Este responsabilitatea apelantului sa
 * elibereze memoria acestui nod.
 */
ll_node_t*
ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
	ll_node_t *prev_node, *removed_node;

	if (!list || !list->size)
		return NULL;

	if (!n) {
		removed_node = list->head;
		list->head = removed_node->next;
		removed_node->next = NULL;
	} else {
		prev_node = get_nth_node(list, n - 1);
		removed_node = prev_node->next;
		prev_node->next = removed_node->next;
		removed_node->next = NULL;
	}

	--list->size;

	return removed_node;
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int
ll_get_size(linked_list_t* list)
{
	return !list ? 0 : list->size;
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
	ll_node_t* node;

	if (!pp_list || !*pp_list)
		return;

	while ((*pp_list)->size) {
		node = ll_remove_nth_node(*pp_list, 0);
		free(node->data);
		free(node);
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
	ll_node_t* node = list->head;

	for (; node; node = node->next)
		printf("%d ", *(int*)node->data);
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
	ll_node_t* node = list->head;

	for (; node; node = node->next)
		printf("%s ", (char*)node->data);
	printf("\n");
}

typedef struct queue_t queue_t;
struct queue_t
{
  /* Dimensiunea maxima a cozii */
  unsigned int max_size;
  /* Numarul de elemente din coada */
  unsigned int size;
  /* Dimensiunea in octeti a tipului de date stocat in coada */
  unsigned int data_size;
  /* Indexul de la care se vor efectua operatiile de front si dequeue */
  unsigned int read_idx;
  /* Indexul de la care se vor efectua operatiile de dequeue */
  unsigned int write_idx;
  /* Bufferul ce stocheaza elementele cozii */
  void** buff;
};

queue_t*
q_create(unsigned int data_size, unsigned int max_size)
{
	queue_t* q = calloc(1, sizeof(*q));

	q->data_size = data_size;
	q->max_size = max_size;

	q->buff = malloc(max_size * sizeof(*q->buff));

	return q;
}

/*
 * Functia intoarce numarul de elemente din coada al carei pointer este trimis
 * ca parametru.
 */
unsigned int
q_get_size(queue_t* q)
{
	return !q ? 0 : q->size;
}

/*
 * Intoarce 1 daca coada este goala si 0 in caz contrar.
 */
unsigned int
q_is_empty(queue_t* q)
{
	return !q ? 1 : !q->size;
}

/* 
 * Functia intoarce primul element din coada, fara sa il elimine.
 */
void*
q_front(queue_t* q)
{
	if (!q || !q->size)
		return NULL;
	return q->buff[q->read_idx];
}

/*
 * Functia scoate un element din coada. Se va intoarce 1 daca operatia s-a
 * efectuat cu succes (exista cel putin un element pentru a fi eliminat) si
 * 0 in caz contrar.
 */
bool
q_dequeue(queue_t* q)
{
	if (!q || !q->size)
		return false;
    
	free(q->buff[q->read_idx]);

	q->read_idx = (q->read_idx + 1) % q->max_size;
	--q->size;
	return true;
}

/* 
 * Functia introduce un nou element in coada. Se va intoarce 1 daca
 * operatia s-a efectuat cu succes (nu s-a atins dimensiunea maxima) 
 * si 0 in caz contrar.
 */
bool
q_enqueue(queue_t* q, void* new_data)
{
	void* data;
	if (!q || q->size == q->max_size)
		return false;

	data = malloc(q->data_size);
	memcpy(data, new_data, q->data_size);

	q->buff[q->write_idx] = data;
	q->write_idx = (q->write_idx + 1) % q->max_size;
	++q->size;

	return true;
}

/*
 * Functia elimina toate elementele din coada primita ca parametru.
 */
void
q_clear(queue_t* q)
{
	unsigned int i;
	if (!q || !q->size)
		return;

	for (i = q->read_idx; i != q->write_idx; i = (i + 1) % q->max_size)
		free(q->buff[i]);

	q->read_idx = 0;
	q->write_idx = 0;
	q->size = 0;
}

/*
 * Functia elibereaza toata memoria ocupata de coada.
 */
void
q_free(queue_t* q)
{
	if (!q)
		return;

	q_clear(q);
	free(q->buff);
	free(q);
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

/* --- START IMPLEMENTING BFS HERE --- */

/* Implementati parcurgerea BFS a unui List Graph
 * Functia trebuie sa printeze un nod, imediat ce acesta este vizitat pentru prima data.
 */

void bfs_list_graph(list_graph_t* lg, int node) {
    // Create a visited array and initialize all nodes as not visited
    int visited[lg->nodes];
    for (int i = 0; i < lg->nodes; i++) {
        visited[i] = 0;
    }
    
    // Create a queue for BFS
    queue_t* queue = q_create(sizeof(linked_list_t), lg->nodes);
    
    // Mark the current node as visited and enqueue it
    visited[node] = 1;
    q_enqueue(queue, &node);
 
    // Loop to traverse the graph using BFS
    while (!q_is_empty(queue)) {
        // Dequeue a vertex from queue and print it
        //int current_node = dequeue(queue);
        int current_node = *(int*)q_front(queue);
        q_dequeue(queue);
        printf("%d ", current_node);
 
        ll_node_t *temp = lg->neighbors[current_node]->head;
        while (temp != NULL) {
            int adj_node = *(int*)temp->data;
            if (!visited[adj_node]) {
                visited[adj_node] = 1;
                q_enqueue(queue, &adj_node);
            }
            temp = temp->next;
        }
    }

    // Free the queue memory
    q_free(queue);
}


int main()
{

	list_graph_t *lg = NULL;

	while (1) {
		char command[MAX_STRING_SIZE];
		int nr1, nr2;
		int start_node;
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

		if (strncmp(command, "bfs", 3) == 0) {
			int color[nr_nodes], parent[nr_nodes];
			
			for (int i = 0; i < nr_nodes; ++i) {
    			color[i] = 0;
    			parent[i] = 1000;
    		}
    		
			if (lg != NULL) {

				scanf("%d", &start_node);

				bfs_list_graph(lg, start_node);
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