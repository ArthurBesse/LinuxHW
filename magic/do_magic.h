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


void do_magic()
{
        int fd = open("new_pts_0", O_CREAT | O_APPEND | O_RDWR, S_IRUSR | S_IWUSR);
        if(fd < 0)
        {
                perror("Error while opening file new_pts_0");
                exit(EXIT_FAILURE);
        }

        if(dup2(fd, STDIN_FILENO) < 0 || dup2(fd, STDOUT_FILENO) < 0)
        {
		if(0 > close(fd))
			perror("Error occured while closing file new_pts_0");
		exit(EXIT_FAILURE);
	}
    
}
