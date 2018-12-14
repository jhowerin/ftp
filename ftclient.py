#!/bin/python
#################################
# Jake Howering
# Oregon State University
# CS 372 - Project 2
# Client-Server Chat Application
# ftclient.py
# Spring 2018
#################################
from socket import *
import sys

BUFFER_SIZE = 1024
MIN_SIZE = 1024
MAX_SIZE = 65535
# function: commandLineValidation
# definition: check for appropriate command line arguments
# arguments: none
# return: none - exit program if input validation fails
def commandLineValidation():
	# Validation check with 5 command line arguments
	if (len(sys.argv) == 5):
		if(sys.argv[1] != 'flip1' and sys.argv[1] != 'flip2' and sys.argv[1] != 'flip3'):
			print("Server name invalid")
			exit(1)
		if(int(sys.argv[2]) < MIN_SIZE or int(sys.argv[2]) > MAX_SIZE):
			print("port number entered: ",sys.argv[2])
			print("Server control port number out of range.  Must be between 1024 and 65535")
			exit(1)
		if(sys.argv[3] != '-l'):
			print("FTP command not valid. Try with option '-l'")
			exit(1)
		if(int(sys.argv[4]) < MIN_SIZE or int(sys.argv[4]) > MAX_SIZE):
			print("Server data port number out of range.  Must be between 1024 and 65535")
			exit(1)
	# Validation check with 5 command line arguments
	elif (len(sys.argv) == 6):
		if(sys.argv[1] != 'flip1' and sys.argv[1] != 'flip2' and sys.argv[1] != 'flip3'):
			print("Server name invalid")
			exit(1)
		if(int(sys.argv[2]) < MIN_SIZE or int(sys.argv[2]) > MAX_SIZE):
			print("Server control port number out of range.  Must be between 1024 and 65535")
			exit(1)
		if(sys.argv[3] != '-g'):
			print("FTP command not valid. Try with option '-g'")
			exit(1)
		if(len(sys.argv[4]) < 1):
			print("File name error.  Please enter a filename")
			exit(1)
		if(int(sys.argv[5]) < MIN_SIZE or int(sys.argv[5]) > MAX_SIZE):
			print("Server data port number out of range.  Must be between 1024 and 65535")
			exit(1)
	# Validation check failed due to invalid number of arguments
	else:
		print("Invalid number of arguments")
		exit(1)
# function: createServerInfo
# definition: get the servername and port info
# arguments: none
# return: serverName, serverPort
def createServerInfo():
	serverName = sys.argv[1]+".engr.oregonstate.edu"
	serverPort = int(sys.argv[2])
	return serverName, serverPort
# function: initiateContact
# definition: create socket to remote server
# arguments: serverName, serverPort
# return: controlSocket
def initiateContact(serverName, serverPort):
	controlSocket = socket(AF_INET,SOCK_STREAM)
	controlSocket.connect((serverName, serverPort))
	return controlSocket
# function: createDataConnection
# definition: bind socket and listen
# arguments: clientSocket, dataPort
# return: dataSocket
def createDataConnection(clientsocket, dataPort):
	serverport = int(sys.argv[dataPort])
	serversocket = socket(AF_INET, SOCK_STREAM)
	serversocket.bind(('', serverport))
	serversocket.listen(1)
	data_socket, addr = serversocket.accept()
	return data_socket
# function: receiveFile
# definition: receive the file
# arguments: data_socket
# return: none
def receiveFile(data_socket, thePort):
	print("Receiving {} from {}:{}".format(sys.argv[4],sys.argv[1],sys.argv[thePort]))
	f = open(sys.argv[4],"w")
	# grab the output from the server
	buff = data_socket.recv(1000)
	# while we havent reached the end, write to the file
	while "__end__" not in buff:
		f.write(buff)
		buff = data_socket.recv(1000)
	print("File transfer complete.")
# function: makeRequest
# definition: send the command to server
# arguments: clientSocket, serverName, serverPort
# return: none
def makeRequest(clientsocket, serverName, serverPort):
		# two main section for the l or g ftp command
    	if sys.argv[3] == "-l":
			# send and receive the ports and commands
			thePort = 4
   			clientsocket.send(sys.argv[thePort])
			clientsocket.recv(1024)
			clientsocket.send("l")
			clientsocket.recv(1024)
			# http://stackoverflow.com/questions/24196932/how-can-i-get-the-ip-address-of-eth0-in-python
			s = socket(AF_INET, SOCK_DGRAM)
			s.connect(("8.8.8.8", 80))
			clientsocket.send(s.getsockname()[0])
			response = clientsocket.recv(1024)
			if response == "error":
				print("Server received an invalid command")
				exit(1)
			data_socket = createDataConnection(clientsocket, thePort)
			print("Receiving directory structure from {}:{}".format(sys.argv[1],sys.argv[thePort]))
			filename = data_socket.recv(100)
			# while we havent reached the end
			while filename != "end":
				# keep printing file names
				print filename
				filename = data_socket.recv(100)
			
    	elif sys.argv[3] == "-g":
			thePort = 5
			clientsocket.send(sys.argv[thePort])
			clientsocket.recv(1024)
			clientsocket.send("g")
			clientsocket.recv(1024)
			# http://stackoverflow.com/questions/24196932/how-can-i-get-the-ip-address-of-eth0-in-python
			s = socket(AF_INET, SOCK_DGRAM)
			s.connect(("8.8.8.8", 80))
			clientsocket.send(s.getsockname()[0])
			response = clientsocket.recv(1024)
			if response == "error":
				print("Server received an invalid command")
				exit(1)
			clientsocket.send(sys.argv[4])
			response = clientsocket.recv(1024)
			if response != "File found":
				print("{}:{} says FILE NOT FOUND".format(sys.argv[1],sys.argv[2]))
				return
			data_socket = createDataConnection(clientsocket, thePort)
			receiveFile(data_socket,thePort)
	# close the socket at the end
	data_socket.close()
# Program Flow
# Step 1 - check to ensure that the program initiated properly
commandLineValidation()
# Step 2 - create the serverInfo
serverName, serverPort = createServerInfo()
# Step 3 - create the control connection
controlConnection = initiateContact(serverName, serverPort)
# Step 4 - sends commands to server
makeRequest(controlConnection, serverName, serverPort)
