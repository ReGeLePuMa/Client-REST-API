#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

class Buffer
{
public:
    char *data;
    size_t size;
    Buffer()
    {
        this->data = NULL;
        this->size = 0;
    }
    int buffer_is_empty()
    {
        return (this->data == NULL);
    }
    void buffer_add(const char *data, size_t data_size)
    {
        if (this->data != NULL)
        {
            this->data = (char *)realloc(this->data, (this->size + data_size) * sizeof(char));
        }
        else
        {
            this->data = (char *)calloc(data_size, sizeof(char));
        }

        memcpy(this->data + this->size, data, data_size);

        this->size += data_size;
    }
    int buffer_find(const char *data, size_t data_size)
    {
        if (data_size > this->size)
            return -1;

        size_t last_pos = this->size - data_size + 1;

        for (size_t i = 0; i < last_pos; ++i)
        {
            size_t j;

            for (j = 0; j < data_size; ++j)
            {
                if (this->data[i + j] != data[j])
                {
                    break;
                }
            }

            if (j == data_size)
                return i;
        }

        return -1;
    }
    int buffer_find_insensitive(const char *data, size_t data_size)
    {
        if (data_size > this->size)
            return -1;

        size_t last_pos = this->size - data_size + 1;

        for (size_t i = 0; i < last_pos; ++i)
        {
            size_t j;

            for (j = 0; j < data_size; ++j)
            {
                if (tolower(this->data[i + j]) != tolower(data[j]))
                {
                    break;
                }
            }

            if (j == data_size)
                return i;
        }

        return -1;
    }
    ~Buffer()
    {
        if (this->data != NULL)
        {
            free(this->data);
        }
    }
};

#endif