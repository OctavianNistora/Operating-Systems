#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <stdlib.h>

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
        char string[256];
        strftime(string, sizeof(string), "%d/%m/%y %T", gmtime(&buffer.st_atime));
        strcat(temp, "\e[1mAccess rights:\e[0m\n");
        strcat(temp, string);
        strcat(temp, "\n");
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
    printf("%s", temp);
}

int main(int argc, char* argv[])
{
    pid_t pid[200];
    int iF=0, iS=0;
    char options[100][9], symlink[100][101];

    for (int i=1; i<argc; i++)
    {
        struct stat buffer;
        if (lstat(argv[i], &buffer)<0)
        {
            printf("Wrong file or not a file\n");
            continue;
        }
        
        displayFile(argv[i], buffer);

        if (S_ISREG(buffer.st_mode))
        {
            do
            {
                displayMenuRegular();
                scanf("%8s", options[iF]);
            }while(!verifyRegularOptions(options[iF]));
            if (strchr(options[iF], 'l'))
            {
                printf("Please give the link name:\n");
                printf("STANDARD INPUT: ");
                scanf("%100s", symlink[iS]);
                iS++;
            }
            printf("\n");
            iF++;
        }

        else if (S_ISDIR(buffer.st_mode))
        {
            do
            {
                displayMenuDirectory();
                scanf("%6s", options[iF]);
                printf("\n");
            }while(!verifyDirectoryOptions(options[iF]));
            iF++;
        }
    }

    int jF=0, jS=0, iP=0;
    for (int i=1; i<argc; i++)
    {
        struct stat buffer;
        if (lstat(argv[i], &buffer)<0)
        {
            printf("Wrong file or not a file\n");
            continue;
        }
        if (S_ISREG(buffer.st_mode))
        {
            if ((pid[iP]=fork())<0)
            {
                printf("Fork failed\n");
                exit(1);
            }
            if (pid[iP]==0)
            {
                interactRegular(argv[i], buffer, options[jF], symlink[jS]);
                exit(0);
            }
            iP++;

            if (strchr(options[jF], 'l'))
            {
                jS++;
            }
            jF++;

            if (strstr(argv[i]+strlen(argv[i])-2, ".c"))
            {
                if ((pid[iP]=fork())<0)
                {
                    printf("Fork failed\n");
                    exit(1);
                }
                if (pid[iP]==0)
                {
                    execlp("./regularCFileScript.sh", "./regularCFileScript.sh", argv[i], NULL);
                }
                iP++;
            }
        }

        else if (S_ISDIR(buffer.st_mode))
        {
            if ((pid[iP]=fork())<0)
            {
                printf("Fork failed\n");
                exit(1);
            }
            if (pid[iP]==0)
            {
                interactDirectory(argv[i], buffer, options[jF]);
                exit(0);
            }
            jF++;
            iP++;

            if ((pid[iP]=fork())<0)
            {
                printf("Fork failed\n");
                exit(1);
            }
            if (pid[iP]==0)
            {
                char textFileName[256];
                int first, last;
                printFileName(argv[i], &first, &last);
                strncpy(textFileName, argv[i]+first, last-first+1);
                strcat(textFileName, "_file.txt");
                execlp("touch", "touch", textFileName, NULL);
            }
            iP++;
        }
    }

    for (int i=0; i<iF; i++)
    {
        waitpid(pid[i], NULL, 0);
    }
    return 0;
    
}