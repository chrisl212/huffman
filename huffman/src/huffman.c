#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

typedef struct _Head {
	long compressed_size;
	long header_size;
	long decompressed_size;
} Header;

typedef struct _HuffNode {
	char val;
	int ascii;
	struct _HuffNode *parent;
	struct _HuffNode *next;
	struct _HuffNode *left;
	struct _HuffNode *right;
} HuffNode;

HuffNode *_huff_new(char val) {
	HuffNode *new = calloc(1, sizeof(*new));
	new->val = val;
	new->ascii = 1;
	return new;
}

HuffNode *_huff_pop(HuffNode *list) {
	while (list->next->next) {
		list = list->next;
	}
	HuffNode *pop = list->next;
	list->next = NULL;
	return pop;
}

void _huff_push(HuffNode *list, HuffNode *new) {
	while (list->next) {
		list = list->next;
	}
	list->next = new;
}

void _huff_free(HuffNode *node) {
	if (node->next) {
		_huff_free(node->next);
	}
	if (node->left) {
		_huff_free(node->left);
	}
	if (node->right) {
		_huff_free(node->right);
	}
	free(node);
}

HuffNode *_build_tree(char *str) {
	HuffNode *root = _huff_new(0);
	while (*str) {
		if (*str == '1') {
			HuffNode *new = _huff_new(*(++str));
			_huff_push(root, new);
		} else if (*str == '0') {
			HuffNode *right = _huff_pop(root);
			HuffNode *left = _huff_pop(root);
			HuffNode *new = _huff_new(0);
			new->ascii = 0;
			new->left = left;
			new->right = right;
			left->parent = new;
			right->parent = new;
			_huff_push(root, new);
		}
		str++;
	}
	HuffNode *ret = _huff_pop(root);
	_huff_free(root);
	return ret;
}

void _fprint_huff(FILE *f, HuffNode *node, char *str, int size) {
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
	_fprint_huff(f, node->left, c, size+1);
	c[size-1] = '1'; //adds a 1 for right branch
	_fprint_huff(f, node->right, c, size+1);
	free(c); //free c
}

void _print_huff(HuffNode *node) {
	if (node->left) {
		_print_huff(node->left);
	}
	if (node->right) {
		_print_huff(node->right);
	}
	if (node->val == '\0') {
		fprintf(stdout, "0");
	} else {
		fprintf(stdout, "1%c", node->val);
	}
}

int dehuff_c(const char *compressed_path, const char *code_path, const char *decompressed_path) {
	FILE *compressed_f = fopen(compressed_path, "r");
	if (!compressed_f) {
		return EXIT_FAILURE;
	}

	Header header = {0};
	fread(&header, sizeof(header), 1, compressed_f);
	char *huff_raw = malloc(header.header_size);
	fread(huff_raw, 1, header.header_size, compressed_f);	
	huff_raw[header.header_size-1] = 0; //cuts off last 0

	HuffNode *root = _build_tree(huff_raw);
	free(huff_raw);
	FILE *code_f = fopen(code_path, "w");
	if (!code_f) {
		_huff_free(root);
		return EXIT_FAILURE;
	}
	_fprint_huff(code_f, root, NULL, 1);
	fclose(code_f);

	long compressed_size = header.compressed_size-header.header_size-3*sizeof(long);
	char *compressed_buf = malloc(compressed_size+1);
	fread(compressed_buf, 1, compressed_size, compressed_f);
	fclose(compressed_f);

	FILE *decompressed_f = fopen(decompressed_path, "w");
	HuffNode *copy = root;
	while (*compressed_buf) {
		for (int loc = 0; loc < 8; loc++) {
			char read_bit = 0x01 << loc;
			char bit = read_bit & *compressed_buf;
			printf("%d", (bit != 0));
			if (bit) {
				copy = copy->right;
			} else {
				copy = copy->left;
			}
			if (copy->ascii) {
				//fprintf(stdout, "%c", copy->val);
				copy = root;
			}
		}
		compressed_buf++;
	}
	fclose(decompressed_f);

	fclose(compressed_f);
	_huff_free(root);
	return EXIT_SUCCESS;
}

int dehuff_b(const char *compressed_path, const char *decompressed_path) {
	return EXIT_SUCCESS;
}
