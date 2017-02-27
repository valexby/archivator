#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

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
