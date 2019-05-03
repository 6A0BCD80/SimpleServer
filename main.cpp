#include <string.h>
#include <stdio.h>
#include "required/intrinsic.h"
#include "required/nix.h"
#include "required/memory.h"
#include "required/platform.h"
#include "required/assert.h"
#include "marray.h"
#include "string.h"
#include "bucket.h"
#include "nix_socket.h"

s32 *filename = NULL;

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

		send(newSocket,fileChunk,fileSize,0);

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
	i32 sock = 0;

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
		printf("Listing on port: %i\n", port);
		printf("File: %s\n", filename);

		sock = ListenSocket(port,1,&acceptUsers);

	}

	if (filename)
	{
		Free(filename);
		filename=NULL;
	}

	MemoryResults();
}

