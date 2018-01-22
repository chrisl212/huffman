#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"
#include "huffnode.h"

typedef struct _Head {
	long compressed_size;
	long header_size;
	long decompressed_size;
} Header;

HuffNode *_build_tree_c(char *str) {
	HuffNode *root = huff_new(0);
	while (*str) {
		if (*str == '1') {
			HuffNode *new = huff_new(*(++str));
			huff_push(root, new);
		} else if (*str == '0') {
			HuffNode *right = huff_pop(root);
			if (!root->next) {
				huff_free(right);
				huff_free(root);
				return NULL;
			}
			HuffNode *left = huff_pop(root);
			HuffNode *new = huff_new(0);
			new->ascii = 0;
			new->left = left;
			new->right = right;
			huff_push(root, new);
		}
		str++;
	}
	HuffNode *ret = huff_pop(root);
	huff_free(root);
	return ret;
}

HuffNode *_build_tree_b(char *str) {
	HuffNode *root = huff_new(0);
	int loc = 0;
	while (*str) {
		unsigned char read_bit = 0x80 >> loc;
		unsigned char bit = *str & read_bit;
		if (bit) {
			unsigned char read = 0xFF >> (loc+1);
			unsigned char val = (*str & read) << (loc+1);
			read = 0xFF >> (7-loc);
			val |= ((read & *(++str) >> (7-loc)));
			HuffNode *new = huff_new(val);
			huff_push(root, new);
		} else {
			if (!root->next->next) {
				break;
			}
			HuffNode *right = huff_pop(root);
			HuffNode *left = huff_pop(root);
			HuffNode *new = huff_new(0);
			new->ascii = 0;
			new->left = left;
			new->right = right;
			huff_push(root, new);
		}
		if (++loc == 8) {
			loc = 0;
			str++;
		}
	}
	while (root->next && root->next->next) {
		HuffNode *right = huff_pop(root);
		HuffNode *left = huff_pop(root);
		HuffNode *new = huff_new(0);
		new->ascii = 0;
		new->left = left;
		new->right = right;
		huff_push(root, new);
	}
	HuffNode *ret = huff_pop(root);
	huff_free(root);
	return ret;
}


int _decompress_to_f(HuffNode *root, char *compressed_buf, FILE *decompressed_f, long size) {
	HuffNode *copy = root;
	long decoded = 0;
	while (decoded != size) {
		for (int loc = 0; loc < 8; loc++) {
			char read_bit = 0x80 >> loc;
			char bit = read_bit & *compressed_buf;
			if (bit) {
				copy = copy->right;
			} else {
				copy = copy->left;
			}
			if (copy->ascii) {
				fprintf(decompressed_f, "%c", copy->val);
				copy = root;
				if (++decoded == size) {
					break;
				}
			}
		}
		compressed_buf++;
	}
	return EXIT_SUCCESS;	
}

int dehuff_c(const char *compressed_path, const char *code_path, const char *decompressed_path) {
	FILE *compressed_f = fopen(compressed_path, "r");
	if (!compressed_f) {
		return EXIT_FAILURE;
	}
	fseek(compressed_f, 0, SEEK_END);
	long fsize = ftell(compressed_f);
	fseek(compressed_f, 0, SEEK_SET);

	Header header = {0};
	fread(&header, sizeof(header), 1, compressed_f);
	if (header.compressed_size != fsize) {
		fclose(compressed_f);
		return EXIT_FAILURE;
	}
	char *huff_raw = malloc(header.header_size);
	fread(huff_raw, 1, header.header_size, compressed_f);	
	huff_raw[header.header_size-1] = 0; //cuts off last 0

	HuffNode *root = _build_tree_c(huff_raw);
	if (!root) {
		fclose(compressed_f);
		free(huff_raw);
		return EXIT_FAILURE;
	}
	free(huff_raw);
	FILE *code_f = fopen(code_path, "w");
	if (!code_f) {
		huff_free(root);
		return EXIT_FAILURE;
	}
	fprint_huff(code_f, root, NULL, 1);
	fclose(code_f);

	long compressed_size = header.compressed_size-header.header_size-3*sizeof(long);
	char *compressed_buf = malloc(compressed_size+1);
	fread(compressed_buf, 1, compressed_size, compressed_f);
	compressed_buf[compressed_size] = '\0';
	fclose(compressed_f);

	FILE *decompressed_f = fopen(decompressed_path, "w");
	_decompress_to_f(root, compressed_buf, decompressed_f, header.decompressed_size);
	fclose(decompressed_f);

	huff_free(root);
	free(compressed_buf);
	return EXIT_SUCCESS;
}

int dehuff_b(const char *compressed_path, const char *decompressed_path) {
	FILE *compressed_f = fopen(compressed_path, "r");
	if (!compressed_f) {
		return EXIT_FAILURE;
	}
	fseek(compressed_f, 0, SEEK_END);
	long fsize = ftell(compressed_f);
	fseek(compressed_f, 0, SEEK_SET);

	Header header = {0};
	fread(&header, sizeof(header), 1, compressed_f);
	if (header.compressed_size != fsize) {
		fclose(compressed_f);
		return EXIT_FAILURE;
	}
	char *huff_raw = malloc(header.header_size);
	fread(huff_raw, 1, header.header_size, compressed_f);	

	HuffNode *root = _build_tree_b(huff_raw);
	if (!root) {
		fclose(compressed_f);
		free(huff_raw);
		return EXIT_FAILURE;
	}
	free(huff_raw);

	long compressed_size = header.compressed_size-header.header_size-3*sizeof(long);
	char *compressed_buf = malloc(compressed_size+1);
	fread(compressed_buf, 1, compressed_size, compressed_f);
	compressed_buf[compressed_size] = '\0';
	fclose(compressed_f);

	FILE *decompressed_f = fopen(decompressed_path, "w");
	_decompress_to_f(root, compressed_buf, decompressed_f, header.decompressed_size);
	fclose(decompressed_f);

	huff_free(root);
	free(compressed_buf);
	return EXIT_SUCCESS;
}

