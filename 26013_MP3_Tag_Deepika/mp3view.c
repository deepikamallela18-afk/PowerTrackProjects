#include "mp3_tag_reader.h"

// Helper function to decode big-endian 4-byte frame size
int decode_size(const unsigned char *buf)
{
    return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

int view_mp3_tags(char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("ERROR: Unable to open file %s\n", filename);
        return FAILURE;
    }

    // 1. Check ID3 header identifier
    char header_id[4];
    fread(header_id, 1, 3, fp);
    header_id[3] = '\0';

    if (strcmp(header_id, "ID3") != 0)
    {
        printf("ERROR: Not a valid ID3v2 file.\n");
        fclose(fp);
        return FAILURE;
    }

    // 2. Check version bytes (03 00 for ID3v2.3)
    unsigned char version[2];
    fread(version, 1, 2, fp);
    if (version[0] != 0x03 || version[1] != 0x00)
    {
        printf("ERROR: Unsupported ID3 version (%d.%d). Only v2.3 is supported.\n", version[0], version[1]);
        fclose(fp);
        return FAILURE;
    }

    // Skip flags (1 byte) and 4-byte tag size descriptor
    fseek(fp, 5, SEEK_CUR);

    printf("\n------------------- SELECTED VIEW DETAILS -------------------\n");
    printf("MP3 TAG READER AND EDITOR FOR ID3v2[cite: 62]\n");
    printf("-------------------------------------------------------------\n");

    char title[100] = "N/A";
    char artist[100] = "N/A";
    char album[100] = "N/A";
    char year[10] = "N/A";
    char comment[100] = "N/A";

    unsigned char frame_header[10];
    while (fread(frame_header, 1, 10, fp) == 10)
    {
        if (frame_header[0] == 0x00)
            break; // Reached padding

        char frame_id[5];
        memcpy(frame_id, frame_header, 4);
        frame_id[4] = '\0';

        int frame_size = decode_size(&frame_header[4]);
        if (frame_size <= 0)
            break;

        char *content = malloc(frame_size + 1);
        if (!content)
            break;

        fread(content, 1, frame_size, fp);
        content[frame_size] = '\0';

        // Skip 1-byte text encoding descriptor
        char *actual_text = content + 1;

        if (strcmp(frame_id, "TIT2") == 0)
        {
            strncpy(title, actual_text, sizeof(title) - 1);
        }
        else if (strcmp(frame_id, "TPE1") == 0)
        {
            strncpy(artist, actual_text, sizeof(artist) - 1);
        }
        else if (strcmp(frame_id, "TALB") == 0)
        {
            strncpy(album, actual_text, sizeof(album) - 1);
        }
        else if (strcmp(frame_id, "TYER") == 0)
        {
            strncpy(year, actual_text, sizeof(year) - 1);
        }
        else if (strcmp(frame_id, "COMM") == 0)
        {
            // COMM frame structure: Encoding(1) + Language(3) + Description + 0x00 + Comment Text
            char *comm_text = actual_text + 3; // skip language
            while (*comm_text != '\0' && comm_text < content + frame_size)
            {
                comm_text++;
            }
            if (comm_text < content + frame_size)
                comm_text++; // skip null terminator
            strncpy(comment, comm_text, sizeof(comment) - 1);
        }

        free(content);
    }

    printf("TITLE    : %s\n", title);
    printf("ARTIST   : %s\n", artist);
    printf("ALBUM    : %s\n", album);
    printf("YEAR     : %s\n", year);
    printf("COMMENT  : %s\n", comment);
    printf("-------------------------------------------------------------\n");
    printf("------------ DETAILS DISPLAYED SUCCESSFULLY[cite: 62] -------\n\n");

    fclose(fp);
    return SUCCESS;
}