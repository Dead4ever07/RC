
## Implementation Cookbook:
 - Url parser and conversion to a struct 
 - Server ip resolver
 - Connection establishment (Includes socket creation and connection)
 - Login with the right credentials
 - Send PASV command and create a new ip with the right port based on the server response
 - Creation of a new socket to the data ip:port
 - Start the file retrieval with the command RETR
 - Close data socket
 - Send QUIT to the command socket and closing command socket 
