#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>

void printFileName(char *file, int* first, int* last)
{
    *first=0;
    *last=strlen(file)-1;
    while (file[*last]=='/')
    {
        (*last)--;
    }
    for (int i=0; i<*last; i++)
    {
        if (file[i]=='/')
        {
            *first=i+1;
        }
    }
}

void displayFile(char* argv, struct stat buffer)
{
    int first, last;
    printFileName(argv, &first, &last);
    printf("%.*s - ", last-first+1, argv+first);

    if (S_ISREG(buffer.st_mode))
    {
        printf("REGULAR FILE\n");
    }
    else if (S_ISDIR(buffer.st_mode))
    {
        printf("DIRECTORY\n");
    }
    else if (S_ISLNK(buffer.st_mode))
    {
        printf("SYMBOLIC LINK\n");
    }
    else
    {
        printf("OTHER FILE\n");
    }   
}

void displayMenuRegular()
{
    printf("---- MENU -----\n");
    printf("   • n: name\n");
    printf("   • d: size\n");
    printf("   • h: hard link count\n");
    printf("   • m: time of last modification\n");
    printf("   • a: access rights\n");
    printf("   • l: create symbolic link\n");
    printf("Please enter your options:\n");
}

void displayMenuSymbolic()
{
    printf("---- MENU -----\n");
    printf("   • n: name\n");
    printf("   • l: delete symbolic link\n");
    printf("   • d: size of symbolic link\n");
    printf("   • t: size of target file\n");
    printf("   • a: access rights\n");
    printf("Please enter your options:\n");
}

void displayMenuDirectory()
{
    printf("---- MENU -----\n");
    printf("   • n: name\n");
    printf("   • d: size\n");
    printf("   • a: access rights\n");
    printf("   • c: number of .C files in directory\n");
    printf("Please enter your options:\n");
}

int verifyRegularOptions(char* options)
{
    int i=0;
    if (options[i]!='-')
    {
        return 0;
    }
    i++;
    if (options[i]=='n')
    {
        i++;
    }
    if (options[i]=='d')
    {
        i++;
    }
    if (options[i]=='h')
    {
        i++;
    }
    if (options[i]=='m')
    {
        i++;
    }
    if (options[i]=='a')
    {
        i++;
    }
    if (options[i]=='l')
    {
        i++;
    }
    if (options[i]=='\0' && i!=1)
    {
        return 1;
    }
    return 0;
}

int verifySymbolicOptions(char* options)
{
    int i=0;
    if (options[i]!='-')
    {
        return 0;
    }
    i++;
    if (options[i]=='n')
    {
        i++;
    }
    if (options[i]=='l')
    {
        i++;
    }
    if (options[i]=='d')
    {
        i++;
    }
    if (options[i]=='t')
    {
        i++;
    }
    if (options[i]=='a')
    {
        i++;
    }
    if (options[i]=='\0' && i!=1)
    {
        return 1;
    }
    return 0;
}

int verifyDirectoryOptions(char* options)
{
    int i=0;
    if (options[i]!='-')
    {
        return 0;
    }
    i++;
    if (options[i]=='n')
    {
        i++;
    }
    if (options[i]=='d')
    {
        i++;
    }
    if (options[i]=='a')
    {
        i++;
    }
    if (options[i]=='c')
    {
        i++;
    }
    if (options[i]=='\0' && i!=1)
    {
        return 1;
    }
    return 0;
}

