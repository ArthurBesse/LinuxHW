#include <chrono>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <ftw.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>


int main (int argc, char ** argv)
{
	if(2 != argc)
	{
		fprintf(stderr, "Error: Invalid arguments\n");
		exit(EXIT_FAILURE);
	} 

	char * args[] = { argv[1], (char*)NULL };
	if(0 > execv("./b", args))
	{
		perror("Error occurred while calling b");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
