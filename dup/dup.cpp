#include <iostream>
#include <chrono>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>


void close_internal(int fd)
{
	if(close(fd) < 0)
	{
		perror("Error occured while closing file exclusive_file.log");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char** argv)
{
	int fd = open("exclusive_file.log", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	if(fd < 0)
	{
		perror("Error occured while opening file exclusive_file.log");
		exit(EXIT_FAILURE);
	}
	int res = write(fd, "First line", sizeof("First line") - 1);
	
	if(res < 0)
	{
		perror("Error occured while writing in file exclusive_file.log");
		close_internal(fd);
	}
	
	int new_fd = dup(fd);
	
	if(new_fd < 0)
	{
		perror(NULL);
		close_internal(fd);
	}
	
	res = write(new_fd, "Second line", sizeof("Second line") - 1);
	if(res < 0)
	{
		perror("Error occured while writing in file exclusive_file.log");
		close_internal(new_fd);
	}

	close_internal(new_fd);
	exit(EXIT_SUCCESS);
}
