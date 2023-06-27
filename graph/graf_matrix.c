#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STRING_SIZE    256
typedef struct
{
    int** matrix; /* Matricea de adiacenta a grafului */
    int nodes;    /* Numarul de noduri din graf. */
} matrix_graph_t;

/**
 * Initializeaza graful cu numarul de noduri primit ca parametru si aloca
 * memorie pentru matricea de adiacenta a grafului.
 */

/* Graful este ORIENTAT */

/* Nodurile sunt indexate de la 0.*/

int **alloc_matrix(int line, int coloumn) {
	int **a = calloc(line, sizeof(int *)); 
    if (!a) {
		fprintf(stderr, "malloc() failed\n");
		return NULL;
	}
	for (int i = 0; i < line; i++) {
		a[i] = calloc(coloumn, sizeof(int)); 
        if (!a[i]) {
			fprintf(stderr, "malloc() failed\n");
			while (--i >= 0) {
				free(a[i]);
			}
			free(a);
			return NULL;
		}
	}
	return a;
}
matrix_graph_t*
mg_create(int nodes)
{
    matrix_graph_t *grf = malloc(sizeof(matrix_graph_t));
    if (!grf)
        return;
    
    grf->matrix = alloc_matrix(nodes, nodes);
    grf->nodes = nodes;
    return grf;
}

/* Adauga o muchie intre nodurile sursa si destinatie */
void
mg_add_edge(matrix_graph_t* graph, int src, int dest)
{
    graph->matrix[src][dest] = 1;
}

/* Returneaza 1 daca exista muchie intre cele doua noduri, 0 in caz contrar */
int
mg_has_edge(matrix_graph_t* graph, int src, int dest)
{
    if (graph->matrix[src][dest] != 0)
        return 1;

    return 0;
}

/* Elimina muchia dintre nodurile sursa si destinatie */
void
mg_remove_edge(matrix_graph_t* graph, int src, int dest)
{
    graph->matrix[src][dest] = 0;
}

/* Elibereaza memoria folosita de matricea de adiacenta a grafului */
void
mg_free(matrix_graph_t* graph)
{
    for (int i = 0; i < graph->nodes; ++i) {
        free(graph->matrix[i]);
    }
    free(graph->matrix);
    free(graph);
}

void
print_matrix_graph(matrix_graph_t* mg)
{
    for (int i = 0; i < mg->nodes; ++i) {
        for (int j = 0; j < mg->nodes; ++j)
            printf("%d ",mg->matrix[i][j]);
        printf("\n");
    }
}

int main()
{
    
    matrix_graph_t *mg = NULL;
    
    while (1) {
        char command[MAX_STRING_SIZE];
        int nr1, nr2;
        int nr_nodes;
        
        scanf("%s", command);
        
        if (strncmp(command, "create_mg", 9) == 0) {
            scanf("%d", &nr_nodes);
            mg = mg_create(nr_nodes);
        }
        
        if (strncmp(command, "add_edge", 8) == 0) {
            if (mg != NULL) {
                scanf("%d %d", &nr1, &nr2);
                mg_add_edge(mg, nr1, nr2);
            } else {
                printf("Create a graph first!\n");
                exit(0);
            }
        }
        
        if (strncmp(command, "remove_edge", 11) == 0) {
            if (mg != NULL) {
                scanf("%d %d", &nr1, &nr2);
                mg_remove_edge(mg, nr1, nr2);
            } else {
                printf("Create a graph first!\n");
                exit(0);
            }
        }
        
        if (strncmp(command, "print_graph", 11) == 0) {
            if (mg != NULL) {
                print_matrix_graph(mg);
            } else {
                printf("Create a graph first!\n");
                exit(0);
            }
        }
        
        if (strncmp(command, "has_edge", 8) == 0) {
            if (mg != NULL) {
                int flag;
                scanf("%d %d", &nr1, &nr2);
                flag = mg_has_edge(mg, nr1, nr2);
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
            if (mg != NULL) {
                mg_free(mg);
            } else {
                printf("Create a graph first!\n");
                exit(0);
            }
            break;
        }
    }
    
    return 0;
}
