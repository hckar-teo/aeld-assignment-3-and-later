#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

/*
 * Create a directory and all missing parent directories.
 * Equivalent to: mkdir -p <path>
 *
 * Returns:
 *   0  on success
 *  -1  on error
 */
static int mkdir_p(const char *path, mode_t mode)
{
    char *tmp;
    char *p;
    size_t len;

    if (path == NULL || path[0] == '\0')
    {
        errno = EINVAL;
        return -1;
    }

    tmp = strdup(path);
    if (tmp == NULL)
    {
        return -1;
    }

    /* Remove trailing '/' characters */
    len = strlen(tmp);

    while (len > 1 && tmp[len - 1] == '/')
    {
        tmp[len - 1] = '\0';
        len--;
    }

    /*
     * Walk through the path and create each directory.
     *
     * Example:
     *   /tmp/foo/bar
     *
     * Creates:
     *   /tmp
     *   /tmp/foo
     *   /tmp/foo/bar
     */
    for (p = tmp + 1; *p != '\0'; p++)
    {
        if (*p == '/')
        {
            *p = '\0';

            if (mkdir(tmp, mode) != 0)
            {
                if (errno != EEXIST)
                {
                    free(tmp);
                    return -1;
                }
            }

            *p = '/';
        }
    }

    /* Create the final directory */
    if (mkdir(tmp, mode) != 0)
    {
        if (errno != EEXIST)
        {
            free(tmp);
            return -1;
        }
    }

    free(tmp);
    return 0;
}

/*
 * Create parent directories for a file path.
 */
static int create_parent_directory(const char *filepath)
{
    char *path_copy;
    char *last_slash;
    char *parent_dir;
    int result = 0;

    path_copy = strdup(filepath);

    if (path_copy == NULL)
    {
        return -1;
    }

    last_slash = strrchr(path_copy, '/');

    /*
     * If there is no '/' in the path, the file is in the
     * current directory, so there is nothing to create.
     */
    if (last_slash == NULL)
    {
        free(path_copy);
        return 0;
    }

    *last_slash = '\0';

    /*
     * "/file.txt" -> parent directory is "/"
     */
    if (path_copy[0] == '\0')
    {
        parent_dir = "/";
    }
    else
    {
        parent_dir = path_copy;
    }

    result = mkdir_p(parent_dir, 0755);

    free(path_copy);

    return result;
}

static void print_usage(const char *program)
{
    printf("Usage: %s -f <file> -t <text>\n", program);
    printf("\n");
    printf("Options:\n");
    printf("  -f, --file <path>    File to write\n");
    printf("  -t, --text <text>    Text to write\n");
    printf("  -h, --help           Show this help message\n");
}

int main(int argc, char *argv[])
{
    const char *writefile = NULL;
    const char *writestr = NULL;
    FILE *file;

    static const struct option long_options[] =
    {
        {"file", required_argument, 0, 'f'},
        {"text", required_argument, 0, 't'},
        {"help", no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

    int option;

    /*
     * Parse command-line options.
     *
     * ":" after f/t means the option requires an argument.
     */
    while ((option = getopt_long(argc, argv, "f:t:h",
                                 long_options, NULL)) != -1)
    {
        switch (option)
        {
            case 'f':
                writefile = optarg;
                break;

            case 't':
                writestr = optarg;
                break;

            case 'h':
                print_usage(argv[0]);
                return 0;

            case '?':
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    /*
     * Both options are mandatory.
     */
    if (writefile == NULL)
    {
        fprintf(stderr, "Error: --file/-f is required\n");
        return 1;
    }

    if (writestr == NULL)
    {
        fprintf(stderr, "Error: --text/-t is required\n");
        return 1;
    }

    /*
     * Reject unexpected positional arguments.
     */
    if (optind < argc)
    {
        fprintf(stderr, "Error: unexpected argument '%s'\n", argv[optind]);
        return 1;
    }

    printf("Inputs are: writefile %s and writestr %s\n",
           writefile, writestr);

    /*
     * Create parent directory hierarchy if necessary.
     */
    if (create_parent_directory(writefile) != 0)
    {
        fprintf(stderr,
                "Error: could not create parent directory for '%s': %s\n",
                writefile,
                strerror(errno));
        return 1;
    }

    /*
     * "w" creates the file if it doesn't exist and truncates
     * existing content.
     */
    file = fopen(writefile, "w");

    if (file == NULL)
    {
        fprintf(stderr,
                "Error: could not open file '%s': %s\n",
                writefile,
                strerror(errno));
        return 1;
    }

    /*
     * Write text followed by a newline, matching the behavior
     * of the original Bash script's echo.
     */
    if (fprintf(file, "%s\n", writestr) < 0)
    {
        fprintf(stderr,
                "Error: could not write to file '%s': %s\n",
                writefile,
                strerror(errno));

        fclose(file);
        return 1;
    }

    if (fclose(file) != 0)
    {
        fprintf(stderr,
                "Error: could not close file '%s': %s\n",
                writefile,
                strerror(errno));
        return 1;
    }

    return 0;
}