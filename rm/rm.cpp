#include <chrono>
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


int del(char const * fpath, const struct stat * sb, int typeflag, struct FTW * ftwbuf)
{
        if (-1 == remove(fpath))
        {
                int errno_copy = errno;
                fprintf(stderr, "%s%s%s", "Error occured while deleting the file: ", fpath, "\n");
                fprintf(stderr, "%s\n", strerror(errno_copy));
                return FTW_SKIP_SUBTREE;
        }

        return 0;
}

int main(int argc, char** argv)
{
        std::vector<char const *> names;
        names.reserve(argc);
        bool recs = false;
        for (size_t i = 1; i < argc; i++)
        {
                if (false == recs && 0 == strcasecmp(argv[i], "-r"))
                {
                        recs = true;
                }
                else
                {
                        names.push_back(argv[i]);
                }
        }

        if (true == recs)
        {
                for (size_t i = 0; i < names.size(); i++)
                {
                        nftw(names[i], del, 1 << 10, FTW_DEPTH);
                }
        }
        else
        {
                for (size_t i = 0; i < names.size(); i++)
                {
                        nftw(names[i], del, 1 << 10, FTW_ACTIONRETVAL);
                }
        }


        exit(EXIT_SUCCESS);
}
