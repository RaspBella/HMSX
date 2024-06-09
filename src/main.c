#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <solve.h>

int fd;
struct stat sb;
char *bytes;
solve *solves;

int main(int argc, char **argv)
{
    if (argc != 3) {
	fprintf(stderr, "usage: %s `session.csv` `target time to beat`\n", *argv);
	exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDONLY, S_IRUSR | S_IWUSR);

    if (fstat(fd, &sb) == -1) {
	fprintf(stderr, "file: %s not found\n", argv[1]);
	close(fd);
	exit(EXIT_FAILURE);
    }

    bytes = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    solves = new_solves_from_bytes(bytes, sb.st_size);

    

    free_solves(solves);

    munmap(bytes, sb.st_size);

    close(fd);
}
