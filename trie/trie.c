/*
    SD 2023 - Trie
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ALPHABET_SIZE 26
#define ALPHABET "abcdefghijklmnopqrstuvwxyz"

typedef struct trie_node_t trie_node_t;
struct trie_node_t {
    /* Value associated with key (set if end_of_word = 1) */
    void* value;

    /* 1 if current node marks the end of a word, 0 otherwise */
    int end_of_word;

    trie_node_t** children;
    int n_children;
};

typedef struct trie_t trie_t;
struct trie_t {
    trie_node_t* root;
    
    /* Number of keys */
    int size;

    /* Generic Data Structure */
    int data_size;

    /* Trie-Specific, alphabet properties */
    int alphabet_size;
    char* alphabet;

    /* Callback to free value associated with key, should be called when freeing */
    void (*free_value_cb)(void*);

    /* Optional - number of nodes, useful to test correctness */
    int nNodes;
};

int remove_word(trie_t* trie, trie_node_t *curr, char* key, int k);

trie_node_t* trie_create_node(trie_t * trie)
{
    trie_node_t* node = malloc(sizeof(trie_node_t));
    node->value = NULL;
    node->end_of_word = 0;
    node->children = malloc(sizeof(trie_node_t *) * trie->alphabet_size);
    for (int i = 0; i < ALPHABET_SIZE; i++)
        node->children[i] = NULL;
    node->n_children = 0;
    return node;
}

trie_t* trie_create(int data_size, int alphabet_size, char* alphabet,
void (*free_value_cb)(void*))
{
    trie_t* trie = malloc(sizeof(trie_t));
    trie->size = 0;
    trie->root = NULL;
    trie->data_size = data_size;
    trie->alphabet_size = alphabet_size;
    trie->alphabet = alphabet;
    trie->free_value_cb = free_value_cb;
    trie->nNodes = 1;
    return trie;
}

void trie_insert(trie_t* trie, char* key, void* value)
{
    int k = 0;
    if (!trie->root)
        trie->root = trie_create_node(trie);
    trie_node_t *curr = trie->root;
    while (key[k]) {
        int index = key[k] - 'a';
        if (!curr->children[index]) {
            curr->children[index] = trie_create_node(trie);
            trie->nNodes += 1;
            curr->n_children += 1;
        }
        curr = curr->children[index];
        k++;
    }
    curr->end_of_word = 1;
    if (curr->value)
        free(curr->value);
    curr->value = malloc(trie->data_size);
    memcpy(curr->value, value, trie->data_size);
}

void* trie_search(trie_t* trie, char* key)
{
    int k = 0;
    if (key[k] == '\0')
        return NULL;
    trie_node_t *curr = trie->root;
    while (key[k]) {
        int index = key[k] - 'a';
        if (!curr->children[index])
            return NULL;
        curr = curr->children[index];
        k++;
    }
    if (curr->end_of_word)
        return curr->value;
    return NULL;
}

int remove_word(trie_t* trie, trie_node_t *curr, char* key, int k)
{
    int n;
    if (key[k]) {
        if (!curr->children[key[k] - 'a'])
            return -1;
        n = remove_word(trie, curr->children[key[k] - 'a'], key, k+1);
    }
    if (!key[k] && !curr->end_of_word)
        return -1;
    if (!key[k]) {
        free(curr->value);
        curr->end_of_word = 0;
        return curr->n_children;
    }
    if (!n) {
        free(curr->children[key[k] - 'a']->children);
        free(curr->children[key[k] - 'a']);
        curr->children[key[k] - 'a'] = NULL;
        curr->n_children -= 1;
        trie->nNodes -= 1;
    }
    return curr->n_children;
}
void trie_remove(trie_t* trie, char* key)
{
    int k = 0;
    trie_node_t *curr = trie->root;
    int n = remove_word(trie, curr, key, k);
}

void free_node(trie_node_t *curr)
{
    for (int i = 0; i < ALPHABET_SIZE; i++)
        if (curr->children[i]) {
            free_node(curr->children[i]);
            free(curr->children[i]);
        }
    free(curr->value);
    free(curr->children);
}

void trie_free(trie_t** pTrie)
{
    trie_t *trie = *pTrie;
    trie_node_t *curr;
    curr = trie->root;
    if (curr) {
        free_node(curr);
        free(trie->root);
    }
    free(trie);
    pTrie = NULL;
}   

/* Needed for Lambda tests, ignore :) */
void cleanup_example_string(char* str) {
    int len = strlen(str);
    
    if(str[len-2] == '\\') {
        str[len-2] = '\0';
    }
}

int main() {
    int n, value;
    char alphabet[] = ALPHABET;
    char buf[256], key[256], op;
    
    trie_t* trie = trie_create(sizeof(int), ALPHABET_SIZE, alphabet, free);
    
    fgets(buf, 256, stdin);
	sscanf(buf, "%d\n", &n);

    for(int i = 0; i < n; ++i) {
        fgets(buf, 256, stdin);
	    sscanf(buf, "%c", &op);

        if(op == 'i') {
	        sscanf(buf, "%c %s %d\n", &op, key, &value);
            trie_insert(trie, key, &value);
        } else if(op == 'r') {
	        sscanf(buf, "%c %s\n", &op, key);
            cleanup_example_string(key);
            printf("nNodes before removing %s: %d\n", key, trie->nNodes);
            trie_remove(trie, key);
            printf("nNodes after removing %s: %d\n", key, trie->nNodes);
        } else if(op == 's') {
            sscanf(buf, "%c %s\n", &op, key);
            cleanup_example_string(key);

            if(key[0] == '_') {
                key[0] = '\0';
            }

            int* found = trie_search(trie, key);
            printf("%s: ", key[0] == '\0' ? "_" : key);

            if(found) {
                printf("%d\n", *found);
            } else if (key[0] == '\0') { 
                printf("-1");
            } else {
                printf("not found\n");
            }
            
        }
    }

    trie_free(&trie);

    return 0;
}