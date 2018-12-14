/******************************
 * Jake Howering
 * Oregon State University
 * CS 372 - Project 2
 * Client-Server Chat Application
 * ftserver.c
 * Spring 2018
 ******************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
/******************************
 * function: createAddress
 * Definition: Prepare to create 
 * the socket address from command 
 * line arguments
 * Arguments: arg1
 * Return: server information
 ******************************/
struct addrinfo * createAddress(char * arg1, char * arg2);
/******************************
 * function: createSocket
 * Definition: create the socket
 * Arguments: servInfo
 * Return: server socket
 ******************************/
int createSocket(struct addrinfo * servInfo);
/******************************
 * function: bindSocket
 * Definition: bind the socket and server 
 * Arguments: sockfd, servInfo
 * Return: none
 ******************************/
void bindSocket(int sockfd, struct addrinfo * servInfo);
  /******************************
 * function: sendFile
 * Definition: send requested file
 * Arguments: ipAddress, port, file
 * Return: none
 ******************************/
void sendFile(char * ipAddress, char * port, char * filename);
  /******************************
 * function: sendList
 * Definition: send the list of files
 * Arguments: ipAddress, port, files, numberOfFiles
 * Return: none
 ******************************/
void sendList(char * ipAddress, char * port, char ** files, int numberOfFiles);
 /******************************
 * function: handleRequest
 * Definition: perform actions
 * based on received FTP command
 * Arguments: newFd, port, ipAddress,
 * okMsg, errMsg, command
 * Return: none
 ******************************/
void handleRequest(int newFd, char port[100], char ipAddress[100], 
	char okMsg[100], char errMsg[100], char command[100]);
 /******************************
 * function: startup
 * Definition: receive the client 
 * commands and perform action
 * Arguments: newFd / socket
 * Return: none
 ******************************/
void startup(int newFd);
/******************************
 * function: main
 * Definition: main function
 * Arguments: command line args
 ******************************/
