/**
 * FTP Client Application
 * 
 * Implementation of an FTP client following RFC 959
 * Features:
 *   - URL parsing (ftp://[user:pass@]host/path)
 *   - DNS resolution
 *   - FTP authentication
 *   - Passive mode data transfer
 *   - Binary file download with progress indicator
 * 
 * Reference: https://beej.us/guide/bgnet/pdf/bgnet_a4_c_2.pdf
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

/* ============================================================================
 * CONSTANTS AND DATA STRUCTURES
 * ============================================================================ */

#define BUFF_SIZE 512
#define FTP_PORT 21

typedef struct {
    char username[BUFF_SIZE];
    char password[BUFF_SIZE];
    char hostname[BUFF_SIZE];
    char filepath[BUFF_SIZE];
    char ip_address[BUFF_SIZE];
} FTP_URL;


/* ============================================================================
 * URL PARSING
 * ============================================================================ */

/**
 * Parse FTP URL into components
 * Expected format: ftp://[user:pass@]host/path
 * 
 * @param url The URL string to parse
 * @param parsed_url Pointer to FTP_URL to store parsed components
 * @return 0 on success, -1 on error
 */
int ftpUrlParser(const char *url, FTP_URL *parsed_url) {
    if (strlen(url) > BUFF_SIZE) {
        fprintf(stderr, "ERROR: URL too long (max %d chars)\n", BUFF_SIZE);
        return -1;
    }
    
    if (strncmp(url, "ftp://", 6) != 0) {
        fprintf(stderr, "ERROR: URL must start with ftp://\n");
        return -1;
    }

    const char *ptr = url + 6;  // Skip "ftp://"
    const char *at_symbol = strchr(ptr, '@');
    const char *slash;

    // Parse username and password if provided
    if (at_symbol) {
        const char *colon = strchr(ptr, ':');
        if (!colon || colon > at_symbol) {
            fprintf(stderr, "ERROR: Invalid URL format (user:pass@host/path)\n");
            return -1;
        }

        // Extract username
        size_t username_len = colon - ptr;
        if (username_len >= BUFF_SIZE) {
            fprintf(stderr, "ERROR: Username too long\n");
            return -1;
        }
        strncpy(parsed_url->username, ptr, username_len);
        parsed_url->username[username_len] = '\0';

        // Extract password
        size_t password_len = at_symbol - (colon + 1);
        if (password_len >= BUFF_SIZE) {
            fprintf(stderr, "ERROR: Password too long\n");
            return -1;
        }
        strncpy(parsed_url->password, colon + 1, password_len);
        parsed_url->password[password_len] = '\0';

        ptr = at_symbol + 1;  // Move past '@'
    } else {
        // Use anonymous credentials
        strcpy(parsed_url->username, "anonymous");
        strcpy(parsed_url->password, "anonymous");
    }

    // Parse host and path
    slash = strchr(ptr, '/');
    if (!slash) {
        fprintf(stderr, "ERROR: URL must include path (host/path)\n");
        return -1;
    }

    // Extract host
    size_t hostname_len = slash - ptr;
    if (hostname_len >= BUFF_SIZE) {
        fprintf(stderr, "ERROR: Host too long\n");
        return -1;
    }
    strncpy(parsed_url->hostname, ptr, hostname_len);
    parsed_url->hostname[hostname_len] = '\0';

    // Extract path
    strncpy(parsed_url->filepath, slash, BUFF_SIZE - 1);
    parsed_url->filepath[BUFF_SIZE - 1] = '\0';

    return 0;
}


/* ============================================================================
 * NETWORK UTILITIES
 * ============================================================================ */

/**
 * Resolve hostname to IP address using DNS
 * 
 * @param ftp_url Pointer to FTP_URL containing hostname and to store IP
 * @return 0 on success, -1 on error
 */
int ipAddressResolver(FTP_URL *ftp_url) {
    if (!ftp_url) {
        fprintf(stderr, "ERROR: NULL pointer provided\n");
        return -1;
    }

    struct hostent *host_entry;
    if ((host_entry = gethostbyname(ftp_url->hostname)) == NULL) {
        herror("gethostbyname()");
        return -1;
    }

    strcpy(ftp_url->ip_address, inet_ntoa(*((struct in_addr *)host_entry->h_addr_list[0])));
    return 0;
}

