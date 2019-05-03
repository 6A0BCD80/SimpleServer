#ifndef NIX_SOCKET_H
#define NIX_SOCKET_H
#include <sys/mman.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ON_ACCEPT_USERS(name) void name(i32 newSocket)
typedef ON_ACCEPT_USERS(on_accept_users);

int CreateSocket(char urlString[], int portNumber)
{
	int sock=0;
	char *tmp_ptr;
	int port = 0;
	struct hostent *host = NULL;
	struct sockaddr_in dest_addr={};

	port = portNumber;

	host = gethostbyname(urlString);

	if (host == NULL)
	{
		printf("unable to get host\n");
	} else {
		sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

		if (sock == -1)
		{
			printf("Socket failed\n");
		} else {

			printf("%i\n",host->h_length);

			dest_addr.sin_family=AF_INET;
			dest_addr.sin_port = htons(port);

			memcpy(&dest_addr.sin_addr, host->h_addr_list[0], host->h_length);
			//dest_addr.sin_addr.s_addr = *(unsigned long*)(host->h_addr);
		
			memset(&(dest_addr.sin_zero),'\0',8);

		//	tmp_ptr = inet_ntoa(dest_addr.sin_addr);

			if (connect(sock, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr)) == -1)
			{
				printf("could not connect to %s\n", urlString);
			}
		}
	}


	return sock;
}

i32 ListenSocket(i32 port, i32 maxClients,on_accept_users *accept_user_function)
{
     i32 sock=0;
     i32 newsockfd=0; 
     i32 portno=0; 
     i32 clilen;
     struct sockaddr_in serv_addr={};
     struct sockaddr_in cli_addr={};

	sock = socket(AF_INET,SOCK_STREAM,0);

	Assert(sock);
	bzero((char*) &serv_addr, sizeof(serv_addr));
	portno = port;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sock,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("can not bind socket!\n");
	} else {
		listen(sock,maxClients);

		newsockfd = accept(sock,(struct sockaddr *) &cli_addr, (unsigned int*) &clilen);

		if (newsockfd < 0)
		{
			printf("An error has occured!\n");
		} else {
			if (accept_user_function)
			{
				accept_user_function(newsockfd);
			}
		}
	}

	return sock;
}

#endif
