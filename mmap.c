#include <sys/mman.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Pass source as first argument and destination as second\n");
        return 0;
    }

    /* open files */
    int file_in = open(argv[1], O_RDONLY, 0);
    if (file_in == -1) {
        printf("Cannot open source file\n");
        return 1;
    }

    int file_out = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, __S_IWRITE | __S_IREAD);
    if (file_out == -1) {
        printf("Cannot create output file\n");
        return 1;
    }

    /* get input size */
    struct stat in_stat;
    fstat(file_in, &in_stat);

    /* set output size */
    lseek(file_out, in_stat.st_size - 1, SEEK_SET);
    write(file_out, "", 1);

    void *src = mmap(0, in_stat.st_size, PROT_READ, MAP_SHARED, file_in, 0);
    void *dst = mmap(0, in_stat.st_size, PROT_WRITE, MAP_SHARED, file_out, 0);

    memcpy(dst, src, in_stat.st_size);
    return 0;
}
