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


#define PROMT_ERROR(msg, errno_backup) \
{ \
        int errno_backup = errno_backup; \
        fprintf(stderr, "%s", msg); \
        fprintf(stderr, "file: %s, line: %d\n", __FILE__, __LINE__); \
        fprintf(stderr, "%s\n", strerror(errno_backup)); \
}

void close_internal(int fd)
{
	if(close(fd) < 0)
	{
		perror("Error occured while closing file new_pts_0");
		exit(EXIT_FAILURE);
	}
}

int do_magic()
{
	int fd = open("new_pts_0", O_CREAT | O_APPEND | O_RDWR, S_IRUSR | S_IWUSR);
	if(fd < 0)
	{
		perror("Error while opening file new_pts_0");
		exit(EXIT_FAILURE);
	}

	if(dup2(fd, STDIN_FILENO) < 0 || dup2(fd, STDOUT_FILENO) < 0)
		close_internal(fd);
	return fd;

}


int main(int argc, char** argv)
{	
	int fd = do_magic();
	std::string s;
	std::cin >> s;
	std::cout << s;
	close_internal(fd);
	exit(EXIT_SUCCESS);
}
