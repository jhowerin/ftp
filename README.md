# FTP Application
<p>
The program includes an FTP server written in C and a FTP client written in python.
The FTP server binds an IP address and IP Port to a socket and awaits incoming connections.
The FTP client initiates a control session to the FTP server, which in turn initiates the data
channel for data transfer
</p>

## To Run the FTP Server
1. Compile the FTP Server using make
2. Initiate the FTP Server with commands
```
ftserver <port number between 1024 and 65535>
```
--------------------------------------------------------------------------------
## To Run the FTP Client
1. Ensure permission are setup properly
```
chmod 777 ftclient.py
```
2. Get the list of files available
```
./ftclient.py <server> <server_port> -l <data_port>
```
OR
```
ftclient.py <server> <server_port> -l <data_port>
```
3. Get a specific files
```
./ftclient.py <server> <server_port> -g <file_name> <data_port>
```
OR
```
ftclient.py <server> <sever_port> -g <file_name> <data_port>
```
--------------------------------------------------------------------------------
