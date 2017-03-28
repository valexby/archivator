#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>

#define TOKEN_SZ 300*1024*1024
#define SET_BIT(dest, offset) ((dest) | (1 << offset))

typedef struct node node;

struct node
{
	node *right, *left;
	unsigned char symbol;
	long long weight;
};

int get_stat(long long *, FILE*);
int array_sum(long long *, int);
int compare_leafs(const void *, const void *);

node* make_tree(const long long *);
void find_minimal_nodes(int, node **);
void print_tree(node*, int);
void free_tree(node*);
void get_huffman_codes_wrapped(node *, bool*[256], int[256], bool[256], int);
void get_huffman_codes(node *, bool* [256], int[256]);
void save_stat(const long long *, FILE*);
void load_stat(long long *, FILE*);

int get_stat(long long *stat, FILE *fd)
{
	int i, j;
	unsigned char *buf = malloc(TOKEN_SZ*sizeof(unsigned char));
	memset(stat, 0, sizeof(long long)*256);

	while ((i = fread(buf, 1, TOKEN_SZ, fd)) && i > 0)
	{
		for (j = 0 ; j < i ; j++)
		{
			stat[(size_t)buf[j]]++;
		}
	}

	free(buf);
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

void save_stat(const long long *stat, FILE *fd)
{
	int i;
	unsigned char size = 0;
	
	for (i = 0; i < 256; i++)
		if (stat[i] != 0) size++;

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
	
}

void load_stat(long long *stat, FILE* fd)
{
	int i;
	unsigned char size = 0, buf;

	memset(stat, 0, sizeof(long long) * 256);

	fread(&size, sizeof(unsigned char), 1, fd);
	for (i = size; i > 0; i--)
	{
		fread(&buf, sizeof(unsigned char), 1, fd);
		fread(&(stat[buf]), sizeof(long long), 1, fd);
	}
}


void put_huff_code(const bool* code, int size, unsigned char* buf, int* indx_out, int* offset, FILE* fo)
{
	int indx_code;
	for (indx_code = 0; indx_code < size; indx_code++)
	{
		if (code[indx_code]) 
			buf[*indx_out] = SET_BIT(buf[*indx_out], *offset);
		(*offset)++;
		if (*offset == 8) 
		{
			*offset = 0;
			(*indx_out)++;
			if (*indx_out >= TOKEN_SZ)
			{
				if (fwrite(buf, sizeof(char), *indx_out, fo) != *indx_out)
					perror("put_huff_code");
				*indx_out = 0;
			}
		}
	}
}

void make_archive(node* root, FILE* fi, FILE* fo)
{
	int sizes[256] = {0};
       	bool* codes[256] = {0};
	unsigned char *buf_in, *buf_out, sym;
	int i, indx_in = 0, indx_out = 0, offset = 0;

	get_huffman_codes(root, codes, sizes);

	buf_in = malloc(TOKEN_SZ*sizeof(unsigned char));
	buf_out = malloc(TOKEN_SZ*sizeof(unsigned char));

	while ((i = fread(buf_in, 1, TOKEN_SZ, fi)) && i > 0)
	{
		for (indx_in = 0; indx_in < i; indx_in++)
		{
			sym = buf_in[indx_in];
			put_huff_code(codes[sym], sizes[sym], buf_out, &indx_out, &offset, fo);		
		}
	}
	if (fwrite(buf_out, sizeof(char), indx_out, fo) != indx_out)
		perror("make_arch_write");

	for (int i = 0; i < 256; i++)
		free(codes[i]);
	free(buf_in);
	free(buf_out);
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

void get_huffman_codes(node *root, bool* codes[256], int sizes[256])
{
	bool buff[256];
	get_huffman_codes_wrapped(root, codes, sizes, buff, 0);
}

void get_huffman_codes_wrapped(node *root, bool* codes[256], int sizes[256], bool buff[256], int size)
{
	if ((*root).left == NULL && (*root).right == NULL)
	{
		unsigned char index = (*root).symbol;
		codes[index] = malloc(sizeof(unsigned char) * size);
		memcpy(codes[index], buff, size);
		sizes[index] = size;
		return;
	}
	if ((*root).left != NULL) 
	{
		buff[size] = true;
		get_huffman_codes_wrapped((*root).left, codes, sizes, buff, size + 1);
	}
	if ((*root).right != NULL)
	{
		buff[size] = false;
		get_huffman_codes_wrapped((*root).right, codes, sizes, buff, size + 1);
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
	long long *stat;
	node *root;
	FILE *fi, *fo;
	stat = malloc(256*sizeof(long long));

	fi = fopen("test.txt", "rb");
	if (fi == NULL) { perror("main"); exit(1);}
	fo = fopen("test.out", "wb");
	if (fo == NULL) { perror("main"); exit(1);}

	get_stat(stat, fi);
	root = make_tree(stat);
	save_stat(stat, fo);
	if (fclose(fi) == EOF) {perror("main input close");}
	fi = fopen("test.txt", "rb");
	make_archive(root, fi, fo);

	if (fclose(fi) == EOF) {perror("main input close");}
	if (fclose(fo) == EOF) {perror("main output close");}
	free_tree(root);
	free(stat);
	return 0;
}
