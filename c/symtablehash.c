/*--------------------------------------------------------------------*/
/* symtablehash.c                                                     */
/* Author: Oliver Schwartz                                            */
/*--------------------------------------------------------------------*/
#include "symtable.h"
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
/*--------------------------------------------------------------------*/
#define TRUE  1
#define FALSE 0
#define INITIAL_BUCKETS 509
#define MAX_BUCKETS 65521
/*--------------------------------------------------------------------*/

/* all possible values for the number of buckets in symbol table */
static size_t bucketSizes[] = {509, 1021, 2039, 4093, 8191, 16381,
                               32749, 65521};

/*--------------------------------------------------------------------*/

/* node structure */
struct Node {
    /* key associate with binding */
    const char *key;

    /* value associated with binding */
    const void *value;

    /* pointer to next node in linked-list (LL) */
    struct Node *next; 
};

/* symbol table structure (typdef'ed as SymTable_T) */
struct SymTable_T {
    /* array of pointers to Nodes */
    struct Node **buckets;

    /* number of bindings in ST */
    size_t bindingCount;

    /* number of buckets in ST (i.e. length of buckets array)*/
    size_t bucketCount;

    /* index in global array 'bucketSizes' */
    int bucketSizeIndex;
};

/* static method to return a hash code for pcKey that is between 
   0 and uBucketCount-1, inclusive. Input: oSymTable symble table,
   and pcKey, a char array containing the key to be hashed. (adapted 
   from assignment version) */   
static size_t SymTable_hash(SymTable_T oSymTable, const char *pcKey)
{
    const size_t HASH_MULTIPLIER = 65599;
    size_t uHash = 0;
    size_t u;
    assert(pcKey != NULL);
    for (u = 0; pcKey[u] != '\0'; u++)
        uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];
    return uHash % oSymTable->bucketCount;
}

/* Constructs new SymTable data structure. Void input; returns new
   SymTable_T data structure */
SymTable_T SymTable_new(void) {
    SymTable_T oSymTable= (SymTable_T)malloc(sizeof(struct SymTable_T));    
    if (oSymTable == NULL) /* check for sufficient memory */
        return NULL;
    
    /* initialize array of Node pointers */
    oSymTable->bindingCount = 0;
    oSymTable->bucketSizeIndex = 0;
    oSymTable->bucketCount = INITIAL_BUCKETS;
    oSymTable->buckets = calloc(INITIAL_BUCKETS, sizeof(struct Node *));
    return oSymTable;
}

/* Frees all memory occupied by the symbol table; input: oSymTable 
   symbol table; returns NULL */
void SymTable_free(SymTable_T oSymTable) {
    struct Node *currentNode;
    struct Node *nextNode;
    size_t i;
    assert(oSymTable != NULL);

    /* for each bucket in array, free corresponding linked list */
    for (i = 0; i < oSymTable->bucketCount; i++) {
        if (oSymTable->buckets[i] != NULL) {
            for (currentNode = oSymTable->buckets[i]; 
                 currentNode != NULL; currentNode = nextNode) {
                nextNode = currentNode->next;
                free((void *)currentNode->key);/* free defensive copy */
                free(currentNode);
            }
        }
    }
    free(oSymTable->buckets);
    free(oSymTable);
}

/* returns the number of bindings pairs in the symbol table;
   input: oSymTable symbol table */
size_t SymTable_getLength(SymTable_T oSymTable) {
    return oSymTable->bindingCount;
}

/* Inserts a new binding into symbol table. Adds binding and 
   returns 1 if ST does not contain the key. Else, function does not
   change the ST and returns 0. If insufficient memory, 0 returned. 
   Input: oSymTable symbol table, pcKey char array, and void pointer 
   pvValue. (Note: a defensive copy of the value is not created, 
   so it is up to the client to allocate/free this memory) */
int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
                                       const void *pvValue) {
    size_t index;
    size_t i;
    struct Node *currentNode;
    struct Node *nextNode;
    struct Node *newNode;
    assert(oSymTable != NULL && pcKey != NULL);

    /* resize bucket array if bindingCount is equal to bucketCount */
    if (oSymTable->bucketCount == oSymTable->bindingCount && 
          oSymTable->bindingCount != MAX_BUCKETS) {
        struct Node *movedNode;
        struct Node **oldBuckets;
        oSymTable->bucketSizeIndex++;
        oSymTable->bucketCount= bucketSizes[oSymTable->bucketSizeIndex];
        oldBuckets = oSymTable->buckets; 
        oSymTable->buckets = calloc(oSymTable->bucketCount, 
                                    sizeof(struct Node *));
        assert(oSymTable->buckets != NULL); /* check enough memory */

        /* rehash all current elements in symbol table and reinsert
           in newly allocated bucket array */
        for (i = 0; i < bucketSizes[oSymTable->bucketSizeIndex - 1]; 
                                                                  i++) {
            for (currentNode = oldBuckets[i]; 
                  currentNode != NULL; currentNode = nextNode) {
                nextNode = currentNode->next;
                index = SymTable_hash(oSymTable, currentNode->key);
                
                /* put binding at new hash location */
                movedNode = (struct Node*)malloc(sizeof(struct Node));
                assert(movedNode != NULL); /* check enough memory */
                movedNode->value = currentNode->value;
                movedNode->key = (const char*)
                                   malloc(strlen(currentNode->key) + 1);
                strcpy((char*)movedNode->key, currentNode->key);
                movedNode->next = oSymTable->buckets[index];
                oSymTable->buckets[index] = movedNode;

                /* free old node and its defensive copy of key */
                free((void *)currentNode->key);
                free(currentNode);
            }
        }
        free(oldBuckets);
    }

    /* if key already in ST, do nothing and return */
    if (SymTable_contains(oSymTable, pcKey)) 
        return FALSE;
    
    /* compute index in bucket array */
    index = SymTable_hash(oSymTable, pcKey);

    /* add a new node to ST with given key & value */
    newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode == NULL) /* check if there is enough memory */
        return FALSE;
    
    /* create a new node & add to front of linked list starting
       at the given bucket, saving a defensive copy of the key */
    newNode->key  = (const char*)malloc(strlen(pcKey) + 1);
    strcpy((char*)newNode->key, pcKey);
    newNode->value = pvValue;
    newNode->next  = oSymTable->buckets[index];
    oSymTable->buckets[index] = newNode;
    oSymTable->bindingCount++;
    return TRUE;
}

