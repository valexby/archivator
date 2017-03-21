#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>

#define TOKEN_SZ 300*1024*1024

typedef struct node node;

struct node
{
	node *right, *left;
	unsigned char symbol;
	long long weight;
};

int get_stat(long long *, const char[]);
int array_sum(long long *, int);
int compare_leafs(const void *, const void *);

node* make_tree(const long long *);
void find_minimal_nodes(int, node **);
void print_tree(node*, int);
void free_tree(node*);
void get_huffman_codes(node *, unsigned char[256], int[256], unsigned char, int);
void save_stat(const long long *, const char[]);

int get_stat(long long *stat, const char file_name[])
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
	node* a = *((node**)A);
	node* b = *((node**)B);
	if ((*a).weight > (*b).weight) return 1;
	return -1;
}

void find_minimal_nodes(int size, node **nodes)
{
	unsigned char min;
	int mins[] = {-1, -1}, j, i;
	
	for (i = 0; i < 2; i++)
	{
		min = mins[0] == 0 ? (*(nodes[1])).weight : (*(nodes[0])).weight;
		mins[i] = mins[0] == 0 ? 1 : 0 ;
		for (j = 0; j < size; j++)
		{
			if ((*(nodes[j])).weight < min && j != mins[0]) // checks we don't get two similar minimals
			{
				min = (*(nodes[j])).weight;
				mins[i] = j;
			}
		}
	}
	for (i = 0; i < 2; i++)
	{
		node *temp = nodes[mins[i]];
		nodes[mins[i]] = nodes[size - (i + 1)];
		nodes[size - (i + 1)] = temp;
	}
}

void save_stat(const long long *stat, const char file_name[])
{
	int i;
	unsigned char size = 0;
	FILE *fd;
	
	for (i = 0; i < 256; i++)
		if (stat[i] != 0) size++;

	fd = fopen(file_name, "w");
	if (fd == NULL)
	{
		perror("save_stat");
		return;
	}

	fwrite(&size, sizeof(unsigned char), 1, fd);
	for (i = 0; i < 256; i++)
	{
		if (stat[i] != 0)
		{
			size = i;
			fwrite(&size, sizeof(unsigned char), 1, fd);
			fwrite(&(stat[i]), sizeof(long long), 1, fd);
		}
	}
	fclose(fd);
	
}

void load_stat(long long *stat, const char file_name[])
{
	int i;
	unsigned char size = 0, buf;
	FILE *fd;

	memset(stat, 0, sizeof(long long) * 256);
	
	fd = fopen(file_name, "rb");
	if (fd == NULL)
	{
		perror("save_stat");
		return;
	}

	fread(&size, sizeof(unsigned char), 1, fd);
	for (i = size; i > 0; i--)
	{
		fread(&buf, sizeof(unsigned char), 1, fd);
		fread(&(stat[buf]), sizeof(long long), 1, fd);
	}
	fclose(fd);
}

node* make_tree(const long long *stat)
{
	unsigned char i, size = 0;
	node **leafs, *nooby;
	leafs = malloc(256 * sizeof(node*));
	i = 255;
	
	do
	{
		if (stat[i] == 0) 
		{
			i--;
			continue;
		}
		leafs[size] = malloc(sizeof(node));
		(*(leafs[size])).right = NULL;
		(*(leafs[size])).left = NULL;
		(*(leafs[size])).symbol = i;
		(*(leafs[size])).weight = stat[i];
		size++;
		i--;
	}
	while (i != 255);

	while (size != 1)
	{
		find_minimal_nodes(size, leafs);
		nooby = malloc(sizeof(node));
		(*nooby).right = leafs[size - 2];
		(*nooby).left = leafs[size - 1];
		(*nooby).symbol = 255;
		(*nooby).weight = (*(leafs[size - 1])).weight + (*(leafs[size - 2])).weight;
		leafs[size - 1] = NULL;
		leafs[size - 2] = nooby;
		size--;
	}
	node *root = leafs[0];
	free(leafs);
	return root; 
	/*for (i = 0; i < size; i++)
		free(leafs[i]);
	free(leafs);*/
}

void print_tree(node *root, int layer)
{
	int i;
	if ((*root).left != NULL) print_tree((*root).left, layer + 1);
	for (i = layer; i >= 0; i--)
	{
		printf("   ");
	}
	printf("%lld\n\n", (*root).weight);
	if ((*root).right != NULL) print_tree((*root).right, layer + 1);
}

void get_huffman_codes(node *root, unsigned char codes[256], int sizes[256], unsigned char code, int size)
{
	if ((*root).left != NULL) get_huffman_codes((*root).left, codes, sizes, code, size + 1);
	if ((*root).right != NULL) get_huffman_codes((*root).right, codes, sizes, code | (1 << size), size + 1);
	if ((*root).left == NULL && (*root).right == NULL)
	{
		codes[(*root).symbol] = code;
		sizes[(*root).symbol] = size;
	}
}

void free_tree(node *root)
{
	if ((*root).left != NULL) free_tree((*root).left);
	if ((*root).right != NULL) free_tree((*root).right);
	free(root);
}

int main(int argc, char* argv[])
{
	int sizes[256] = {0};
       	unsigned char codes[256] = {0};
	long long *stat, *stat2;
	node *root;
	stat = malloc(256*sizeof(long long));
	stat2 = malloc(256*sizeof(long long));

	if (get_stat(stat, "gulliver.txt") == 1)
	{
		free(stat);
		exit(1);
	}
	root = make_tree(stat);
	//print_tree(root, 0);
	get_huffman_codes(root, codes, sizes, 0, 0);
	free_tree(root);
	save_stat(stat, "test.out");
	load_stat(stat2, "test.out");
	for (int i = 0; i < 256; i++)
		if (stat[i] != stat2[i]) printf("%d failed!\n", i);
	free(stat);
	free(stat2);
	return 0;
}
