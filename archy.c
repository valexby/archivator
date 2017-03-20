#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>

#define TOKEN_SZ 300*1024*1024

typedef struct
{
	void *right, *left;
	unsigned char symbol;
	long long weight;
} leaf;

int get_stat(long long *, char[]);
int array_sum(long long *, int);
int compare_leafs(const void *, const void *);
void make_tree(long long *);

int get_stat(long long *stat, char file_name[])
{
	int i, j;
	FILE *fd;
	unsigned char *buf = malloc(TOKEN_SZ*sizeof(unsigned char));
	memset(stat, 0, sizeof(long long)*256);

	fd = fopen(file_name, "rb");

	while ((i = fread(buf, 1, TOKEN_SZ, fd)) && i > 0)
	{
		for (j = 0 ; j < i ; j++)
		{
			stat[(size_t)buf[j]]++;
		}
	}

	free(buf);
	if ( fclose(fd) != 0 )
	{
		perror("archy");
		return 1;
	}
	return 0;
}

int array_sum(long long *array, int size)
{
	long long sum = 0;
	for (size -= 1; size >= 0; size--)
		sum += array[0];
	return sum;
}

int compare_leafs(const void *A, const void *B)
{
	leaf* a = *((leaf**)A);
	leaf* b = *((leaf**)B);
	if ((*a).weight > (*b).weight) return 1;
	return -1;
}

void make_tree(long long *stat)
{
	unsigned char i, size = 0;
	leaf **leafs;
	leafs = malloc(256 * sizeof(leaf*));
	i = 255;
	do
	{
		if (stat[i] == 0) 
		{
			i--;
			continue;
		}
		leafs[size] = malloc(sizeof(leaf));
		(*(leafs[size])).right = NULL;
		(*(leafs[size])).left = NULL;
		(*(leafs[size])).symbol = i;
		(*(leafs[size])).weight = stat[i];
		size++;
		i--;
	}
	while (i != 255);
	qsort(leafs, size, sizeof(long long), compare_leafs);
	for (i = 0; i < size; i++)
		free(leafs[i]);
	free(leafs);
}

int main(int argc, char* argv[])
{
	long long *stat;
	stat = malloc(256*sizeof(long long));

	/*if (argc != 2)
	{
		printf("wrong arguments\n");
		return 1;
	}*/

	if (get_stat(stat, "gulliver.txt") == 1)
	{
		free(stat);
		exit(1);
	}
	int len = 0;
	for (int i = 255; i >= 0; i--)
		if (stat[i] != 0)
			len ++;
	printf("%d\n", len);
	make_tree(stat);
	free(stat);
	return 0;
}