void interactRegular(char *argv, struct stat buffer, char options[], char *symName)
{
    int i=1;

    char temp[4096];
    strcpy(temp, "");

    if (options[i]=='n')
    {
        strcat(temp, "\e[1mName of file:\e[0m ");
        int first, last;
        printFileName(argv, &first, &last);
        strncat(temp, argv+first, last-first+1);
        strcat(temp, "\n");
        i++;
    }
    
    if (options[i]=='d')
    {
        strcat(temp, "\e[1mSize of file:\e[0m ");
        char size[100];
        sprintf(size, "%ld", buffer.st_size);
        strcat(temp, size);
        strcat(temp, " bytes\n");
        i++;
    }

    if (options[i]=='h')
    {
        strcat(temp, "\e[1mHard link count:\e[0m ");
        char link[100];
        sprintf(link, "%ld", buffer.st_nlink);
        strcat(temp, link);
        strcat(temp, "\n");
        i++;
    }

    if (options[i]=='m')
    {
        char string[101];
        strftime(string, sizeof(string), "%d/%m/%y %T", gmtime(&buffer.st_mtime));
        strcat(temp, "\e[1mTime of last modification:\e[0m ");
        strcat(temp, string);
        strcat(temp, "\n");
        i++;
    }

    if (options[i]=='a')
    {
        strcat(temp, "\e[1mAccess rights:\e[0m\n");
        for (int k=0; k<3; k++)
        {
            if (k==0)
            {
                strcat(temp, "  \e[4mUser:\e[0m\n");
            }
            else if (k==1)
            {
                strcat(temp, "  \e[4mGroup:\e[0m\n");
            }
            else
            {
                strcat(temp, "  \e[4mOthers:\e[0m\n");
            }
            
            if (buffer.st_mode & (S_IRUSR>>k*3))
            {
                strcat(temp, "   Read - yes\n");
            }
            else
            {
                strcat(temp, "   Read - no\n");
            }

            if (buffer.st_mode & (S_IWUSR>>k*3))
            {
                strcat(temp, "   Write - yes\n");
            }
            else
            {
                strcat(temp, "   Write - no\n");
            }

            if (buffer.st_mode & (S_IXUSR>>k*3))
            {
                strcat(temp, "   Exec - yes\n");
            }
            else
            {
                strcat(temp, "   Exec - no\n");
            }
        }
        i++;
    }

    if (options[i]=='l')
    {
        if (symlink(argv, symName))
        {
            strcat(temp, "STANDARD OUTPUT: the link was not created\n");
        }
        else
        {
            strcat(temp, "STANDARD OUTPUT: the link was created\n");
        }
    }
    strcat(temp, "\n");
    printf("%s", temp);
}

int isDirectory(char *dir)
{
    struct stat dirStat;
    lstat(dir, &dirStat);
    if (S_ISDIR(dirStat.st_mode))
    {
        return 1;
    }
    return 0;
}

int totalCFiles(char *dir)
{
    int count=0;
    DIR *directory=opendir(dir);
    struct dirent *entry;
    while ((entry=readdir(directory))!=NULL)
    {
        int i=0;
        while (entry->d_name[i]!='\0')
        {
            i++;
        }
        if (entry->d_name[i-1]=='c' && entry->d_name[i-2]=='.')
        {
            count++;
        }

        if (strcmp(entry->d_name, ".")!=0 && strcmp(entry->d_name, "..")!=0)
        {
            char potentialDirectory[256];
            strcpy(potentialDirectory, dir);
            strcat(potentialDirectory, "/");
            strcat(potentialDirectory, entry->d_name);
            struct stat buffer;
            lstat(potentialDirectory, &buffer);
            if (S_ISDIR(buffer.st_mode))
            {
                count+=totalCFiles(potentialDirectory);
            }
        }
    }
    closedir(directory);
    return count;
}

void interactSymbolic(char *argv, struct stat buffer, char options[])
{
    int i=1;

    char temp[4096];
    strcpy(temp, "");
    if (options[i]=='n')
    {
        strcat(temp, "\e[1mName of symbolic link:\e[0m ");
        int first, last;
        printFileName(argv, &first, &last);
        strncat(temp, argv+first, last-first+1);
        strcat(temp, "\n");
        i++;
    }

    if (options[i]=='l')
    {
        remove(argv);
        strcat(temp, "STANDARD OUTPUT: the link was removed\n");
        printf("%s", temp);
        return;
    }

    if (options[i]=='d')
    {
        strcat(temp, "\e[1mSize of symbolic link:\e[0m ");
        char size[100];
        sprintf(size, "%ld", buffer.st_size);
        strcat(temp, size);
        strcat(temp, " bytes\n");
        i++;
    }

    if (options[i]=='t')
    {
        struct stat auxBuffer;
        stat(argv, &auxBuffer);
        strcat(temp, "\e[1mSize of target file:\e[0m ");
        char size[100];
        sprintf(size, "%ld", auxBuffer.st_size);
        strcat(temp, size);
        strcat(temp, " bytes\n");
        i++;
    }

    if (options[i]=='a')
    {
        strcat(temp, "\e[1mAccess rights:\e[0m\n");
        for (int k=0; k<3; k++)
        {
            if (k==0)
            {
                strcat(temp, "  \e[4mUser:\e[0m\n");
            }
            else if (k==1)
            {
                strcat(temp, "  \e[4mGroup:\e[0m\n");
            }
            else
            {
                strcat(temp, "  \e[4mOthers:\e[0m\n");
            }
            
            if (buffer.st_mode & (S_IRUSR>>k*3))
            {
                strcat(temp, "   Read - yes\n");
            }
            else
            {
                strcat(temp, "   Read - no\n");
            }
            if (buffer.st_mode & (S_IWUSR>>k*3))
            {
                strcat(temp, "   Write - yes\n");
            }
            else
            {
                strcat(temp, "   Write - no\n");
            }
            if (buffer.st_mode & (S_IXUSR>>k*3))
            {
                strcat(temp, "   Exec - yes\n");
            }
            else
            {
                strcat(temp, "   Exec - no\n");
            }
        }
        i++;
    }
    strcat(temp, "\n");
    printf("%s", temp);
}

