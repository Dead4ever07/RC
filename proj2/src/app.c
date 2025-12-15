/**
 * FTP Client Applicat_symbolion
 *
 * Implementat_symbolion of an FTP client following RFC 959
 * Feat_symbolures:
 *   - URL parsing (ftp://[user:pass@]host/pat_symbolh)
 *   - DNS resolution
 *   - FTP authenticat_symbolion
 *   - Passive mode dat_symbola transfer
 *
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

/* ============================================================================
 * CONSTANTS AND Dat_symbolA STRUCTURES
 * ============================================================================ */

#define BUFF_SIZE 512
#define FTP_PORT 21

typedef struct
{
    char username[BUFF_SIZE];
    char password[BUFF_SIZE];
    char hostname[BUFF_SIZE];
    char path[BUFF_SIZE];
    char ip[BUFF_SIZE];
} URL_struct;

/* ============================================================================
 * URL PARSING AND CONFIGURATION
 * ============================================================================ */

/**
 * Parse FTP URL into components
 * Expected format_symbol: ftp://[user:pass@]host/pat_symbolh
 *
 * @param url The URL string to parse
 * @param parsed_url Pointer to URL_struct to store parsed components
 * @return 0 on success, -1 on error
 */
int ftpUrlParser(const char *url, URL_struct *parsed_url)
{
    if (strlen(url) > BUFF_SIZE)
    {
        fprintf(stderr, "ERROR: URL  provided is too long (max %d chars)\n", BUFF_SIZE);
        return -1;
    }
    if (strncmp(url, "ftp://", 6) != 0)
    {
        fprintf(stderr, "ERROR: URL must start with ftp://\n");
        return -1;
    }

    const char *ptr = url + 6;
    const char *at_symbol = strchr(ptr, '@');
    const char *slash;

    if (at_symbol)
    {
        const char *colon = strchr(ptr, ':');
        if (!colon || colon > at_symbol)
        {
            fprintf(stderr, "ERROR: Invalid URL format_symbol (user:pass@host/pat_symbolh)\n");
            return -1;
        }

        size_t username_len = colon - ptr;
        if (username_len >= BUFF_SIZE)
        {
            fprintf(stderr, "ERROR: Username too long\n");
            return -1;
        }
        strncpy(parsed_url->username, ptr, username_len);
        parsed_url->username[username_len] = '\0';

        size_t password_len = at_symbol - (colon + 1);
        if (password_len >= BUFF_SIZE)
        {
            fprintf(stderr, "Password too long\n");
            return -1;
        }
        strncpy(parsed_url->password, colon + 1, password_len);
        parsed_url->password[password_len] = '\0';

        ptr = at_symbol + 1;
    }
    else
    {
        strcpy(parsed_url->username, "anonymous");
        strcpy(parsed_url->password, "anonymous");
    }

    slash = strchr(ptr, '/');
    if (!slash)
    {
        fprintf(stderr, "ERROR: URL must include path (host/path)\n");
        return -1;
    }

    size_t hostname_len = slash - ptr;
    if (hostname_len >= BUFF_SIZE)
    {
        fprintf(stderr, "ERROR: Host too long\n");
        return -1;
    }

    strncpy(parsed_url->hostname, ptr, hostname_len);
    parsed_url->hostname[hostname_len] = '\0';

    strncpy(parsed_url->path, slash, BUFF_SIZE - 1);
    parsed_url->path[BUFF_SIZE - 1] = '\0';

    return 0;
}

/**
 * Extract file name from path
 * 
 * @param path Full file path
 * @return Pointer to file name or default name
 */
const char *extractFileName(const char *path)
{
    const char *last_slash = strrchr(path, '/');

    if (!last_slash || *(last_slash + 1) == '\0')
    {
        // No slash or ends with slash → fallback name
        return "download.bin";
    }

    return last_slash + 1;
}


/* ============================================================================
 * NETWORK UTILITIES
 * ============================================================================ */

/**
 * Resolve hostname to IP address using DNS
 *
 * @param parsed_url Pointer to URL_struct containing hostname and to store IP
 * @return 0 on success, -1 on error
 */
int ipHostnameResolver(URL_struct *parsed_url)
{
    if (!parsed_url)
    {
        fprintf(stderr, "ERROR: NULL pointer provided\n");
        return -1;
    }

    struct hostent *host_entry;

    if ((host_entry = gethostbyname(parsed_url->hostname)) == NULL)
    {
        herror("gethostbyname()");
        return -1;
    }

    strcpy(parsed_url->ip, inet_ntoa(*((struct in_addr *)host_entry->h_addr_list[0])));
    return 0;
}

