/* twosum.c */
#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
	char *arr[] = {"2", "7", "11", "15"};
	int target = 9;
	int length = sizeof(arr) / sizeof(char *);
	SymTable_T st = SymTable_new();

	// Fill SymTable
	for (int i = 0; i < length; i++)
		SymTable_put(st, arr[i], (void *) &i);

	// Search for complement and print indices of the solutions
	for (int i = 0; i < length; i++) {
		int cmp = target - atoi(arr[i]);
		char buf[100];
		snprintf(buf, sizeof(buf), "%d", cmp);

		if (SymTable_contains(st, (const char *) &buf) 
			&& atoi(SymTable_get(st, (const char *) &buf)) != i)
			printf("Indices: %d, %d\n", i, (int) atoi(SymTable_get(st, (const char *) &buf)));
	}

	SymTable_free(st);
	return 0;
}