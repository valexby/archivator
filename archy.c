#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>

#define token 1024*1024*300

int main(int argc, char* argv[])
{
	int fd, i, j;
	char buf[token];
	size_t token_sz = token;
	long long stat[256];

	memset(stat, 0, sizeof(stat));

	if (argc != 2) 
	{
		printf("wrong arguments\n");
		return 1;
	}

	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
	{
		perror("archy");
		return 1;
	}
	while ((i = read(fd, buf, token_sz)) && i >= 0)
	{
		for (j = 0 ; j < i ; j++)
			stat[(int)buf[j]]++;
	}

	perror("archy");

	close(fd);

	printf("%d %d %d\n", stat['h'], stat['H'], stat['l']);
}
