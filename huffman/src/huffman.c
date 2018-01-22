#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"

typedef struct _Head {
	long compressed_size;
	long header_size;
	long decompressed_size;
} Header;

typedef struct _HuffNode {
	char val;
	int ascii;
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
			_huff_push(root, new);
		}
		str++;
	}
	HuffNode *ret = _huff_pop(root);
	_huff_free(root);
	return ret;
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
	fclose(compressed_f);

	HuffNode *root = _build_tree(huff_raw);
	_print_huff(root);
	free(huff_raw);

	_huff_free(root);
	return EXIT_SUCCESS;
}

int dehuff_b(const char *compressed_path, const char *decompressed_path) {
	return EXIT_SUCCESS;
}
