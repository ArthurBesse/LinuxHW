#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>


int main(int argc, char** argv)
{
    if(2 != argc)
    {
        fprintf(stderr, "Invalid arguments\n");
        exit(EXIT_FAILURE);
    }
    std::vector<std::string> v{std::istream_iterator<std::string>{std::cin}, std::istream_iterator<std::string>{}};
    int fork_res = fork();
    if(0 > fork_res)
    {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    if(0 == fork_res)
    {
        char ** arg_list = new char * [v.size() + 2];
        arg_list[v.size() + 1] = (char*) NULL;
        for (int i = 0; i < v.size(); ++i) {
            arg_list[i + 1] = new char[v[i].size()]{};
            strcpy(arg_list[i + 1], v[i].c_str());
        }

        arg_list[0] = new char[strlen(argv[1]) + 1]{};
        strcpy(arg_list[0], argv[1]);

        int exec_res = execvp(argv[1], arg_list);
        if(0 > exec_res)
        {
            perror(NULL);
            exit(EXIT_FAILURE);
        }
    }
    else
        wait(NULL);
    exit(EXIT_SUCCESS);
}
