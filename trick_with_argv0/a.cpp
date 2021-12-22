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
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
#include <logger.h>

int main (int argc, char ** argv)
{
	Logger logger(NULL);

	if(2 != argc)
	{
		Logger::logf(Logger::ERROR, __FILE__, __LINE__, "Error: Invalid arguments\n");
		exit(EXIT_FAILURE);
	}

	char * args[] = { argv[1], (char*)NULL };
	int pid = fork();

	if(0 > pid)
	{
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	if(0 == pid)
	{
		if(0 > execv("./b", args))
		{
			perror("Error occurred while calling b");
			exit(EXIT_FAILURE);
		}
	}
	else
		wait(NULL);

	exit(EXIT_SUCCESS);
}
