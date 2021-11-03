#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


int main(int argc, char const ** argv)
{
	if(0 > execlp("ls", "-a", "-l", (char*)NULL))
	{
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
