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


#define PROMT_ERROR(msg, errno_backup) \
{ \
        int errno_backup = errno_backup; \
        fprintf(stderr, "%s", msg); \
        fprintf(stderr, "file: %s, line: %d\n", __FILE__, __LINE__); \
        perror(strerror(errno_backup)); \
}



int main(int argc, char** argv)
{
	if (argc != 3)
	{
		perror("Ambigious arguments count");
		exit(EXIT_FAILURE);
	}

	struct stat sb1;
	if (-1 == stat(argv[1], &sb1))
	{
		int errno_copy = errno;
		std::ostringstream err;
		err << "error occured while accessing the file: " << argv[1] << "\n";
		PROMT_ERROR(err.str().c_str(), errno_copy);
		exit(EXIT_FAILURE);
	}

	if (S_IFDIR == (sb1.st_mode  & S_IFMT))
	{
		perror("Currently recursive copy not supported. Source should not be directory.");
		return -1;
		//              exit(EXIT_FAILURE);
	}

	struct stat sb2;
	if (-1 == stat(argv[2], &sb2))
	{
		int errno_copy = errno;
		std::ostringstream err;
		err << "error occured while accessing the file: " << argv[2] << "\n";
		PROMT_ERROR(err.str().c_str(), errno_copy);
		exit(EXIT_FAILURE);
	}

	int copy_from = open(argv[1], O_RDONLY);
	if (copy_from < 0)
	{
		int errno_copy = errno;
		std::ostringstream err;
		err << "error occured during opening the file: " << argv[1] << "\n";
		PROMT_ERROR(err.str().c_str(), errno_copy);
		exit(EXIT_FAILURE);
	}


	char destination[1 << 16]{};

	if (S_IFDIR == (sb2.st_mode & S_IFMT))
	{
		strcpy(destination + strlen(destination), argv[2]);
		strcpy(destination + strlen(destination), "/");
		strcpy(destination + strlen(destination), basename(argv[1]));
	}

	int copy_to_flags = O_CREAT | O_WRONLY;
	int creat_mode = S_IRUSR | S_IWUSR;

	int copy_to = open(destination, copy_to_flags, creat_mode);
	if (copy_to < 0)
	{
		int errno_copy = errno;
		std::ostringstream err;
		err << "error occured during opening the file: " << argv[2] << "\n";
		PROMT_ERROR(err.str().c_str(), errno_copy);
	}



	// Start copying.
	int bytes_read = -1;
	constexpr int buff_size = 1 << 20;
	char buffer[buff_size];
	while ((bytes_read = read(copy_from, buffer, buff_size)) > 0)
	{
		int bytes_written = -1;
		if ((bytes_written = write(copy_to, buffer, bytes_read)) != bytes_read)
		{
			int errno_copy = errno;
			std::ostringstream err;
			err << "error occured during writing into the file: " << argv[2] << "\n";
			PROMT_ERROR(err.str().c_str(), errno_copy);

		}
	}
	
	if (close(copy_from) < 0)
	{
		int errno_copy = errno;
		std::ostringstream err;
		err << "error occured during closing the file: " << argv[1] << "\n";
		PROMT_ERROR(err.str().c_str(), errno_copy);
	}

	if (close(copy_to) < 0)
	{
		int errno_copy = errno;
		std::ostringstream err;
		err << "error occured during closing the file: " << argv[2] << "\n";
		PROMT_ERROR(err.str().c_str(), errno_copy);
	}
	exit(EXIT_SUCCESS);
}
