/*
 * Copyright (c) 2015 by Farsight Security, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>

uint8_t *load_container(char *filename, ssize_t *buf_size)
{
    int fd, n;
    uint8_t *buf = NULL;
    struct stat stat_buf;

    fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "can't open %s\n", filename);
        return NULL;
    }
    if (fstat(fd, &stat_buf) == -1)
    {
        fprintf(stderr, "can't stat %s\n", filename);
        close(fd);
        return NULL;
    }
    buf = malloc(stat_buf.st_size);
    if (buf == NULL)
    {
        fprintf(stderr, "can't malloc\n");
        close(fd);
        return NULL;
    }
    n = read(fd, buf, stat_buf.st_size);
    if (n != stat_buf.st_size)
    {
        fprintf(stderr, "can't read %s\n", filename);
        close(fd);
        free(buf);
        return NULL;
    }
    close(fd);

    *buf_size = n;
    return buf;
}
