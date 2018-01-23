#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"
#include "huffnode.h"

//builds a binary tree from a bit-encoded header
HuffNode *_build_tree(char *str, char opt) {
	HuffNode *root = huff_new(0); //creates the root node of the stack
	int loc = 0; //the initial location of the read bit
	while (*str) {
		unsigned char condition = 0;
		if (opt == 'b') {
			unsigned char read_bit = 0x80 >> loc; //the bit that will be used to read a 1 or 0 from the header
			condition = *str & read_bit; //reads the bit from the header
		} else {
			condition = (*str == '1');
		}
		if (condition) {
			unsigned char val = 0;
			if (opt == 'b') {
				unsigned char read = 0xFF >> (loc+1); //if the value is non-zero (a 1), read 8 bits and push that onto the stack
				val = (*str & read) << (loc+1);
				read = 0xFF >> (7-loc);
				val |= ((read & *(++str) >> (7-loc)));
			} else {
				val = *(++str);
			}
			HuffNode *new = huff_new(val);
			huff_push(root, new);
		} else {
			//if the value is zero, pop two nodes from the stack and push a new node containing them
			if (!root->next->next) {
				break;
			}
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
		if (opt =='b' && ++loc == 8) {
			//if you've reached the end of the byte, reset the location and increment the pointer
			loc = 0;
			str++;
		} else if (opt == 'c') {
			str++;
		}
	}
	while (root->next && root->next->next) {
		//ensure that all nodes have been added to the tree
		HuffNode *right = huff_pop(root);
		HuffNode *left = huff_pop(root);
		HuffNode *new = huff_new(0);
		new->ascii = 0;
		new->left = left;
		new->right = right;
		huff_push(root, new);
	}
	HuffNode *ret = huff_pop(root); //pop the tree from the stack and free the root node
	huff_free(root);
	return ret;
}

//dehuffs a compressed buffer to a file using the huffman tree provided
int _decompress_to_f(HuffNode *root, char *compressed_buf, FILE *decompressed_f, long size) {
	HuffNode *copy = root; //creates a copy of the huffman tree
	long decoded = 0; //counts the number of decoded characters
	while (decoded != size) {
		//loops until all bytes have been decoded
		for (int loc = 0; loc < 8; loc++) {
			char read_bit = 0x80 >> loc; //the bit used to read the compressed data
			char bit = read_bit & *compressed_buf; //reads the data bit by bit
			if (bit) {
				//if a 1, move to the right in the tree
				copy = copy->right;
			} else {
				//if a 0, move to the left in the tree
				copy = copy->left;
			}
			if (copy->ascii) {
				//if the current node in the tree is a leaf node, print the character and reset the tree
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

//builds a huffman tree and decompresses a file using a char-based header
int dehuff(const char *compressed_path, const char *code_path, const char *decompressed_path, char opt) {
	FILE *compressed_f = fopen(compressed_path, "r"); //opens the compressed file for reading
	if (!compressed_f) {
		return EXIT_FAILURE;
	}
	fseek(compressed_f, 0, SEEK_END);
	long fsize = ftell(compressed_f);
	fseek(compressed_f, 0, SEEK_SET); //calculates the compressed file's size

	Header header = {0}; //initializes the header
	fread(&header, sizeof(header), 1, compressed_f); //reads in the header
	if (header.compressed_size != fsize) {
		//if the file sizes do not match, the file is corrupt
		fclose(compressed_f);
		return EXIT_FAILURE;
	}
	char *huff_raw = malloc(header.header_size);
	fread(huff_raw, 1, header.header_size, compressed_f); //reads the header data
	if (opt == 'c') {
		huff_raw[header.header_size-1] = 0; //cuts off last 0
	}

	HuffNode *root = _build_tree(huff_raw, opt);
	if (!root) {
		//if the huffman tree is not successfully created, the file is corrupt
		fclose(compressed_f);
		free(huff_raw);
		return EXIT_FAILURE;
	}
	free(huff_raw);
	
	if (opt == 'c') {
		FILE *code_f = fopen(code_path, "w"); //opens the file for the huffman code
		if (!code_f) {
			huff_free(root);
			return EXIT_FAILURE;
		}
		fprint_huff(code_f, root, NULL, 1); //prints the proper output to the code file
		fclose(code_f);
	}

	long compressed_size = header.compressed_size-header.header_size-3*sizeof(long); //the length of the compressed data in the file
	char *compressed_buf = malloc(compressed_size+1);
	fread(compressed_buf, 1, compressed_size, compressed_f); //reads the compressed data from the file
	compressed_buf[compressed_size] = '\0';
	fclose(compressed_f);

	FILE *decompressed_f = fopen(decompressed_path, "w");
	_decompress_to_f(root, compressed_buf, decompressed_f, header.decompressed_size); //decompresses the file
	fclose(decompressed_f);

	huff_free(root);
	free(compressed_buf);
	return EXIT_SUCCESS;
}

