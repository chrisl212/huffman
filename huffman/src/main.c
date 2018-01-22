#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

void _usage(const char *pname) {
	fprintf(stderr, "usage: %s -[b|c] args\n", pname);
}

int main(int argc, char **argv) {
	char *pname = argv[0];
	if (argc < 2) {
			_usage(pname);
			return EXIT_FAILURE;
	}
	char *opt = argv[1];
	if (strcmp(opt, "-b") == 0) {
		if (argc != 4) {
			_usage(pname);
			return EXIT_FAILURE;
		}
		return dehuff_b(argv[2], argv[3]);
	} else if (strcmp(opt, "-c") == 0) {
		if (argc != 5) {
				_usage(pname);
				return EXIT_FAILURE;
		}
		return dehuff_c(argv[2], argv[3], argv[4]);
	} else {
		_usage(pname);
		return EXIT_FAILURE;
	}


	return EXIT_SUCCESS;
}

