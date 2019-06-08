# SimpleServer
A simple file server that transfer a file from one machine to another on the same network. I wrote this because you don't really need a whole http server to transfer one file. Most Linux have nc installed already and all you really need to is make it listen to the server to recieve your file.

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
