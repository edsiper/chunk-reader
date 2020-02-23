/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <stdio.h>
#include <stdlib.h>
#include "mpack.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define FLB_FALSE 0
#define FLB_TRUE  1

int mp_count(void *data, size_t bytes)
{
    int count = 0;
    mpack_reader_t reader;

    mpack_reader_init_data(&reader, (const char *) data, bytes);
    while (mpack_reader_remaining(&reader, NULL) > 0) {
        count++;
        mpack_discard(&reader);
    }

    mpack_reader_destroy(&reader);
    return count;
}

/*
 * Validate if every item in the 'data' buffer has the following format:
 *
 * array (2 items)
 *   [0] =>  extension fixext8
 *   [1] =>  map
 */
int mp_is_valid_chunk(void *data, size_t bytes)
{
    int c = 0;
    mpack_tag_t array;
    mpack_tag_t timestamp;
    mpack_tag_t map;
    mpack_reader_t reader;

    mpack_reader_init_data(&reader, (const char *) data, bytes);
    while (mpack_reader_remaining(&reader, NULL) > 0) {

        array = mpack_read_tag(&reader);
        if (mpack_reader_error(&reader) != mpack_ok) {
            printf("error: ARRAY MATCH\n");
            goto error;
        }

        timestamp = mpack_read_tag(&reader);
        mpack_done_ext(&reader);

        map = mpack_read_tag(&reader);
        mpack_done_map(&reader);

        mpack_done_array(&reader);
        c++;
    }

    printf("processed records => %i\n", c);
    mpack_reader_destroy(&reader);
    return FLB_TRUE;

 error:
    printf("error: processed records: %i\n", c);
    mpack_reader_destroy(&reader);
    return FLB_FALSE;
}

int main(int argc, char **argv)
{
    int ret;
    char *file;
    char *buf;
    size_t size;
    size_t bytes;
    FILE *f;
    struct stat st;

    if (argc < 2) {
        fprintf(stderr, "usage: ./reader CHUNK_FILE\n");
        exit(EXIT_FAILURE);
    }

    ret = stat(argv[1], &st);
    if (ret == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    size = st.st_size;

    f = fopen(argv[1], "rb");
    if (!f) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    buf = malloc(size);
    if (!buf) {
        perror("malloc");
        fclose(f);
        exit(EXIT_FAILURE);
    }

    bytes = fread(buf, 1, size, f);
    fclose(f);

    /* Count number of items */
    ret = mp_count(buf, size);
    printf("- number of items: %i\n", ret);

    /* Check if the items have the expected structure */
    ret = mp_is_valid_chunk(buf, size);
    printf("- is valid       : %s\n", ret ? "Yes" : "No");
    printf("\n\n");
    free(buf);
    return 0;
}
