#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "util.h"

void print_usage(void)
{
	printf("usage: archy <key> [...]\n"
		"keys:\n"
		"-h\thelp\n"
		"-c ARCHIVE SOURCE\tmake archive\n"
		"-x DECOMPRESSED ARCHIVE\tdecompress\n");
}

int main(int argc, char* argv[])
{
	long long *stat;
	node *root;
	FILE *fi, *fo;
	if (argc < 2) 
	{ 
		printf("Wrong arguments\n");
		print_usage();
		exit(1);
	}
	if (strcmp(argv[1], "-h") == 0)
	{
		print_usage();
		exit(0);
	}

	if (((fi = fopen(argv[2], "rb")) && fi == NULL) || 
			((fo = fopen(argv[3], "wb")) && fo == NULL))
       	{
		printf("%s(%d): ", __FILE__, __LINE__);
		perror(argv[2]);
		exit(1); 
	}
       	stat = malloc(256*sizeof(long long));
	
	if (strcmp(argv[1], "-c") == 0)
	{
		get_stat(stat, fi);
		root = make_tree(stat);
		save_stat(stat, fo);
		rewind(fi);
		compress(root, fi, fo, stat);
	}
	if (strcmp(argv[1], "-x") == 0)
	{
		load_stat(stat, fi);
		root = make_tree(stat);
		decompress(root, fi, fo);
	}

	free_tree(root);
	free(stat);

	if (fclose(fi) == EOF || fclose(fo) == EOF)
       	{
		printf("%s(%d): %s", __FILE__, __LINE__, strerror(errno));
	}

	return 0;
}