/**
 * Create TCP connection to FTP server
 * 
 * @param ftp_url Pointer to FTP_URL with IP and connection info
 * @return Socket file descriptor on success, -1 on error
 */
int connectionCreation(FTP_URL *ftp_url) {
    int control_socket;
    struct sockaddr_in server_addr;

    // Initialize server address structure
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ftp_url->ip_address);
    server_addr.sin_port = htons(FTP_PORT);

    // Create socket
    if ((control_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    // Connect to server
    if (connect(control_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        close(control_socket);
        return -1;
    }
    
    return control_socket;
}

/**
 * Open data connection for file transfer (used in PASV mode)
 * 
 * @param data_ip IP address string
 * @param data_port Port number
 * @return Socket file descriptor on success, -1 on error
 */
int openDataConnection(const char *data_ip, int data_port) {
    int data_socket;
    struct sockaddr_in data_addr;

    if ((data_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("data socket()");
        return -1;
    }

    bzero(&data_addr, sizeof(data_addr));
    data_addr.sin_family = AF_INET;
    data_addr.sin_port = htons(data_port);
    data_addr.sin_addr.s_addr = inet_addr(data_ip);

    if (connect(data_socket, (struct sockaddr *)&data_addr, sizeof(data_addr)) < 0) {
        perror("data connect()");
        close(data_socket);
        return -1;
    }

    return data_socket;
}


/* ============================================================================
 * FTP PROTOCOL COMMUNICATION
 * ============================================================================ */

/**
 * Send command to FTP server
 * 
 * @param control_socket Socket file descriptor
 * @param command Command string to send
 * @return 0 on success, -1 on error
 */
int ftpSend(int control_socket, const char *command) {
    printf("> %s", command);
    int bytes_written = write(control_socket, command, strlen(command));
    if (bytes_written <= 0) {
        perror("write()");
        return -1;
    }
    return 0;
}

/**
 * Read FTP server reply (handles multi-line responses per RFC 959)
 * 
 * @param control_socket Socket file descriptor
 * @param reply_buffer Buffer to store complete reply
 * @return Numeric status code (e.g., 220, 230, 227) or -1 on error
 */
int ftpRead(int control_socket, char *reply_buffer) {
    char temp_buffer[BUFF_SIZE];
    int total_bytes = 0;

    reply_buffer[0] = '\0';

    while (1) {
        bzero(temp_buffer, BUFF_SIZE);

        int bytes_read = read(control_socket, temp_buffer, BUFF_SIZE - 1);
        if (bytes_read <= 0) {
            perror("read()");
            return -1;
        }

        if (total_bytes + bytes_read >= BUFF_SIZE) {
            fprintf(stderr, "ERROR: FTP reply too long\n");
            return -1;
        }
        
        strncat(reply_buffer, temp_buffer, bytes_read);
        total_bytes += bytes_read;

        printf("< %s", temp_buffer);

        // Find last line in current chunk
        char *last_line = temp_buffer;
        char *current_char = temp_buffer;
        while (*current_char) {
            if (*current_char == '\n' && *(current_char + 1) != '\0') {
                last_line = current_char + 1;
            }
            current_char++;
        }

        // Check if last line is a complete final response (XXX<space>)
        if (strlen(last_line) >= 4 &&
            isdigit((unsigned char)last_line[0]) &&
            isdigit((unsigned char)last_line[1]) &&
            isdigit((unsigned char)last_line[2]) &&
            last_line[3] == ' ') {
            return atoi(last_line);
        }
    }
}


/* ============================================================================
 * FTP OPERATIONS
 * ============================================================================ */

/**
 * Authenticate with FTP server
 * Sends USER and PASS commands, then sets binary transfer mode
 * 
 * @param control_socket Control socket file descriptor
 * @param ftp_url Pointer to FTP_URL with credentials
 * @return 0 on success, -1 on error
 */
int ftpLogin(int control_socket, FTP_URL *ftp_url) {
    char response_buffer[BUFF_SIZE];
    char command[BUFF_SIZE * 2];

    // Read server greeting (expect 220)
    if (ftpRead(control_socket, response_buffer) != 220) {
        fprintf(stderr, "ERROR: FTP server not ready\n");
        return -1;
    }

    // Send username
    sprintf(command, "USER %s\r\n", ftp_url->username);
    if (ftpSend(control_socket, command) < 0)
        return -1;

    int response_code = ftpRead(control_socket, response_buffer);
    if (response_code != 331 && response_code != 230) {
        fprintf(stderr, "ERROR: Invalid USER response (code %d)\n", response_code);
        return -1;
    }

    // Send password if needed (331 = password required)
    if (response_code == 331) {
        sprintf(command, "PASS %s\r\n", ftp_url->password);
        if (ftpSend(control_socket, command) < 0)
            return -1;

        if (ftpRead(control_socket, response_buffer) != 230) {
            fprintf(stderr, "ERROR: Login failed\n");
            return -1;
        }
    }

    // Set binary transfer mode
    sprintf(command, "TYPE I\r\n");
    if (ftpSend(control_socket, command) < 0)
        return -1;
    if (ftpRead(control_socket, response_buffer) != 200) {
        fprintf(stderr, "ERROR: Failed to set binary mode\n");
        return -1;
    }

    return 0;
}

/**
 * Enter passive mode and get data connection info
 * Sends PASV command and parses response (h1,h2,h3,h4,p1,p2)
 * 
 * @param control_socket Control socket file descriptor
 * @param data_ip Buffer to store data connection IP address
 * @param data_port Pointer to store data connection port number
 * @return 0 on success, -1 on error
 */
int enterPassiveMode(int control_socket, char *data_ip, int *data_port) {
    char response_buffer[BUFF_SIZE];

    if (ftpSend(control_socket, "PASV\r\n") < 0)
        return -1;

    int response_code = ftpRead(control_socket, response_buffer);
    if (response_code != 227) {
        fprintf(stderr, "ERROR: PASV failed (code %d)\n", response_code);
        fprintf(stderr, "Raw response:\n%s\n", response_buffer);
        return -1;
    }

    // Parse PASV response: "227 ... (h1,h2,h3,h4,p1,p2)"
    char *paren_pos = strchr(response_buffer, '(');
    if (!paren_pos) {
        fprintf(stderr, "ERROR: PASV parse error - no '('\n");
        return -1;
    }

    int h1, h2, h3, h4, p1, p2;
    if (sscanf(paren_pos + 1, "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2) != 6) {
        fprintf(stderr, "ERROR: PASV parse error - malformed numbers\n");
        return -1;
    }

    sprintf(data_ip, "%d.%d.%d.%d", h1, h2, h3, h4);
    *data_port = p1 * 256 + p2;

    return 0;
}

/**
 * Get file size using SIZE command
 * 
 * @param control_socket Control socket file descriptor
 * @param remote_path Remote file path
 * @return File size in bytes, or 0 if SIZE command fails
 */
int ftpGetFileSize(int control_socket, const char *remote_path) {
    char command[BUFF_SIZE];
    char response_buffer[BUFF_SIZE];

    sprintf(command, "SIZE %s\r\n", remote_path);
    ftpSend(control_socket, command);

    int response_code = ftpRead(control_socket, response_buffer);
    if (response_code != 213) {
        fprintf(stderr, "WARNING: SIZE command failed (code %d)\n", response_code);
        return 0;
    }

    int file_size = 0;
    sscanf(response_buffer, "%*d %d", &file_size);
    return file_size;
}

/**
 * Extract filename from path
 * 
 * @param filepath Full file path
 * @return Pointer to filename or default name
 */
const char *extractFileName(const char *filepath) {
    const char *last_slash = strrchr(filepath, '/');

    if (!last_slash || *(last_slash + 1) == '\0') {
        return "download.bin";
    }

    return last_slash + 1;
}

/**
 * Download file from FTP server with progress indicator
 * 
 * @param control_socket Control connection socket
 * @param data_socket Data connection socket
 * @param remote_path Remote file path
 * @return 0 on success, -1 on error
 */
int ftpDownload(int control_socket, int data_socket, const char *remote_path) {
    char data_buffer[BUFF_SIZE];
    char command[BUFF_SIZE];

    // Get file size for progress tracking
    int total_file_size = ftpGetFileSize(control_socket, remote_path);

    // Send RETR command
    sprintf(command, "RETR %s\r\n", remote_path);
    ftpSend(control_socket, command);

    char response_buffer[BUFF_SIZE];
    int response_code = ftpRead(control_socket, response_buffer);
    if (response_code != 150 && response_code != 125) {
        fprintf(stderr, "ERROR: RETR failed (code %d)\n", response_code);
        return -1;
    }

    // Open local file for writing
    const char *local_filename = extractFileName(remote_path);
    FILE *output_file = fopen(local_filename, "wb");
    if (!output_file) {
        perror("fopen()");
        return -1;
    }

    // Download file with progress indicator
    int bytes_read;
    int bytes_downloaded = 0;
    int last_percent = -1;

    printf("\nDownloading to: %s\n", local_filename);

    while ((bytes_read = read(data_socket, data_buffer, BUFF_SIZE)) > 0) {
        fwrite(data_buffer, 1, bytes_read, output_file);
        bytes_downloaded += bytes_read;

        // Update progress bar if we know file size
        if (total_file_size > 0) {
            int current_percent = (bytes_downloaded * 100) / total_file_size;

            if (current_percent != last_percent) {
                last_percent = current_percent;

                int bar_width = 40;
                int filled_position = (current_percent * bar_width) / 100;

                printf("\r[");
                for (int i = 0; i < bar_width; i++) {
                    if (i < filled_position)
                        printf("#");
                    else
                        printf(" ");
                }

                printf("] %3d%%   %d / %d MB",
                       current_percent,
                       bytes_downloaded / (1024 * 1024),
                       total_file_size / (1024 * 1024));
                fflush(stdout);
            }
        }
    }

    printf("\n✓ Download complete: %s (%d bytes)\n", local_filename, bytes_downloaded);

    fclose(output_file);
    close(data_socket);

    // Read final transfer status
    ftpRead(control_socket, response_buffer);
    return 0;
}

/**
 * Send QUIT command and close connection gracefully
 * 
 * @param control_socket Control socket file descriptor
 */
void ftpQuit(int control_socket) {
    ftpSend(control_socket, "QUIT\r\n");
    char response_buffer[BUFF_SIZE];
    ftpRead(control_socket, response_buffer);
}


/* ============================================================================
 * MAIN PROGRAM
 * ============================================================================ */

int main(int argc, char **argv) {
    // Validate arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s ftp://[user:pass@]host/path/to/file\n", argv[0]);
        fprintf(stderr, "Example: %s ftp://anonymous:pass@ftp.example.com/pub/file.txt\n", argv[0]);
        return 1;
    }

    FTP_URL ftp_url;

    // Parse URL
    printf("=== FTP Client ===\n");
    if (ftpUrlParser(argv[1], &ftp_url) != 0)
        return 1;

    // Resolve hostname to IP
    if (ipAddressResolver(&ftp_url) != 0)
        return 1;

    // Display connection info
    printf("\nConnection Info:\n");
    printf("  User     : %s\n", ftp_url.username);
    printf("  Password : %s\n", ftp_url.password);
    printf("  Host     : %s\n", ftp_url.hostname);
    printf("  IP       : %s\n", ftp_url.ip_address);
    printf("  Path     : %s\n\n", ftp_url.filepath);

    // Connect to FTP server
    printf("Connecting to FTP server...\n");
    int control_socket = connectionCreation(&ftp_url);
    if (control_socket < 0)
        return 1;

    // Authenticate
    if (ftpLogin(control_socket, &ftp_url) != 0) {
        close(control_socket);
        return 1;
    }

    // Enter passive mode
    char data_ip[64];
    int data_port;
    if (enterPassiveMode(control_socket, data_ip, &data_port) != 0) {
        close(control_socket);
        return 1;
    }

    printf("Data connection: %s:%d\n", data_ip, data_port);

    // Open data connection
    int data_socket = openDataConnection(data_ip, data_port);
    if (data_socket < 0) {
        close(control_socket);
        return 1;
    }

    // Download file
    if (ftpDownload(control_socket, data_socket, ftp_url.filepath) != 0) {
        close(control_socket);
        return 1;
    }

    // Disconnect
    ftpQuit(control_socket);
    close(control_socket);
    
    printf("\nSession closed.\n");
    return 0;
}