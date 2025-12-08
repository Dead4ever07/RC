/**
 * Implementation Cookbook:
 *      - Url parser and conversion to a struct
 *      - Server ip resolver
 *      - Connection establishment (Includes socket creation and connection)
 */

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/time.h>

#define BUFF_SIZE 512
#define FTP_PORT 21

typedef struct
{
    char username[BUFF_SIZE];
    char password[BUFF_SIZE];
    char address[BUFF_SIZE];
    char path[BUFF_SIZE];
    char ip[BUFF_SIZE];
} URL_struct;

int ftpUrlParser(const char *url, URL_struct *out)
{
    if (strlen(url) > BUFF_SIZE)
    {
        fprintf(stderr, "The URL provided is too big.\n");
        return -1;
    }
    if (strncmp(url, "ftp://", 6) != 0)
    {
        fprintf(stderr, "The URL does not contain ftp://\n");
        return -1;
    }

    const char *ptr = url + 6;
    const char *at = strchr(ptr, '@');
    const char *slash;

    if (at)
    {
        const char *colon = strchr(ptr, ':');
        if (!colon || colon > at)
        {
            fprintf(stderr, "The URL doesn't match the expected format\n");
            return -1;
        }

        size_t ulen = colon - ptr;
        if (ulen >= BUFF_SIZE)
        {
            fprintf(stderr, "Username too long\n");
            return -1;
        }
        strncpy(out->username, ptr, ulen);
        out->username[ulen] = '\0';

        size_t plen = at - (colon + 1);
        if (plen >= BUFF_SIZE)
        {
            fprintf(stderr, "Password too long\n");
            return -1;
        }
        strncpy(out->password, colon + 1, plen);
        out->password[plen] = '\0';

        ptr = at + 1;
    }
    else
    {
        strcpy(out->username, "anonymous");
        strcpy(out->password, "anonymous");
    }

    slash = strchr(ptr, '/');
    if (!slash)
    {
        fprintf(stderr, "The URL doesn't match the expected format\n");
        return -1;
    }

    size_t hlen = slash - ptr;
    if (hlen >= BUFF_SIZE)
    {
        fprintf(stderr, "Host too long\n");
        return -1;
    }
    strncpy(out->address, ptr, hlen);
    out->address[hlen] = '\0';

    strncpy(out->path, slash, BUFF_SIZE - 1);
    out->path[BUFF_SIZE - 1] = '\0';

    return 0;
}

int ipAddressResolver(URL_struct *out)
{
    if (!out)
    {
        fprintf(stderr, "The provided struct was NULL\n");
        return -1;
    }

    struct hostent *h;

    if ((h = gethostbyname(out->address)) == NULL)
    {
        herror("gethostbyname()");
        return -1;
    }

    strcpy(out->ip, inet_ntoa(*((struct in_addr *)h->h_addr_list[0])));

    return 0;
}

int connectionCreation(URL_struct *url_struct)
{
    int sockfd;
    struct sockaddr_in server_addr;

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(url_struct->ip);
    server_addr.sin_port = htons(FTP_PORT);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket()");
        return -1;
    }

    

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect()");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

/**
 * Reads FTP reply (handles multi-line responses).
 * Returns numeric status code (e.g. 220, 230, 227) or -1 on error.
 */
int ftpRead(int sockfd, char *buffer)
{
    char temp[BUFF_SIZE];
    int total_len = 0;

    buffer[0] = '\0';

    while (1)
    {
        bzero(temp, BUFF_SIZE);

        int n = read(sockfd, temp, BUFF_SIZE - 1);
        if (n <= 0)
        {
            perror("read()");
            return -1;
        }

        if (total_len + n >= BUFF_SIZE)
        {
            fprintf(stderr, "FTP reply too long\n");
            return -1;
        }
        strncat(buffer, temp, n);
        total_len += n;

        printf("< %s", temp);

        char *last_line = temp;
        char *p = temp;
        while (*p)
        {
            if (*p == '\n' && *(p + 1) != '\0')
            {
                last_line = p + 1;
            }
            p++;
        }

        if (strlen(last_line) >= 4 &&
            isdigit((unsigned char)last_line[0]) &&
            isdigit((unsigned char)last_line[1]) &&
            isdigit((unsigned char)last_line[2]) &&
            last_line[3] == ' ')
        {

            return atoi(last_line);
        }
    }
}

int ftpSend(int sockfd, const char *cmd)
{
    printf("> %s", cmd);
    int n = write(sockfd, cmd, strlen(cmd));
    if (n <= 0)
    {
        perror("write()");
        return -1;
    }
    return 0;
}

int ftpLogin(int sockfd, URL_struct *url)
{
    char buffer[BUFF_SIZE];

    if (ftpRead(sockfd, buffer) != 220)
    {
        fprintf(stderr, "FTP server not ready\n");
        return -1;
    }

    char cmd[BUFF_SIZE * 2];
    sprintf(cmd, "USER %s\r\n", url->username);
    if (ftpSend(sockfd, cmd) < 0)
        return -1;

    int code = ftpRead(sockfd, buffer);
    if (code != 331 && code != 230)
    {
        fprintf(stderr, "Invalid USER response\n");
        return -1;
    }

    if (code == 331)
    {
        sprintf(cmd, "PASS %s\r\n", url->password);
        if (ftpSend(sockfd, cmd) < 0)
            return -1;

        if (ftpRead(sockfd, buffer) != 230)
        {
            fprintf(stderr, "Login failed\n");
            return -1;
        }
    }

    sprintf(cmd, "TYPE I\r\n");
    if (ftpSend(sockfd, cmd) < 0)
        return -1;
    if (ftpRead(sockfd, buffer) != 200)
    {
        fprintf(stderr, "Failed to set binary mode\n");
        return -1;
    }

    return 0;
}

