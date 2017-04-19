#include <stdbool.h>
#include <stdio.h>

typedef struct node node;

int get_stat(long long *, FILE*);
int array_sum(long long *, int);
int compare_leafs(const void *, const void *);

node* make_tree(const long long *);
void find_minimal_nodes(int, node **);
void put_huff_code(const bool*, int, unsigned char*, int*, int*, FILE*);
void compress(node*, FILE*, FILE*, long long*);
void decompress(node*, FILE*, FILE*);
void free_tree(node*);
void get_huffman_codes_wrapped(node *, bool*[256], int[256], bool[256], int);
void get_huffman_codes(node *, bool* [256], int[256]);
void save_stat(const long long *, FILE*);
void load_stat(long long *, FILE*);
int commpress(const char*, const char*);

