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


#define PROMT_ERROR(msg, errno_backup) \
{ \
	int errno_backup = errno_backup; \
	fprintf(stderr, "%s", msg); \
	fprintf(stderr, "file: %s, line: %d\n", __FILE__, __LINE__); \
        perror(strerror(errno_backup)); \
}


int main(int argc, const char** argv)
{
	if(argc != 3)
	{
		perror("Ambigious arguments count");
		return EXIT_FAILURE;
	}
	int copy_from = open(argv[1], O_RDONLY);
	if(copy_from < 0)
	{
		int errno_copy = errno;
		std::ostringstream err;
                err << "error occured during opening the file: " << argv[1] << "\n";
		PROMT_ERROR(err.str().c_str(), errno_copy);
	}
	int copy_to_flags = O_CREAT | O_WRONLY;
	int creat_mode = S_IRUSR | S_IWUSR;
	int copy_to = open(argv[2], copy_to_flags, creat_mode);
	if(copy_to < 0)
	{
		int errno_copy = errno;
                std::ostringstream err;
                err << "error occured during opening the file: " << argv[2] << "\n";
		PROMT_ERROR(err.str().c_str(), errno_copy);
	}

	auto start_time = std::chrono::system_clock::now();

	// Truncating the file.
	/*struct stat st;
	stat(argv[1], &st);
	if(ftruncate(copy_to, st.st_size) < 0)
	{
		int errno_copy = errno;
        	std::ostringstream err;
	        err << "error occured during truncating the file: " << argv[2] << "\n";
        	PROMT_ERROR(err.str().c_str(), errno_copy);

	}*/

	// Start copying.
	int bytes_read = -1;
	constexpr const int buff_size = 131072;
	char buffer[buff_size];
	while((bytes_read = read(copy_from, buffer, buff_size)) > 0)
	{
		int bytes_written = -1;
		if((bytes_written = write(copy_to, buffer, bytes_read)) != bytes_read )
		{
	                int errno_copy = errno;
        	        std::ostringstream err;
                	err << "error occured during writing into the file: " << argv[2] << "\n";
			PROMT_ERROR(err.str().c_str(), errno_copy);

		}
	}

	std::cout << "elapsed time: " << (std::chrono::system_clock::now() - start_time).count()
		<< "ns\n";

	if(close(copy_from) < 0)
	{
		int errno_copy = errno;
                std::ostringstream err;
                err << "error occured during closing the file: " << argv[1] << "\n";
                PROMT_ERROR(err.str().c_str(), errno_copy);
	}
        if(close(copy_to) < 0)
        {
                int errno_copy = errno;
                std::ostringstream err;
                err << "error occured during closing the file: " << argv[2] << "\n";
                PROMT_ERROR(err.str().c_str(), errno_copy);
        }
	


}
