#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bfd.h>


#define die(msg, ...) \
    do { fprintf(stderr, msg, ## __VA_ARGS__); exit(1); } while (0)

#define errno_die(msg) \
    do { perror(msg); exit(1); } while(0)

#define bfd_die(msg) \
    do { bfd_perror(msg); exit(1); } while(0)


#define FD_DIR "/proc/self/fd/"
// 2^^64 is 20 digits
#define FD_FILE_MAX_LEN (sizeof(FD_DIR) + 20)


int main(int argc, char *argv[]) {
    if (argc < 2)
        die("Usage: %s <other gdb args> ... <program>\n", argv[0]);


    // Get contents of the embed-breakpoints section

    bfd *abfd = bfd_openr(argv[argc-1], NULL);
    if (!abfd)
        bfd_die("bfd_openr");

    // We must call this to load the data!
    if (!bfd_check_format(abfd, bfd_object))
        bfd_die("bfd_check_format");

    asection *sec = bfd_get_section_by_name(abfd, "embed-breakpoints");
    if (!sec)
        bfd_die("bfd_get_section_by_name");

    size_t contents_size = sec->size;
    char *contents = malloc(contents_size);
    if (!contents)
        die("Could not allocate contents\n");

    if (!bfd_get_section_contents(abfd, sec, contents, 0, contents_size))
        bfd_die("bfd_get_section_contents");

    if (!bfd_close(abfd))
        bfd_die("bfd_close");


    // Write GDB breakpoint commands to a temporary file

    FILE *temp = tmpfile();
    if (!temp)
        errno_die("tmpfile");

    for (size_t i = 0; i < contents_size; i += sizeof(void*)) {
        fprintf(temp, "break *%p\n",
            *( (void**) (contents+i) ));
    }

    rewind(temp);


    // Execute GDB with that script, along with original args

    char fd_file_path[FD_FILE_MAX_LEN];
    snprintf(fd_file_path, FD_FILE_MAX_LEN,
        "%s%d", FD_DIR, fileno(temp));

    char **gdb_args = malloc(sizeof(char*) * (argc+3));
    if (!gdb_args)
        die("Could not allocate gdb_args\n");

    gdb_args[0] = "gdb";
    gdb_args[1] = "-x";
    gdb_args[2] = fd_file_path;
    for (size_t i=1; i<argc; i++) {
        gdb_args[2+i] = argv[i];
    }
    gdb_args[2+argc] = (char*) NULL;

    execvp("gdb", gdb_args);

    // If we get here, then execvp failed.
    errno_die("execvp");
}