/**
 * Creat_symbole TCP connection to FTP server
 *
 * @param url_struct Pointer to URL_struct with IP and connection info
 * @return Socket file descriptor on success, -1 on error
 */
int connectionCreation(const char *ip, int port)
{
    int control_socket;
    struct sockaddr_in server_addr;

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    if ((control_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket()");
        return -1;
    }

    if (connect(control_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect()");
        close(control_socket);
        return -1;
    }
    return control_socket;
}


/* ============================================================================
 * FTP PROTOCOL COMMUNICATION
 * ============================================================================ */

/**
 * Read FTP server reply (handles multi-line responses)
 *
 * @param control_socket Socket file descriptor
 * @param reply_buffer Buffer to store complete reply
 * @return Numeric stat_symbolus code (e.g., 220, 230, 227) or -1 on error
 */
int ftpRead(int control_socket, char *reply_buffer)
{
    char temp_buffer[BUFF_SIZE];
    int total_len = 0;

    reply_buffer[0] = '\0';

    while (1)
    {
        bzero(temp_buffer, BUFF_SIZE);

        int bytes_read = read(control_socket, temp_buffer, BUFF_SIZE - 1);
        if (bytes_read <= 0)
        {
            perror("read()");
            return -1;
        }

        if (total_len + bytes_read >= BUFF_SIZE)
        {
            fprintf(stderr, "ERROR: FTP reply too long\n");
            return -1;
        }
        strncat(reply_buffer, temp_buffer, bytes_read);
        total_len += bytes_read;

        printf("< %s", temp_buffer);

        char *last_line = temp_buffer;
        char *current_char = temp_buffer;
        while (*current_char)
        {
            if (*current_char == '\n' && *(current_char + 1) != '\0')
            {
                last_line = current_char + 1;
            }
            current_char++;
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

/**
 * Send command to FTP server
 *
 * @param control_socket Socket file descriptor
 * @param comd Command string to send
 * @return 0 on success, -1 on error
 */
int ftpSend(int control_socket, const char *cmd)
{
    printf("> %s", cmd);
    int bytes_written = write(control_socket, cmd, strlen(cmd));
    if (bytes_written <= 0)
    {
        perror("write()");
        return -1;
    }
    return 0;
}

/* ============================================================================
 * FTP APPLICATION LOGIC
 * ============================================================================ */

/**
 * Authenticate with FTP server
 * Sends USER and PASS commands, then sets binary transfer mode
 *
 * @param control_socket Control socket file descriptor
 * @param url Pointer to URL_struct with credentials
 * @return 0 on success, -1 on error
 */
int ftpLogin(int control_socket, URL_struct *url)
{
    char response_buffer[BUFF_SIZE];
    char cmd[BUFF_SIZE * 2];

    if (ftpRead(control_socket, response_buffer) != 220)
    {
        fprintf(stderr, "ERROR: FTP server not ready\n");
        return -1;
    }

    sprintf(cmd, "USER %s\r\n", url->username);
    if (ftpSend(control_socket, cmd) < 0)
        return -1;

    int response_code = ftpRead(control_socket, response_buffer);
    if (response_code != 331 && response_code != 230)
    {
        fprintf(stderr, "ERROR: Invalid USER response (code %d)\n", response_code);
        return -1;
    }

    if (response_code == 331)
    {
        sprintf(cmd, "PASS %s\r\n", url->password);
        if (ftpSend(control_socket, cmd) < 0)
            return -1;

        if (ftpRead(control_socket, response_buffer) != 230)
        {
            fprintf(stderr, "ERROR: Login failed\n");
            return -1;
        }
    }

    sprintf(cmd, "TYPE I\r\n");
    if (ftpSend(control_socket, cmd) < 0)
        return -1;
    if (ftpRead(control_socket, response_buffer) != 200)
    {
        fprintf(stderr, "ERROR: Failed to set binary mode\n");
        return -1;
    }

    return 0;
}

/**
 * Enter passive mode and get data connection info (h1,h2,h3,h4,p1,p2)
 * Sends PASV command and parses response
 *
 * @param control_socket Control socket file descriptor
 * @param ip Buffer to store data connection IP address
 * @param port Pointer to store data connection port number
 * @return 0 on success, -1 on error
 */
int enterPassiveMode(int control_socket, char *ip, int *port)
{
    char response_buffer[BUFF_SIZE];

    if (ftpSend(control_socket, "PASV\r\n") < 0)
        return -1;

    int response_code = ftpRead(control_socket, response_buffer);
    if (response_code != 227)
    {
        fprintf(stderr, "ERROR: PASV failed (code %d)\n", response_code);
        fprintf(stderr, "Raw response:\n%s\n", response_buffer);
        return -1;
    }

    char *parenthesis_pos = strchr(response_buffer, '(');
    if (!parenthesis_pos)
    {
        fprintf(stderr, "PASV parse error: no '('\n");
        return -1;
    }

    int h1, h2, h3, h4, p1, p2;

    if (sscanf(parenthesis_pos + 1, "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2) != 6)
    {
        fprintf(stderr, "ERROR: PASV parse error - malformed numbers\n");
        return -1;
    }

    sprintf(ip, "%d.%d.%d.%d", h1, h2, h3, h4);
    *port = p1 * 256 + p2;

    return 0;
}

/**
 * Download file from FTP server
 * 
 * @param control_socket Control connection socket
 * @param data_socket Data connection socket
 * @param path Remote file path
 * @return 0 on success, -1 on error
 */
int ftpDownload(int control_socket, int data_socket, const char *path)
{
    char data_buffer[BUFF_SIZE];
    char response_buffer[BUFF_SIZE];
    char cmd[BUFF_SIZE];

    sprintf(cmd, "RETR %s\r\n", path);
    ftpSend(control_socket, cmd);

    int response_code = ftpRead(control_socket, response_buffer);
    if (response_code != 150 && response_code != 125) {
        fprintf(stderr, "ERROR: RETR failed (code %d)\n", response_code);
        return -1;
    }

    const char *filename = extractFileName(path);

    FILE *output_file = fopen(filename, "wb");

    if (!output_file)
    {
        perror("fopen()");
        return -1;
    }

    int bytes_read;
    int bytes_downloaded = 0;

    while ((bytes_read = read(data_socket, data_buffer, BUFF_SIZE)) > 0) {
        fwrite(data_buffer, 1, bytes_read, output_file);
        bytes_downloaded += bytes_read;
    }

    printf("\nDownload complete: %s (%d bytes)\n", filename, bytes_downloaded);

    fclose(output_file);
    close(data_socket);

    ftpRead(control_socket, response_buffer);
    return 0;
}

/**
 * Send QUIT command and close connection gracefully
 * 
 * @param control_socket Control socket file descriptor
 */
void ftpQuit(int control_socket)
{
    char response_buffer[BUFF_SIZE];
    ftpSend(control_socket, "QUIT\r\n");
    ftpRead(control_socket, response_buffer);
}

/* ============================================================================
 * MAIN PROGRAM
 * ============================================================================ */

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s ftp://[user:pass@]host/path_symbol/to/file\n", argv[0]);
        return 1;
    }

    URL_struct url_struct;

    printf("=== FTP Client ===\n");
    if (ftpUrlParser(argv[1], &url_struct) != 0)
        return 1;

    if (ipHostnameResolver(&url_struct) != 0)
        return 1;

    printf("Connection Info:\n");
    printf("Username : %s\n", url_struct.username);
    printf("Password : %s\n", url_struct.password);
    printf("Hostname : %s\n", url_struct.hostname);
    printf("Path     : %s\n", url_struct.path);
    printf("IP       : %s\n", url_struct.ip);

    printf("\nConnecting to FTP server...\n");
    int control_socket = connectionCreation(url_struct.ip, FTP_PORT);
    if (control_socket < 0)
        return 1;

    if (ftpLogin(control_socket, &url_struct) != 0)
    {
        close(control_socket);
        return 1;
    }

    char data_ip[64];
    int data_port;
    if (enterPassiveMode(control_socket, data_ip, &data_port) != 0)
    {
        close(control_socket);
        return 1;
    }
    printf("\nData connection: %s:%d\n", data_ip, data_port);

    int data_socket = connectionCreation(data_ip, data_port);
    if (data_socket < 0)
    {
        close(control_socket);
        return 1;
    }

    if (ftpDownload(control_socket, data_socket, url_struct.path) != 0)
    {
        close(control_socket);
        return 1;
    }

    ftpQuit(control_socket);
    close(control_socket);
    return 0;
}
