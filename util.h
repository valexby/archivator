/**
 * @file util.h
 * @author Alex Vasiuk
 * @brief Header with declared functions that implement Huffman compression algorithm
 */
#include <stdbool.h>
#include <stdio.h>

#define TOKEN_SZ 300*1024*1024
#define SET_BIT(dest, offset) ((dest) | (1 << (offset)))
#define GET_BIT(dest, offset) (((dest) >> (offset)) & 1)

/**
 * @brief Implementation of Huffman binary three's node.
 *
 * Node of Huffman's three. Has two pointers on its childs; symbol, wich will be
 * coded by that node, if that node is leaf; and weight of node.
 */
typedef struct node
{
	node *right; /**< Pointers on right child of that node. */
	node *left; /**< Pointers on left child of that node. */
	unsigned char symbol; /**< Symbol, wich will coded by current brunch of three. 
				For intermediate node this param don't have any sence. */
	long long weight; /** Weight of current node. */
} node;

int get_stat(long long *, FILE*);
int array_sum(long long *, int);
int compare_leafs(const void *, const void *);

node* make_tree(const long long *);
void find_minimal_nodes(int, node **);
void put_huff_code(const bool*, int, unsigned char*, int*, int*, FILE*);
void compress(node*, FILE*, FILE*, long long*);
void decompress(node*, FILE*, FILE*);
void decode_byte(unsigned char, node*, node**, unsigned char*, int*, FILE*);

void free_tree(node*);
void get_huffman_codes_wrapped(node *, bool*[256], int[256], bool[256], int);
void get_huffman_codes(node *, bool* [256], int[256]);
void save_stat(const long long *, FILE*);
void load_stat(long long *, FILE*);
int commpress(const char*, const char*);

