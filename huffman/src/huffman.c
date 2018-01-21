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
	struct _HuffNode *left;
	struct _HuffNode *right;
} HuffNode;

char *_reverse(const char *str, int len) {
	char *new = malloc(sizeof(*new)*len);
	for (int i = 2; i <= len; i++) {
		new[i-2] = str[len-i];
	}
	new[len-1] = '\0';
	return new;
}

HuffNode *_build_tree(char *str) {
	HuffNode *node = calloc(1, sizeof(*node));
	while (*str) {
		putchar(*str);
		if (*str == '0') {
			if (!node->right) {
				node->right = _build_tree(++str);
			} else {
				return node;
			}
		} else if (*str != '1') {
			if (!node->right) {
				node->right = calloc(1, sizeof(*node->right));
				node->right->val = *str;
			} else {
				node->left = calloc(1, sizeof(*node->left));
				node->left->val = *str;
			}
		}
		str++;
	}
	return node;	
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
	char *reversed_huff = _reverse(huff_raw, header.header_size-1);
	puts(reversed_huff);
	HuffNode *root = _build_tree(reversed_huff);
	putchar('\n');
	_print_huff(root);

	free(reversed_huff);
	free(huff_raw);
	fclose(compressed_f);
	return EXIT_SUCCESS;
}

int dehuff_b(const char *compressed_path, const char *decompressed_path) {
	return EXIT_SUCCESS;
}
