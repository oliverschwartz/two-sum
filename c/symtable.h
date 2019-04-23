/*--------------------------------------------------------------------*/
/* symtable.h                                                         */
/* Author: Oliver Schwartz                                            */
/*--------------------------------------------------------------------*/
#include <stddef.h>
#ifndef SYM_INCLUDED
#define SYM_INCLUDED
/*--------------------------------------------------------------------*/

/* a SymTable_T is a collection of key:value bindings */
typedef struct SymTable_T *SymTable_T;

/*--------------------------------------------------------------------*/

/* Constructs new SymTable data structure. Void input; returns new
   SymTable_T data structure. Returns NULL if insufficient memory
   available. */
SymTable_T SymTable_new(void);

/* Frees all memory occupied by the symbol table; input: oSymTable 
   symbol table; returns NULL */
void SymTable_free(SymTable_T oSymTable);

/* returns the number of bindings pairs in the symbol table;
   input: oSymTable symbol table */
size_t SymTable_getLength(SymTable_T oSymTable);

/* Inserts a new binding into symbol table. Adds binding and 
   returns 1 if ST does not contain the key. Else, function does not
   change the ST and returns 0. If insufficient memory, 0 returned. 
   Input: oSymTable symbol table, pcKey char array, and void pointer 
   pvValue. (Note: a defensive copy of the value is not created, 
   so it is up to the client to allocate/free this memory).
   However, after calling put() the client may free memory 
   alloc'ed to pcKey (as a defensive copy is created). (Also
   Note: the value stored, pvValue, can be NULL). */
int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
                                       const void *pvValue);

/* Replaces value of binding that contains pcKey with pvValue. 
   If ST contains binding with pcKey, replace binding's value with
   pvValue and return the old value. Otherwise, do nothing, return NULL.
   Input: oSymTable symbol table, pcKey char array, and void pointer 
   pvValue */
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
   const void *pvValue);

/* Checks if ST contains pcKey. Returns 1 if yes. If not, returns 0. 
   Input: oSymTable symbol table, pcKey char array holding key */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* Returns the value of the binding in ST whose key is pcKey.
   If such a key doesn't exist, return NULL. Input: oSymTable
   symbol table, pcKey char array holding key */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* Removes binding containing pcKey from symbol table.
   If ST contains a binding with pcKey, remove this binding and return 
   oointer to binding's value. Otherwise, do nothing and return NULL. 
   Input: oSymTable symbol table, pcKey char array holding key */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* applies pfApply() to each binding in the symbol table 
   (void return type). Input: oSymTable symbol table, pvExtra (a 
   pointer to an extra parameter) */
void SymTable_map(SymTable_T oSymTable,
   void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
   const void *pvExtra);

/*--------------------------------------------------------------------*/

#endif