void interactDirectory(char *dir, struct stat buffer, char options[])
{
    int i=1;
    
    char temp[4096];
    strcpy(temp, "");

    if (options[i]=='n')
    {
        strcat(temp, "\e[1mName of directory:\e[0m ");
        int first, last;
        printFileName(dir, &first, &last);
        strncat(temp, dir+first, last-first+1);
        strcat(temp, "\n");
        i++;
    }

    if (options[i]=='d')
    {
        strcat(temp, "\e[1mSize of directory:\e[0m ");
        char size[100];
        sprintf(size, "%ld", buffer.st_size);
        strcat(temp, size);
        strcat(temp, " bytes\n");
        i++;
    }

    if (options[i]=='a')
    {
        strcat(temp, "\e[1mAccess rights:\e[0m\n");
        for (int k=0; k<3; k++)
        {
            if (k==0)
            {
                strcat(temp, "  \e[4mUser:\e[0m\n");
            }
            else if (k==1)
            {
                strcat(temp, "  \e[4mGroup:\e[0m\n");
            }
            else
            {
                strcat(temp, "  \e[4mOthers:\e[0m\n");
            }
            
            if (buffer.st_mode & (S_IRUSR>>k*3))
            {
                strcat(temp, "   Read - yes\n");
            }
            else
            {
                strcat(temp, "   Read - no\n");
            }

            if (buffer.st_mode & (S_IWUSR>>k*3))
            {
                strcat(temp, "   Write - yes\n");
            }
            else
            {
                strcat(temp, "   Write - no\n");
            }

            if (buffer.st_mode & (S_IXUSR>>k*3))
            {
                strcat(temp, "   Exec - yes\n");
            }
            else
            {
                strcat(temp, "   Exec - no\n");
            }
        }
        i++;
    }

    if (options[i]=='c')
    {
        char string[256];
        strcpy(string, dir);
        int n=strlen(string);
        while (string[n-1]=='/')
        {
            string[n-1]='\0';
            n--;
        }
        strcat(temp, "\e[1mNumber of C files in directory:\e[0m ");
        char number[100];
        sprintf(number, "%d", totalCFiles(string));
        strcat(temp, number);
        strcat(temp, "\n");
        i++;
    }
    strcat(temp, "\n");
    printf("%s", temp);
}

