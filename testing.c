#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int pipe1[2];
    int pid;

    pipe(pipe1);
    if ((pid=fork())<0)
    {
        perror("Child process creation error\n");
	    exit(1);
    }
    else if (pid==0)
    {
        close(pipe1[0]);
        dup2(pipe1[1], 1);
        close(pipe1[1]);
        execlp("./regularCFileScript.sh", "./regularCFileScript.sh", "assignmentProject.c", NULL);
    }
    else
    {
        close(pipe1[1]);
        dup2(pipe1[0], 0);
        char input[101];
        int i=0;
        while (read(0, &input[i], 1) > 0)
        {
            if (input[i]=='\n')
            {
                input[i]='\0';
                printf("%s C\n", input);
                i=0;
                continue;
            }
            i++;
        }
        close(pipe1[0]);
    }
    return 0;
}