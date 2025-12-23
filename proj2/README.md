# FTP Protocol

 - [Running FTP App ](#running-ftp-app)
 - [FTP Protocol Implementation](#ftp-protocol-implementation)
     - [URL](#url) 
        - [URL Structure](#url-structure)
        - [URL Parsing](#url-parsing)
     - [Server IP Resolution](#server-ip-resolution)
     - [Connection Establishment](#connection-establishment)
     - [Login](#login)
     - [Passive Mode](#passive-mode)
     - [File Download](#file-download)
     - [Quit](#quit)

## Running FTP App

To run FTP application, execute the following command replacing the tag with the respective URL

```bash
make run <VALID-FTP-URL>
```

## FTP Protocol Implementation:

### URL

#### URL Structure

The structure `URL_struct` was designed to store all the core information required to run the FTP protocol. This includes the decomposed URL components, as well as the resolved IP address of the server.

```c
typedef struct
{
    char username[BUFF_SIZE];
    char password[BUFF_SIZE];
    char hostname[BUFF_SIZE];
    char path[BUFF_SIZE];
    char ip[BUFF_SIZE];    
} URL_struct;
```

#### URL Parsing

The function `ftpUrlParser` parses the URL and fills the `URL_struct`. 
In case of an anonymous connection the values of USER and PASS are both "anonymous".


### Server IP Resolution

For this part of the example code provided, `getip.c` already had a functional version that was capable of resolving the IP based on an address. We already uncovered the address by parsing the URL, this step is straightforward and only requires calling the `gethostbyname` function and storing the returned IP address returned in the `URL_struct`.

### Connection Establishment

The function responsible for this step is called `connectionCreation`. Its purpose is to create a connection to the server via the IP address and returns a connected socket file descriptor.


### Login

The login is handled by the function `ftpLogin` which handles the `USER` and `PASS` commands
 while taking into account the return codes sent from the server. It then tells the server that the transfer mode will be binary by sending the `TYPE I` command.


### Passive Mode Username : demo
Password : password
Hostname : test.rebex.net
Path     : /readme.txt
IP       : 194.108.117.16

The function that handles the logic behind the passive mode connection is the `enterPassiveMode`. 
This function starts by sending the PASV command, and after receiving a successful response it extracts the extracts the `(h1,h2,h3,h4,p1,p2)` tuple and computes the data connection IP address and port. This information will be further used by the `connectionCreation` function to establish the connection.  


### File Download

The file download logic is handled by the function `ftpDownload`. 
This function starts by sending the RETR command. After receiving a successful response it parses the path to extract the file name and opens/creates it. After the setup is complete, the function continuously reads data from the data connection. When the transfer finishes, it closes the data connection and the output file. Then reads the final response from the server.


### Quit

The final step of the protocol is closing the control connection by sending the `QUIT` command to the server, which responds with a `221 Goodbye` message.