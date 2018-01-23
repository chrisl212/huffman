#ifndef HUFFMAN_H
#define HUFFMAN_H

//structure for the header of the file containing the three important numbers
typedef struct _Head {
	long compressed_size;
	long header_size;
	long decompressed_size;
} Header;

int dehuff(const char *compressed_path, const char *code_path, const char *decompressed_path, char opt);

#endif
