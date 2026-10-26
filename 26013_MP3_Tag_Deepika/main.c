#include "mp3_tag_reader.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("ERROR: ./a.out : INVALID ARGUMENTS\n");
        print_help();
        return FAILURE;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
    {
        print_help();
        return SUCCESS;
    }

    if (strcmp(argv[1], "-v") == 0)
    {
        if (argc != 3)
        {
            printf("ERROR: Invalid arguments for viewing.\n");
            printf("Usage: ./a.out -v <mp3_filename>\n");
            return FAILURE;
        }
        return view_mp3_tags(argv[2]);
    }
    else if (strcmp(argv[1], "-e") == 0)
    {
        if (argc < 5)
        {
            printf("ERROR: Invalid arguments for editing.\n");
            printf("Usage: ./a.out -e <-t/-a/-A/-y/-c> \"new_text\" <mp3_filename>\n");
            return FAILURE;
        }
        return edit_mp3_tags(argv, argv[argc - 1]);
    }
    else
    {
        printf("ERROR: Invalid option '%s'\n", argv[1]);
        print_help();
        return FAILURE;
    }

    return SUCCESS;
}

void print_help(void)
{
    printf("-----------------------HELP MENU-----------------------\n");
    printf("1. -v  -> to view mp3 file contents[cite: 61]\n");
    printf("2. -e  -> to edit mp3 file contents[cite: 61]\n");
    printf("       2.1. -t -> to edit song title[cite: 61]\n");
    printf("       2.2. -a -> to edit artist name[cite: 61]\n");
    printf("       2.3. -A -> to edit album name[cite: 61]\n");
    printf("       2.4. -y -> to edit year[cite: 61]\n");
    printf("       2.5. -c -> to edit comment[cite: 61]\n");
    printf("3. --help -> display this help info\n");
    printf("-------------------------------------------------------\n");
}