int main(int argc, char *argv[]){
// Step 1 - command line validation
	if(argc != 2){
		printf("Error: enter executable serverPort\n");
		exit(1);
	}
	printf("Server open on %s\n", argv[1]);
// Step 2 - Create the server information
	struct addrinfo * servInfo = createAddress(NULL,argv[1]);
// Step 3 - Create the server socket
	int sockfd = createSocket(servInfo);
	if(sockfd == -1){
		printf("Socket error!\n");
		exit(1);
	}
// Step 4 - Bind the server IP and Port to the socket
	bindSocket(sockfd, servInfo);
// Step 5 - Put socket into listen mode
	if(listen(sockfd, 5) == -1){
		close(sockfd);
		fprintf(stderr, "Error in listening on socket\n");
		exit(1);
	}
// Step 6 - Ready and waiting for connections
// create connection address
	struct sockaddr * their_addr;
	// create a size for the connection
    socklen_t addr_size;
	// create a new file descriptor for the connection
	int newFd;
	// run forever
	while(1){
		// get the address size
		addr_size = sizeof(their_addr);
		// accept a new client
		newFd = accept(sockfd, &their_addr, &addr_size);
		// if there is no new client keep waiting
		if(newFd == -1){
			// no incoming connection for now, keep waiting
			continue;
		}
		// Step 6a - startup
		startup(newFd);
		close(newFd);
	}	
// Step 7 - Wrap up - close connection, free memory
// Reference: http://beej.us/guide/bgnet/html/single/bgnet.html#simpleclient
	close(sockfd);
	freeaddrinfo(servInfo);
}
//Function Definitions
struct addrinfo * createAddress(char * arg1, char * arg2){
	// Reference: http://beej.us/guide/bgnet/html/single/bgnet.html#simpleclient
	struct addrinfo hints, *servSocket;
	int rv;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	// If rv is not 0, we have an error because we did not fill out the address struct correctly
	if((rv = getaddrinfo(arg1, arg2, &hints, &servSocket)) != 0){
		fprintf(stderr,"getaddrinfo error: %s\nPlease check socket information\n",gai_strerror(rv));
		exit(1);
	}
	return servSocket;
}
int createSocket(struct addrinfo * servInfo){
	// Reference: http://beej.us/guide/bgnet/html/single/bgnet.html#simpleclient
	// The socket function is part of the sys/socket.h library
	// For portability of the program, also include sys/types.h library, "just in case"
	int sockfd;
	if ((sockfd = socket((servInfo)->ai_family, servInfo->ai_socktype, servInfo->ai_protocol)) == -1){
		fprintf(stderr, "Error in creating socket\n");
		exit(1);
	}
	return sockfd;
}
void bindSocket(int sockfd, struct addrinfo * servInfo){
	if (bind(sockfd, servInfo->ai_addr, servInfo->ai_addrlen) == -1) {
		close(sockfd);
		fprintf(stderr, "Error in binding socket\n");
		exit(1);
	}
}
void sendFile(char * ipAddress, char * port, char * filename){
	sleep(2); //delay for setup
	//Reference: http://beej.us/guide/bgnet/html/single/bgnet.html#simpleserver
	struct addrinfo * res = createAddress(ipAddress, port);
	int dataSocket = createSocket(res);
	int status;
	if ((status = connect(dataSocket, res->ai_addr, res->ai_addrlen)) == -1){
		fprintf(stderr, "Error in connecting socket\n");
		exit(1);
	}
	// create and set buffer for the file contents to send.
	char buffer[1000];
	memset(buffer, 0, sizeof(buffer));
	// Reference: http://www.mathcs.emory.edu/~cheung/Courses/561/Syllabus/3-C/bin-files.html
	// Reference: https://stackoverflow.com/questions/11229230/sending-and-receiving-a-file-server-client-in-c-using-socket-on-unix
	int fd = open(filename, O_RDONLY);
	while (1) {
		int bytes_read = read(fd, buffer, sizeof(buffer)-1);
		if (bytes_read == 0)
			break;
		if (bytes_read < 0) {
			printf("Error reading file\n");
			return;
		}
		void *p = buffer;
		while (bytes_read > 0) {
			int bytes_written = send(dataSocket, p, sizeof(buffer),0);
			if (bytes_written < 0) {
				printf("Error writing to socket\n");
				return;
			}
			bytes_read -= bytes_written;
			p += bytes_written;
		}
		memset(buffer,0,sizeof(buffer));
	}
	// clear out the buffer and send the end message
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, "__end__");
	send(dataSocket, buffer, sizeof(buffer),0);
	// close the socket and free the address info
	close(dataSocket);
	freeaddrinfo(res);
}
void sendList(char * ipAddress, char * port, char ** files, int numberOfFiles){
	sleep(2); //delay for setup 
	//Reference: http://beej.us/guide/bgnet/html/single/bgnet.html#simpleserver
	struct addrinfo * res = createAddress(ipAddress, port);
	int dataSocket = createSocket(res);
	int status;
	if ((status = connect(dataSocket, res->ai_addr, res->ai_addrlen)) == -1){
		fprintf(stderr, "Error: Data socket error\n");
		exit(1);
	}
	// Iterate and send the files
	int i = 0;
	for (; i < numberOfFiles; i++){
		send(dataSocket,files[i],100,0);
	}
	// send endMsg to let the client know we have completed sending the directory
	char * endMsg = "end";
	send(dataSocket, endMsg, strlen(endMsg),0);
	// close socket and free address information
	close(dataSocket);
	freeaddrinfo(res);
}
void handleRequest(int newFd, char port[100], char ipAddress[100], 
	char okMsg[100], char errMsg[100], char command[100]){
	if(strcmp(command,"l") == 0){
		// acknowledge command with an okMsg
		send(newFd, okMsg, strlen(okMsg),0);
		printf("List directory requested on port %s\n", port);
		printf("Sending directory contents to %s:%s\n", ipAddress, port);
		// Reference: https://stackoverflow.com/questions/32442600/memory-allocation-for-char-array
		char ** array = malloc(100*sizeof(char *));
		int i = 0;
		for(; i < 100; i++){
			array[i] = malloc(100*sizeof(char));
			memset(array[i],0,sizeof(array[i]));
		}
		char ** files = array;
		// Reference: https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
		DIR * d;
		struct dirent * dir;
		d = opendir(".");
		i = 0;
		if (d){
			while ((dir = readdir(d)) != NULL){
				if (dir->d_type == DT_REG){
					strcpy(files[i], dir->d_name);
					i++;
				}
			}
			closedir(d);
		}
		int numberOfFiles = i;
		// send the contents of the file array to the client
		sendList(ipAddress, port, files, numberOfFiles);
		//free memory
		i = 0;
		for (; i < 100; i++){
			free(files[i]);
		}
	}
	else if(strcmp(command, "g") == 0){
		//command received was g
		send(newFd, okMsg, strlen(okMsg),0);
		// get the file name from the client
		char filename[100];
		memset(filename, 0, sizeof(filename));
		recv(newFd, filename, sizeof(filename)-1,0);
		printf("File \"%s\" requested on port %s\n", filename, port);
		// Reference: https://stackoverflow.com/questions/32442600/memory-allocation-for-char-array	
		char ** array = malloc(100*sizeof(char *));
		int i = 0;
		for(; i < 100; i++){
			array[i] = malloc(100*sizeof(char));
			memset(array[i],0,sizeof(array[i]));
		}
		char ** files = array;
		// Reference: https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
		DIR * d;
		struct dirent * dir;
		d = opendir(".");
		i = 0;
		if (d){
			while ((dir = readdir(d)) != NULL){
				if (dir->d_type == DT_REG){
					strcpy(files[i], dir->d_name);
					i++;
				}
			}
			closedir(d);
		}
		int numberOfFiles = i;
		int flag = 0;
		i = 0;
		//check if file is in directory
		for (; i < numberOfFiles; i++){
			if(strcmp(files[i], filename) == 0){
				flag = 1;
			}
		}
		if(flag){
			// if file is found
			printf("File found, sending %s to client\n", filename);
			char * fileFound = "File found";
			send(newFd, fileFound, strlen(fileFound),0);
			// create a new filename based on current location
			char newFilename[100];
			memset(newFilename,0,sizeof(newFilename));
			strcpy(newFilename, "./");
			char * end = newFilename + strlen(newFilename);
			end += sprintf(end, "%s", filename);
			// send new file to client
			sendFile(ipAddress, port, newFilename);
		}
		else{
			// else the file was not found, send that to the client
			printf("File not found, sending error message to %s:%s\n", ipAddress, port);
			char * file_not_found = "File not found";
			send(newFd, file_not_found, 100, 0);
		}
		//free memory
		i = 0;
		for (; i < 100; i++){
			free(files[i]);
		}
	}
	else{// If the command is not recognized as either l or g
		send(newFd, errMsg, strlen(errMsg), 0);
		printf("Error: command not found, please send either -l or -g\n");
	}
	//let user know - ready for new connections
	printf("Waiting for new connections\n");
}
void startup(int newFd){
	char * okMsg = "ok";
	char * errMsg = "error";
	// receive the data connection port from the client
	char port[100];
	memset(port, 0, sizeof(port));
	if(recv(newFd, port, sizeof(port)-1, 0) == -1){
		printf("Error: error receiving port\n");
		send(newFd, errMsg, strlen(errMsg),0);
		exit(1);
	}
	// send ok message for the data connection port from the client
	else
		send(newFd, okMsg, strlen(okMsg),0);
	// receive the FTP command from the client
	char command[100];
	// Reference: http://beej.us/guide/bgnet/html/single/bgnet.html
	memset(command,0,sizeof(command));
	if(recv(newFd, command, sizeof(command)-1, 0) == -1){
		printf("Error: error receiving the command\n");
		send(newFd, errMsg, strlen(errMsg),0);
		exit(1);
	}
	// send ok message for the data connection FTP command from the client
	else
		send(newFd, okMsg, strlen(okMsg),0);
	// receive the ip of the client
	char ipAddress[100];
	memset(ipAddress,0,sizeof(ipAddress));
	recv(newFd, ipAddress, sizeof(ipAddress)-1,0);
	// print that we have a connection
	printf("Connection from %s\n", ipAddress);
	//call the handleRequest function and go deeper into the rabbit hole
	handleRequest(newFd, port, ipAddress, okMsg, errMsg, command);
}