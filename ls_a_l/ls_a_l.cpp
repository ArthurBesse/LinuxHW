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


int main(int argc, char const ** argv)
{
	int pid = fork();
	if(0 > pid)
	{
		perror(nullptr);
		exit(EXIT_FAILURE);
	}
	if(0 == pid)
	{
		if(0 > execlp("ls", "-a", "-l", (char*)nullptr))
		{
			perror(nullptr);
			exit(EXIT_FAILURE);
		}
	}
	else
		wait(nullptr);
	exit(EXIT_SUCCESS);
}
