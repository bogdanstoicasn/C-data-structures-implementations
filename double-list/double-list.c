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

typedef struct dll_node_t dll_node_t;
struct dll_node_t
{
    void* data; /* Pentru ca datele stocate sa poata avea orice tip, folosim un
                   pointer la void. */
    dll_node_t *prev, *next;
};

typedef struct doubly_linked_list_t doubly_linked_list_t;
struct doubly_linked_list_t
{
    dll_node_t* head;
    unsigned int data_size;
    unsigned int size;
};  

/*
 * Functie care trebuie apelata pentru alocarea si initializarea unei liste.
 * (Setare valori initiale pentru campurile specifice structurii LinkedList).
 */
doubly_linked_list_t*
dll_create(unsigned int data_size)
{
	doubly_linked_list_t *new_node = malloc(sizeof(doubly_linked_list_t));

    new_node->head = NULL;

    new_node->data_size = data_size;

    new_node->size = 0;

	return new_node;
	
}

/*
 * Functia intoarce un pointer la nodul de pe pozitia n din lista.
 * Pozitiile din lista sunt indexate incepand cu 0 (i.e. primul nod din lista se
 * afla pe pozitia n=0). Daca n >= nr_noduri, atunci se intoarce nodul de pe
 * pozitia rezultata daca am "cicla" (posibil de mai multe ori) pe lista si am
 * trece de la ultimul nod, inapoi la primul si am continua de acolo. Cum putem
 * afla pozitia dorita fara sa simulam intreaga parcurgere?
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
dll_node_t*
dll_get_nth_node(doubly_linked_list_t* list, unsigned int n)
{
    // daca nu exista lista returneaza NULL (list->head == NULL)(vezi initializare)
    if (n == 0)
        return list->head;
    
    int poz = n % list->size;

    dll_node_t *current = list->head;
    for (int i = 0; i < poz; ++i)
        current = current->next;

    return current;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Cand indexam pozitiile nu "ciclam" pe lista circulara ca la
 * get, ci consideram nodurile in ordinea de la head la ultimul (adica acel nod
 * care pointeaza la head ca nod urmator in lista). Daca n >= nr_noduri, atunci
 * adaugam nodul nou la finalul listei.
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
void
dll_add_nth_node(doubly_linked_list_t* list, unsigned int n, const void* new_data)
{
	dll_node_t *new_node = malloc(sizeof(dll_node_t));

    new_node->data = malloc(list->data_size);
	
    memcpy(new_node->data, new_data, list->data_size);

	new_node->prev = NULL;
	new_node->next = NULL;

	dll_node_t* current = list->head;
  	
	if (n > list->size) 
		n = list->size;


	if (n == 0) {
    	if (list->size != 0) {
      		new_node->next = list->head;
      		list->head = new_node;
      		new_node->prev = current->prev;
      		current->prev = new_node;
      		
			for (int i = 0; i < list->size - 1; ++i) 
				current = current->next;
      		
			current->next = new_node;
    	} else {
      		list->head = new_node;
      		new_node->prev = new_node;
      		new_node->next = new_node;
    	}
  	} else {
    	for (int i = 0; i < n - 1; i++) 
			current = current->next;
    	
		dll_node_t *temp = current->next;
    	
		new_node->next = temp;
    	current->next = new_node;
    	
		new_node->prev = current;
    	temp->prev = new_node;
    
  }
	list->size++;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Functia intoarce un pointer spre acest nod
 * proaspat eliminat din lista. Daca n >= nr_noduri - 1, se elimina nodul de la
 * finalul listei. Este responsabilitatea apelantului sa elibereze memoria
 * acestui nod.
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
dll_node_t*
dll_remove_nth_node(doubly_linked_list_t* list, unsigned int n)
{

	if (list->head ==NULL)
		return NULL;
	
	if (n >= list->size)
		n = list->size - 1;

	list->size--;

	if (n == 0) {
		if(list->size == 0) {
			dll_node_t *old_node = list->head;
			list->head = NULL;
			
			return old_node;
		}

		dll_node_t *last = list->head->prev;
		dll_node_t *old_node = list->head;
		dll_node_t *second_node = old_node->next;
		
		list->head = second_node;
		last->next = second_node;
		second_node->prev = last;

		return old_node; 
	}

	dll_node_t *current = list->head;
	for (int i = 0; i < n - 1; ++i)
		current = current->next;
	
	dll_node_t *third = current->next->next;
	dll_node_t *old_node = current->next;

	current->next = third;
	third->prev = current;
	
	return old_node;

}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int
dll_get_size(doubly_linked_list_t* list)
{
	return list->size;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista.
 */
