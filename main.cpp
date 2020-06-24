#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "required/intrinsic.h"
#include "required/nix.h"
#include "required/memory.h"
#include "required/platform.h"
#include "required/assert.h"
#include "marray.h"
#include "string.h"
#include "bucket.h"
#include "nix_socket.h"
#include <ifaddrs.h>
#include <linux/if_link.h>

s32 *filename = NULL;
b32 http=false;
i32 sock = 0;

void *Usage(char* programName)
{
	printf("\n\nSimple File Server\n");
	printf("Usage %s <options>\n", programName);
	printf("-f Specify the file to upload <required> \n");
	printf("-p Port number <not required, default: 8080>\n");
	printf("-h Send with http protocol\n");
	return 0;
}

ON_ACCEPT_USERS(acceptUsers)
{
	void *fileChunk = NULL;
	i32 fileSize = 0;
	FILE *file = fopen(filename,"rb");

	if (file)
	{
		fseek(file,0,SEEK_END);
		fileSize = ftell(file);
		rewind(file);

		fileChunk = MemoryRaw(fileSize + 1);

		fread(fileChunk,1,fileSize,file);

		if (http)
		{

			//NOTES(): Wait for a responds

			char buffer[256] = {};
			int readCount = 0;

			do
			{
				readCount =	recv(newSocket,buffer,255,0);

				if (readCount >  0)
				{

					s32 *temp = NULL;
					s32 *httpHeader = NULL;
					httpHeader = S32("HTTP/1.1 200 OK\n");
					i32 headerLength = 0;

					temp = httpHeader;

					char fileSizeString[256] = {};

					sprintf(fileSizeString,"%i",fileSize);


					httpHeader = S32Cat(temp,6,"Server: Apache/2.4.6 (CentOS)\n",
							"Accept-Ranges: bytes\n", 
							"Content-Type: application\n",
							"Content-Length: ", fileSizeString,"\n\n"
							);

					headerLength = Strlen(httpHeader);

					if (temp)
					{
						Free(temp);
						temp=NULL;
					}

					void *packet = MemoryRaw(headerLength + fileSize);

					memcpy(packet,httpHeader,headerLength);
					memcpy( ((ui8*) packet)+headerLength,fileChunk, fileSize);

					send(newSocket,packet,headerLength+fileSize,0);


					if (httpHeader)
					{
						Free(httpHeader);
						httpHeader=NULL;
					}
					if (packet)
					{
						Free(packet);
						packet=NULL;
					}

				}
			} while (readCount !=0);


		} else {
			send(newSocket,fileChunk,fileSize,0);
		}
		//close(sock);
		if (fileChunk)
		{
			Free(fileChunk);
			fileChunk=NULL;
		}

		fclose(file);
	}
}

bool CheckData()
{
	bool result = true;

	if (filename == NULL)
	{
		printf("Please enter a filename\n");
		result = false;
	} else {
		//NOTES(): Check if filew exist
		FILE *file = fopen(filename,"r");

		if (file == NULL)
		{
			printf("Can not openfile, either the file doesn't exist or you don't have permisson\n");
			result = false;
		} else {
			fclose(file);
		}
	}
	
	return result;
}

int main(int argc, char *argv[])
{
	i32 port = 8080;
	struct ifaddrs *ifAddr = NULL, *ifa = NULL;
	int family = 0,s = 0;
	char host[NI_MAXHOST] = {};

	for (i32 argIndex=1; argIndex < argc;argIndex+=2)
	{
		if (argIndex + 1 <= argc)
		{
			char *options = *(argv + argIndex);
			char *value = *(argv + argIndex+1);

			if (StrCmp(options,"-p"))
			{
					port = atoi(value);
			}

			if (StrCmp(options,"-h"))
			{
				http=true;
			}

			if (StrCmp(options,"-f"))
			{
				if (filename == NULL)
				{
					filename = S32(value);
				}
			}
		}
	}

	if (CheckData()) 
	{
		if (getifaddrs(&ifAddr) == -1)
		{
			printf("Unable to get ip Address\n");
		} else {
			for (ifa  = ifAddr;ifa != NULL;ifa = ifa->ifa_next)
			{
				family = ifa->ifa_addr->sa_family;

				if (ifa->ifa_addr)
				{
					s = getnameinfo(ifa->ifa_addr,
							(family == AF_INET) ? sizeof(struct sockaddr_in) :
							sizeof(struct sockaddr_in6),
							host, NI_MAXHOST,
							NULL, 0, NI_NUMERICHOST);

					printf("%-8s : %s\n",ifa->ifa_name, host);

				}
			}
		}


		printf("Listing on port: %i\n", port);
		printf("File: %s\n", filename);

		sock = ListenSocket(port,1,&acceptUsers);

	} else {
		Usage(argv[0]);
	}

	if (filename)
	{
		Free(filename);
		filename=NULL;
	}

	MemoryResults();
}