int main(int argc, char* argv[])
{
    pid_t pid[2];
    char options[9], symlink[101];
    int gradeFile=creat("gradeFile.txt", 0777);

    for (int i=1; i<argc; i++)
    {
        struct stat buffer;
        if (lstat(argv[i], &buffer)<0)
        {
            printf("Wrong file error: or not a file\n");
            continue;
        }
        
        displayFile(argv[i], buffer);

        if (S_ISREG(buffer.st_mode))
        {
            do
            {
                displayMenuRegular();
                scanf("%8s", options);
            }while(!verifyRegularOptions(options));

            if (strchr(options, 'l'))
            {
                printf("Please give the link name:\n");
                printf("STANDARD INPUT: ");
                scanf("%100s", symlink);
            }
            printf("\n");

            if ((pid[0]=fork())<0)
            {
                printf("fork failed\n");
                exit(1);
            }
            else if (pid[0]==0)
            {
                interactRegular(argv[i], buffer, options, symlink);
                exit(0);
            }

            if (strstr(argv[i]+strlen(argv[i])-2, ".c"))
            {
                int pipefd[2];
                if (pipe(pipefd)<0)
                {
                    printf("pipe failed\n");
                    exit(2);
                }
                if ((pid[1]=fork())<0)
                {
                    printf("fork failed\n");
                    exit(1);
                }
                if (pid[1]==0)
                {
                    close(pipefd[0]);
                    dup2(pipefd[1], 1);
                    close(pipefd[1]);
                    execlp("./regularCFileScript.sh", "./regularCFileScript.sh", argv[i], NULL);
                    printf("script failed\n");
                    exit(3);
                }
                close(pipefd[1]);

                pid_t waitPid;
                int status;
                for (int i=0; i<2; i++)
                {
                    waitPid=wait(&status);
                    if(WIFEXITED(status))
                    {
                        printf("The process with PID %d has ended with the exit code %d\n", waitPid, WEXITSTATUS(status));
                    }
                }
                printf("\n\n\n\n");

                char buffer[21];
                int i=0, erros, warnings;
                while (read(pipefd[0], &buffer[i], 1)>0)
                {
                    if (buffer[i]=='\n')
                    {
                        buffer[i]='\0';
                        erros=strtol(buffer, NULL, 10);
                        i=0;
                        break;
                    }
                    i++;
                }
                while (read(pipefd[0], &buffer[i], 1)>0)
                {
                    if (buffer[i]=='\n')
                    {
                        buffer[i]='\0';
                        warnings=strtol(buffer, NULL, 10);
                        break;
                    }
                    i++;
                }

                int score;
                if (erros>0)
                {
                    score=1;
                }
                else if (warnings>10)
                {
                    score=2;
                }
                else if (warnings>0)
                {
                    score=2+8*(10-warnings)/10;
                }
                else
                {
                    score=10;
                }

                char scoreString[101];
                int first, last;
                printFileName(argv[i], &first, &last);
                strncpy(scoreString, argv[i]+first, last-first+1);
                strcat(scoreString, ": ");
                if (score==10)
                {
                    strcat(scoreString, "10");
                }
                else
                {
                    char digit[3];
                    digit[0]=score+'0';
                    digit[1]='\n';
                    digit[2]='\0';
                    strcat(scoreString, digit);
                }
                write(gradeFile, scoreString, strlen(scoreString));
                continue;
            }
            else
            {
                if ((pid[1]=fork())<0)
                {
                    printf("fork failed\n");
                    exit(1);
                }
                if (pid[1]==0)
                {
                    execlp("grep", "grep", "-c", """",  argv[i], NULL);
                    printf("grep failed\n");
                    exit(3);
                }
            }
        }

        else if (S_ISLNK(buffer.st_mode))
        {
            do
            {
                displayMenuSymbolic();
                scanf("%8s", options);
                printf("\n");
            }while(!verifySymbolicOptions(options));

            if ((pid[0]=fork())<0)
            {
                printf("fork failed\n");
                exit(1);
            }
            else if (pid[0]==0)
            {
                interactSymbolic(argv[i], buffer, options);
                exit(0);
            }

            if (!strchr(options, 'l'))
            {
                if ((pid[1]=fork())<0)
                {
                    printf("fork failed\n");
                    exit(1);
                }
                if (pid[1]==0)
                {
                    execlp("chmod", "chmod", "0760", argv[i], NULL);
                    printf("chmod failed\n");
                    exit(3);
                }
            }
        }

        else if (S_ISDIR(buffer.st_mode))
        {
            do
            {
                displayMenuDirectory();
                scanf("%8s", options);
                printf("\n");
            }while(!verifyDirectoryOptions(options));

            if ((pid[0]=fork())<0)
            {
                printf("fork failed\n");
                exit(1);
            }
            else if (pid[0]==0)
            {
                interactDirectory(argv[i], buffer, options);
                exit(0);
            }

            if ((pid[1]=fork())<0)
            {
                printf("fork failed\n");
                exit(1);
            }
            if (pid[1]==0)
            {
                char textFileName[256];
                int first, last;
                printFileName(argv[i], &first, &last);
                strncpy(textFileName, argv[i]+first, last-first+1);
                strcat(textFileName, "_file.txt");
                execlp("touch", "touch", textFileName, NULL);
                printf("touch failed\n");
                exit(3);
            }
        }

        pid_t waitPid;
        int status;
        for (int i=0; i<2; i++)
        {
            waitPid=wait(&status);
            if(WIFEXITED(status))
            {
                printf("The process with PID %d has ended with the exit code %d\n", waitPid, WEXITSTATUS(status));
            }
        }

        printf("\n\n\n\n");
    }
    return 0;
    
}

//Laurentiu ; Claudiu