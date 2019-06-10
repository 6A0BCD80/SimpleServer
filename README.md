# SimpleServer
A simple file server that transfers a file from one machine to another on the same network (Usually used for CTF). 

Works on Linux and OSX.

To build, type, ./build in the terminal.

Options:
-f <file to server>
-p <port number> //Optional, default is 8080

Example:
Server side:
./a.out -f myfile.txt -p 8080

client side:
nc <server ip> 8080 > myfile.txt
