#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
	void *right, *left;
	unsigned char symbol;
	bool is_symbol, separator;
	long long weight;
} root;

#define TOKEN_SZ 300*1024*1024

unsigned char buf[TOKEN_SZ];

int get_stat(long long *stat, char file_name[])
{
	int i, j;
	FILE *fd;
	memset(stat, 0, sizeof(long long)*256);

	fd = fopen(file_name, "rb");

	while ((i = fread(buf, 1, TOKEN_SZ, fd)) && i > 0)
	{
		for (j = 0 ; j < i ; j++)
		{
			stat[(size_t)buf[j]]++;
			if ((size_t)buf[j] > 255)
				printf("%d\n", (int)buf[j]);
		}
	}

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

int compare_long(const void *A, const void *B)
{
	long long a = *((long long*)A);
	long long b = *((long long*)B);
	if (a < b) return 1;
	if (a == b) return 0;
	if (a > b) return -1;
}

void make_tree(long long *stat)
{
	unsigned char i, size = 0;
	root **leafs;
	leafs = (root**)malloc(257 * sizeof(root*));
	for (i = 255; i >= 0; i--)
	{
		if (stat[i] == 0) continue;
		leafs[size] = (root*)malloc(sizeof(root));
		(*(leafs[size])).right = NULL;
		(*(leafs[size])).left = NULL;
		(*(leafs[size])).symbol = i;
		(*(leafs[size])).is_symbol = true;
		(*(leafs[size])).separator = false;
		(*(leafs[size])).weight = stat[i];
		size++;
	}
	leafs[size] = (root*)malloc(sizeof(root));
	(*(leafs[size])).right = NULL;
	(*(leafs[size])).left = NULL;
	(*(leafs[size])).symbol = size;
	(*(leafs[size])).is_symbol = true;
	(*(leafs[size])).separator = true;
	(*(leafs[size])).weight = array_sum(stat, 256);
	size++;
	qsort(stat, 256, sizeof(long long), compare_long);
}

/* typedef struct
{
	void *right, *left;
	unsigned char symbol;
	bool is_symbol, separator;
	long long weight;
}
 */
int main(int argc, char* argv[])
{
	long long *stat;
	stat = (long long*) malloc(256*sizeof(long long));

	/*if (argc != 2)
	{
		printf("wrong arguments\n");
		return 1;
	}*/

	if (get_stat(stat, "test.txt") == 1)
	{
		free(stat);
		return 1;
	}
	free(stat);
	return 0;
}
