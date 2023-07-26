#ifndef REQUESTS_HPP
#define REQUESTS_HPP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class Requests
{
public:
    static char *GET(char *host, char *url, char *query_params,
                     char *cookie, char *token, bool access)
    {
        char *message = new char[BUFLEN]();
        char *line = new char[LINELEN]();

        if (query_params != NULL)
        {
            sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
        }
        else
        {
            sprintf(line, "GET %s HTTP/1.1", url);
        }

        compute_message(message, line);
        memset(line, 0, LINELEN);
        sprintf(line, "Host: %s", host);
        compute_message(message, line);

        if (access == true)
        {
            memset(line, 0, LINELEN);
            sprintf(line, "Authorization: Bearer %s", token);
            compute_message(message, line);
        }
        if (cookie != NULL)
        {
            memset(line, 0, LINELEN);
            strcat(line, "Cookie: ");

            strcat(line, cookie);
            compute_message(message, line);
        }
        compute_message(message, "");

        delete[] line;
        return message;
    }

    static char *POST(char *host, char *url, char *content_type,
                      char *payload, int size, char *cookie, char *token,
                      bool access)
    {
        char *message = new char[BUFLEN]();
        char *line = new char[LINELEN]();

        sprintf(line, "POST %s HTTP/1.1", url);
        compute_message(message, line);

        memset(line, 0, LINELEN);
        sprintf(line, "Host: %s", host);
        compute_message(message, line);

        sprintf(line, "Content-Type: %s", content_type);
        compute_message(message, line);

        if (access == true)
        {
            memset(line, 0, LINELEN);
            sprintf(line, "Authorization: Bearer %s", token);
            compute_message(message, line);
        }

        if (cookie != NULL)
        {
            memset(line, 0, LINELEN);
            strcat(line, "Cookie: ");

            strcat(line, cookie);
            compute_message(message, line);
        }

        sprintf(line, "Content-Length: %d", size);
        compute_message(message, line);
        compute_message(message, "");

        memset(line, 0, LINELEN);
        sprintf(line, "%s", payload);
        compute_message(message, line);

        delete[] line;
        return message;
    }
    static char *DELETE(char *host, char *url, char *query_params,
                        char *cookie, char *token, bool access)
    {
        char *message = new char[BUFLEN]();
        char *line = new char[LINELEN]();

        if (query_params != NULL)
        {
            sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
        }
        else
        {
            sprintf(line, "DELETE %s HTTP/1.1", url);
        }

        compute_message(message, line);
        memset(line, 0, LINELEN);
        sprintf(line, "Host: %s", host);
        compute_message(message, line);

        if (access == true)
        {
            memset(line, 0, LINELEN);
            sprintf(line, "Authorization: Bearer %s", token);
            compute_message(message, line);
        }

        if (cookie != NULL)
        {
            memset(line, 0, LINELEN);
            strcat(line, "Cookie: ");
            strcat(line, cookie);
            compute_message(message, line);
        }

        compute_message(message, "");
        delete[] line;
        return message;
    }

private:
    static constexpr int BUFLEN = 4096;
    static constexpr int LINELEN = 1000;
    static void compute_message(char *message, const char *line)
    {
        strcat(message, line);
        strcat(message, "\r\n");
    }
};

#endif