The FTP SERVER

To Compile:
make

To run:
ftserver <port number between 1024 and 65535>
--------------------------------------------------------------------------------

THE FTP CLIENT

Ensure permission are setup
chmod 777 ftclient.py

To run:
GET FILE LIST
./ftclient.py <flip1-3> <server_port> -l <data_port>

OR

ftclient.py <flip1-3> <server_port> - <data_port>

GET A SPECIFIC FILE
./ftclient.py <flip1-3> <server_port> -g <file_name> <data_port>

OR

ftclient.py <flip1-3> <sever_port> -g <file_name> <data_port>

--------------------------------------------------------------------------------

NOTES:
The CLIENT was tested on flip1

The SERVER was tested on flip2

