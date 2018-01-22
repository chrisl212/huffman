#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffnode.h"

HuffNode *huff_new(char val) {
	HuffNode *new = calloc(1, sizeof(*new));
	new->val = val;
	new->ascii = 1;
	return new;
}

HuffNode *huff_pop(HuffNode *list) {
	while (list->next->next) {
		list = list->next;
	}
	HuffNode *pop = list->next;
	list->next = NULL;
	return pop;
}

void huff_push(HuffNode *list, HuffNode *new) {
	while (list->next) {
		list = list->next;
	}
	list->next = new;
}

void huff_free(HuffNode *node) {
	if (node->next) {
		huff_free(node->next);
	}
	if (node->left) {
		huff_free(node->left);
	}
	if (node->right) {
		huff_free(node->right);
	}
	free(node);
}

void fprint_huff(FILE *f, HuffNode *node, char *str, int size) {
	if (!node) {
		//returns if the pointer is NULL
		return;
	}
	if (!node->left && !node->right) {
		//end of a branch, prints the character and its associated bit code
		str[size-1] = 0; //adds a null terminator
		fprintf(f, "%c:%s\n", node->val, str);
		return;
	}
	char *c = malloc(size+1);
	if (str) {
		memcpy(c, str, size); //if str exists, copy it into c
	}
	c[size-1] = '0'; //adds a 0 for left branch
	fprint_huff(f, node->left, c, size+1);
	c[size-1] = '1'; //adds a 1 for right branch
	fprint_huff(f, node->right, c, size+1);
	free(c); //free c
}