void 
dll_free(doubly_linked_list_t** pp_list) 
{
  	dll_node_t *current = (*pp_list)->head;
    int n = (*pp_list)->size;
  	
	while (n) {
    	dll_node_t *temp = current;
    
		current = current->next;
    
		free(temp->data);
    	free(temp);
    
		n--;
	}
  	free(*pp_list);
  	*pp_list = NULL;
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza int-uri. Functia afiseaza toate valorile int stocate in nodurile
 * din lista separate printr-un spatiu, incepand de la primul nod din lista.
 */
void
dll_print_int_list(doubly_linked_list_t* list)
{
	if (list->size == 0) 
		return;
    
	int n = list->size;
    
	dll_node_t *current = list->head;

	while (n) {
      printf("%d ", *(int*)current->data);
      
	  current = current->next;
      
	  --n;
    }
  
  printf("\n");
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza string-uri. Functia afiseaza toate string-urile stocate in
 * nodurile din lista separate printr-un spatiu, incepand de la ULTIMUL nod din
 * lista si in ordine inversa.
 */
void
dll_print_string_list(doubly_linked_list_t* list)
{
	if (list->size == 0)
		return;
    
    int n = list->size;
    
	dll_node_t *current = list->head->prev;
	
	while (n) {
      printf("%s ", (char*)current->data);
      
	  current = current->prev;
      
	  --n;
    }
  	printf("\n");
}

int main() {
	doubly_linked_list_t *doublyLinkedList;
	int is_int = 0;
	int is_string = 0;
	while(1) {
		char command[16], added_elem[MAX_STRING_SIZE];
		long nr, pos;
		scanf("%s", command);
		if(strncmp(command, "create_str", 10) == 0){
			doublyLinkedList = dll_create(MAX_STRING_SIZE);
			is_string = 1;
		}
		if(strncmp(command, "create_int", 10) == 0){
			doublyLinkedList = dll_create(sizeof(int));
			is_int = 1;
		}
		if(strncmp(command, "add", 3) == 0){
			scanf("%ld", &pos);

			if(is_int) {
				scanf("%ld", &nr);
				dll_add_nth_node(doublyLinkedList, pos, &nr);
			} else if(is_string) {
				scanf("%s", added_elem);
				dll_add_nth_node(doublyLinkedList, pos, added_elem);
			} else {
				printf("Create a list before adding elements!\n");
				exit(0);
			}
		}
		if(strncmp(command, "remove", 6) == 0){
			if(!is_int && !is_string) {
				printf("Create a list before removing elements!\n");
				exit(0);
			}

			scanf("%ld", &pos);
			dll_node_t* removed = dll_remove_nth_node(doublyLinkedList, pos);
			free(removed->data);
			free(removed);
		}
		if(strncmp(command, "print", 5) == 0){
			if(!is_int && !is_string) {
				printf("Create a list before printing!\n");
				exit(0);
			}

			if(is_int == 1){
				dll_print_int_list(doublyLinkedList);
			}
			if(is_string == 1){
				dll_print_string_list(doublyLinkedList);
			}
		}
		if(strncmp(command, "free", 4) == 0){
			if(!is_int && !is_string) {
				printf("Create a list before freeing!\n");
				exit(0);
			}
			dll_free(&doublyLinkedList);
			break;
		}
	}
	return 0;
}