/* Replaces value of binding that contains pcKey with pvValue. 
   If ST contains binding with pcKey, replace binding's value with
   pvValue and return the old value. Otherwise, do nothing, return NULL.
   Input: oSymTable symbol table, pcKey char array, and void pointer 
   pvValue */
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
                                             const void *pvValue) {
    struct Node *currentNode;
    struct Node *nextNode;
    const void *oldValue;
    size_t index; 
    assert(oSymTable != NULL && pcKey != NULL);

    /* compute hash of key */
    index = SymTable_hash(oSymTable, pcKey);

    /* look for key in ST; find it and update its value; if it's not
       there, return NULL */
    for (currentNode = oSymTable->buckets[index]; 
         currentNode != NULL; currentNode = nextNode) {
        if (strcmp(currentNode->key, pcKey) == 0) {
            oldValue = currentNode->value;
            currentNode->value = pvValue;
            return (void *)oldValue;
        }
        nextNode = currentNode->next; 
    }
    return NULL;
}

/* Checks if ST contains pcKey. Returns 1 if yes. If not, returns 0. 
   Input: oSymTable symbol table, pcKey char array holding key */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    struct Node *currentNode;
    struct Node *nextNode;
    size_t index;
    assert(oSymTable != NULL && pcKey != NULL);

    /* first, check whether bucket points to something */
    index = SymTable_hash(oSymTable, pcKey);
    if (oSymTable->buckets[index] == NULL)
        return FALSE;
    
    /* traverse list and see if the key exists */
    for (currentNode = oSymTable->buckets[index]; 
         currentNode != NULL; currentNode = nextNode) {
        if (strcmp(currentNode->key, pcKey) == 0) {
            return TRUE;
        }
        nextNode = currentNode->next;
    }
    return FALSE;
}

/* Returns the value of the binding in ST whose key is pcKey.
   If such a key doesn't exist, return NULL. Input: oSymTable
   symbol table, pcKey char array holding key */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    struct Node *currentNode;
    struct Node *nextNode;
    size_t index;
    assert(oSymTable != NULL && pcKey != NULL);
    
    /* check whether bucket points to something */
    index = SymTable_hash(oSymTable, pcKey);
    if (oSymTable->buckets[index] == NULL)
        return NULL;

    /* traverse list and see if the key exists; if it 
       does, return a pointer to its value */
    for (currentNode = oSymTable->buckets[index]; 
         currentNode != NULL; currentNode = nextNode) {
        if (strcmp(currentNode->key, pcKey) == 0) {
            return (void *)currentNode->value;
        }
        nextNode = currentNode->next;
    }
    return NULL;
}

/* Removes binding containing pcKey from symbol table.
   If ST contains a binding with pcKey, remove this binding and return 
   oointer to binding's value. Otherwise, do nothing and return NULL. 
   Input: oSymTable symbol table, pcKey char array holding key */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    const void* value;
    struct Node *currentNode;
    struct Node *nextNode;
    struct Node *prevNode;
    size_t index;
    assert(oSymTable != NULL && pcKey != NULL);

    /* check whether bucket points to something */
    index = SymTable_hash(oSymTable, pcKey);   
    if (oSymTable->buckets[index] == NULL)
        return NULL;

    /* iterate through linked list, finding correct one to remove */
    for (currentNode = oSymTable->buckets[index], 
         prevNode = currentNode; currentNode != NULL; 
                                 currentNode = nextNode) {
        if (strcmp(currentNode->key, pcKey) == 0) {
            oSymTable->bindingCount--;
            /* free node and return value */
            if (currentNode != oSymTable->buckets[index]) {
                prevNode->next = currentNode->next;
                value = currentNode->value;

                /* free defensive copy of key */
                free((void *)currentNode->key); 

                free(currentNode);
                return (void *)value;
            }
            /* corner case: where key is in first node */
            else {
                oSymTable->buckets[index] = currentNode->next;
                value = currentNode->value;

                /* free defensive copy of key */
                free((void *)currentNode->key);

                free(currentNode);
                return (void *)value;
            }
        }
        prevNode = currentNode;
        nextNode = currentNode->next;
    }
    return NULL;
}

/* applies pfApply() to each binding in the symbol table 
   (void return type). Input: oSymTable symbol table, pvExtra (a 
   pointer to an extra parameter) */
void SymTable_map(SymTable_T oSymTable, 
   void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
                   const void *pvExtra) {
    size_t i;
    struct Node *currentNode;
    struct Node *nextNode;
    assert(oSymTable != NULL && pfApply != NULL);

    /* iterate through all buckets and apply pfApply to nodes */
    for (i = 0; i < oSymTable->bucketCount; i++) {
        /* check there is a linked list at given bucket */
        if (oSymTable->buckets[i] != NULL) {
            for (currentNode = oSymTable->buckets[i]; 
                currentNode != NULL; currentNode = nextNode) {
                (*pfApply)(currentNode->key, 
                           (void *)currentNode->value, 
                           (void *)pvExtra);
                nextNode = currentNode->next;
            }
        }
    }
}