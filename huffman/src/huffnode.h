#ifndef HUFFNODE_H
#define HUFFNODE_H

typedef struct _HuffNode {
	char val;
	int ascii;
	struct _HuffNode *next;
	struct _HuffNode *left;
	struct _HuffNode *right;
} HuffNode;

HuffNode *huff_new(char val);
HuffNode *huff_pop(HuffNode *list);
void huff_push(HuffNode *list, HuffNode *node);
void huff_free(HuffNode *node);
void fprint_huff(FILE *f, HuffNode *node, char *str, int size); 

#endif