int enterPassiveMode(int sockfd, char *ip, int *port)
{
    char buffer[BUFF_SIZE];

    if (ftpSend(sockfd, "PASV\r\n") < 0)
        return -1;

    int code = ftpRead(sockfd, buffer);
    if (code != 227)
    {
        fprintf(stderr, "PASV failed (code %d)\n", code);
        fprintf(stderr, "Raw response:\n%s\n", buffer);
        return -1;
    }

    char *p = strchr(buffer, '(');
    if (!p)
    {
        fprintf(stderr, "PASV parse error: no '('\n");
        return -1;
    }

    int h1, h2, h3, h4, p1, p2;

    if (sscanf(p + 1, "%d,%d,%d,%d,%d,%d",
               &h1, &h2, &h3, &h4, &p1, &p2) != 6)
    {
        fprintf(stderr, "PASV parse error: malformed numbers\n");
        return -1;
    }

    sprintf(ip, "%d.%d.%d.%d", h1, h2, h3, h4);
    *port = p1 * 256 + p2;

    return 0;
}

const char *extractFileName(const char *path)
{
    const char *slash = strrchr(path, '/');

    if (!slash || *(slash + 1) == '\0')
    {
        // No slash or ends with slash → fallback name
        return "download.bin";
    }

    return slash + 1;
}

int openDataConnection(const char *ip, int port)
{
    int sockfd;
    struct sockaddr_in addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("data socket()");
        return -1;
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("data connect()");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int ftpGetFileSize(int sockfd, const char *path)
{
    char cmd[BUFF_SIZE];
    char buffer[BUFF_SIZE];

    sprintf(cmd, "SIZE %s\r\n", path);
    ftpSend(sockfd, cmd);

    int code = ftpRead(sockfd, buffer);
    if (code != 213)
    {
        fprintf(stderr, "SIZE command failed (code %d)\n", code);
        return 0;
    }

    int size = 0;
    sscanf(buffer, "%*d %d", &size);
    return size;
}

int ftpDownload(int controlSock, int dataSock, const char *path)
{
    char buffer[BUFF_SIZE];
    char cmd[BUFF_SIZE];

    int totalSize = ftpGetFileSize(controlSock, path);

    sprintf(cmd, "RETR %s\r\n", path);
    ftpSend(controlSock, cmd);

    int code = ftpRead(controlSock, buffer);
    if (code != 150 && code != 125)
    {
        fprintf(stderr, "RETR failed\n");
        return -1;
    }

    const char *filename = extractFileName(path);


    FILE *file = fopen(filename, "wb");

    if (!file)
    {
        perror("fopen()");
        return -1;
    }

    int n;
    int downloaded = 0;
    int lastPercent = -1;

    printf("\n");

    while ((n = read(dataSock, buffer, BUFF_SIZE)) > 0)
    {
        fwrite(buffer, 1, n, file);
        downloaded += n;

        if (totalSize > 0)
        {
            int percent = (downloaded * 100) / totalSize;

            if (percent != lastPercent)
            {
                lastPercent = percent;

                int barWidth = 40;
                int pos = (percent * barWidth) / 100;

                printf("\r[");
                for (int i = 0; i < barWidth; i++)
                {
                    if (i < pos)
                        printf("#");
                    else
                        printf(" ");
                }

                printf("] %3d%%   %d / %d MB",
                       percent,
                       downloaded / (1024 * 1024),
                       totalSize / (1024 * 1024));
                fflush(stdout);
            }
        }
    }

    printf("\nDownload complete\n");

    fclose(file);
    close(dataSock);

    ftpRead(controlSock, buffer);
    return 0;
}

void ftpQuit(int sockfd)
{
    ftpSend(sockfd, "QUIT\r\n");
    char buffer[BUFF_SIZE];
    ftpRead(sockfd, buffer);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s ftp://[user:pass@]host/path/to/file\n", argv[0]);
        return 1;
    }

    URL_struct url_struct;

    if (ftpUrlParser(argv[1], &url_struct) != 0)
        return 1;

    if (ipAddressResolver(&url_struct) != 0)
        return 1;

    printf("User name : %s\n", url_struct.username);
    printf("Password  : %s\n", url_struct.password);
    printf("Address   : %s\n", url_struct.address);
    printf("Path      : %s\n", url_struct.path);
    printf("IP        : %s\n", url_struct.ip);

    int sockfd = connectionCreation(&url_struct);
    if (sockfd < 0)
        return 1;

    if (ftpLogin(sockfd, &url_struct) != 0)
    {
        close(sockfd);
        return 1;
    }

    char dataIP[64];
    int dataPort;

    if (enterPassiveMode(sockfd, dataIP, &dataPort) != 0)
    {
        close(sockfd);
        return 1;
    }

    int dataSock = openDataConnection(dataIP, dataPort);
    if (dataSock < 0)
    {
        close(sockfd);
        return 1;
    }

    if (ftpDownload(sockfd, dataSock, url_struct.path) != 0)
    {
        close(sockfd);
        return 1;
    }

    ftpQuit(sockfd);
    close(sockfd);
    return 1;
}
