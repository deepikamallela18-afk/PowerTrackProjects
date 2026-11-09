#include "mp3_tag_reader.h"

int edit_mp3_tags(char *argv[], char *filename)
{
    char *tag_flag = argv[2];
    char *new_val = argv[3];

    char target_frame[5];
    if (strcmp(tag_flag, "-t") == 0)
        strcpy(target_frame, "TIT2");
    else if (strcmp(tag_flag, "-a") == 0)
        strcpy(target_frame, "TPE1");
    else if (strcmp(tag_flag, "-A") == 0)
        strcpy(target_frame, "TALB");
    else if (strcmp(tag_flag, "-y") == 0)
        strcpy(target_frame, "TYER");
    else if (strcmp(tag_flag, "-c") == 0)
        strcpy(target_frame, "COMM");
    else
    {
        printf("ERROR: Invalid edit flag '%s'\n", tag_flag);
        printf("Valid flags: -t, -a, -A, -y, -c\n");
        return FAILURE;
    }

    FILE *fp = fopen(filename, "r+");
    if (fp == NULL)
    {
        printf("ERROR: Unable to open file %s in update mode.\n", filename);
        return FAILURE;
    }

    char header_id[4];
    fread(header_id, 1, 3, fp);
    header_id[3] = '\0';
    if (strcmp(header_id, "ID3") != 0)
    {
        printf("ERROR: Not a valid ID3v2 file.\n");
        fclose(fp);
        return FAILURE;
    }

    // Skip version, flags, and size descriptor (7 bytes)
    fseek(fp, 7, SEEK_CUR);

    unsigned char frame_header[10];
    int found = 0;

    while (fread(frame_header, 1, 10, fp) == 10)
    {
        if (frame_header[0] == 0x00)
            break;

        char frame_id[5];
        memcpy(frame_id, frame_header, 4);
        frame_id[4] = '\0';

        int frame_size = (frame_header[4] << 24) | (frame_header[5] << 16) | (frame_header[6] << 8) | frame_header[7];

        if (strcmp(frame_id, target_frame) == 0)
        {
            found = 1;
            int max_len = frame_size - 1; // Subtract 1 byte for text encoding descriptor

            if (strcmp(target_frame, "COMM") == 0)
            {
                fseek(fp, 4, SEEK_CUR); // Skip encoding + language
                int ch;
                while ((ch = fgetc(fp)) != EOF && ch != 0x00)
                    ; // Skip comment description string
            }
            else
            {
                fseek(fp, 1, SEEK_CUR); // Skip encoding byte
            }

            int new_len = strlen(new_val);
            int write_len = (new_len < max_len) ? new_len : max_len;

            fwrite(new_val, 1, write_len, fp);

            // Pad remaining space with null bytes if the new string is shorter
            for (int i = write_len; i < max_len; i++)
            {
                fputc(0x00, fp);
            }

            printf("SUCCESS: Successfully updated %s tag to \"%s\" in %s\n", target_frame, new_val, filename);
            break;
        }

        fseek(fp, frame_size, SEEK_CUR);
    }

    if (!found)
    {
        printf("ERROR: Frame %s not found in file %s.\n", target_frame, filename);
        fclose(fp);
        return FAILURE;
    }

    fclose(fp);
    return SUCCESS;
}