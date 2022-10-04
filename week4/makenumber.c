#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

int main()
{
	int readfd;
	int writefd;
	int retval;
	int nbytes;
	int lineNumber = 1;
	char buffer[3];
	char lineNumberBuffer[512];
	char number[512];

	if((readfd = open("Aladdin.txt",O_RDONLY)) < 0)
	{
		perror("open");
		exit(1);
	}
	if((writefd = open("Aladdin_num.txt",O_WRONLY)) < 0)
	{
		perror("open");
		exit(1);
	}
	
	while(1)
	{
		if((nbytes = read(readfd, buffer, 1)) < 0)
		{
			perror("read");
			exit(1);
		}

		if(buffer[0] != '\n')
		{
			if((nbytes = write(writefd,buffer,1)) < 0)
			{
				perror("write");
				exit(1);
			}
		}
		else
		{
		
			if((nbytes = write(writefd,buffer,1)) <0) // case of enter : print enter
			{
				perror("write");
				exit(1);
			}
		}
	}

	if((retval = close(readfd) < 0))
	{
		perror("close");
		exit(1);
	}
	if((retval = close(writefd) < 0))
	{
		perror("close");
		exit(1);
	}

	return 0;
}
