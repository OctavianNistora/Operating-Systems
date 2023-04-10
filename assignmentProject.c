#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

void displayFile(char* argv, struct stat buffer)
{
    int first=0, last=strlen(argv)-1;
    while (argv[last]=='/')
    {
        last--;
    }
    for (int j=0; j<last; j++)
    {
        if (argv[j]=='/')
        {
            first=j+1;
        }
    }
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

void displayMenu()
{
    printf("---- MENU -----\n");
    printf("   • n: name\n");
    printf("   • d: size\n");
    printf("   • h: hard link count\n");
    printf("   • m: time of last modification\n");
    printf("   • a: access rights\n");
    printf("   • l: create symbolic link\n");
    printf("Please enter your options\n\n");
}

void selectedOptions(char *argv, struct stat buffer)
{
    int i=1;
    char options[8];

    while (1)
    {
        i=0;
        printf("STANDARD INPUT : ");
        scanf("%7s", options);
        if (options[i]!='-')
        {
            continue;
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
            break;
        }
        printf("Wrong input\n");
    }
    putchar('\n');
    i=1;

    if (options[i]=='n')
    {
        printf("\e[1mName of file:\e[0m %s\n", argv);
        i++;
    }
    
    if (options[i]=='d')
    {
        printf("\e[1mSize of file:\e[0m %ld bytes\n", buffer.st_size);
        i++;
    }

    if (options[i]=='h')
    {
        printf("\e[1mHard link count:\e[0m %ld\n", buffer.st_nlink);
        i++;
    }

    if (options[i]=='m')
    {
        char string[101];
        strftime(string, sizeof(string), "%d/%m/%y %T", gmtime(&buffer.st_mtime));
        printf("\e[1mTime of last modification:\e[0m %s\n", string);
        i++;
    }

    if (options[i]=='a')
    {
        printf("\e[1mAccess rights:\e[0m\n");
        for (int k=0; k<3; k++)
        {
            if (k==0)
            {
                printf("  \e[4mUser:\e[0m\n");
            }
            else if (k==1)
            {
                printf("  \e[4mGroup:\e[0m\n");
            }
            else
            {
                printf("  \e[4mOthers:\e[0m\n");
            }
            
            if (buffer.st_mode & (S_IRUSR>>k*3))
            {
                printf("   Read - yes\n");
            }
            else
            {
                printf("   Read - no\n");
            }
            if (buffer.st_mode & (S_IWUSR>>k*3))
            {
                printf("   Write - yes\n");
            }
            else
            {
                printf("   Write - no\n");
            }
            if (buffer.st_mode & (S_IXUSR>>k*3))
            {
                printf("   Exec - yes\n");
            }
            else
            {
                printf("   Exec - no\n");
            }
        }
        i++;
    }

    if (options[i]=='l')
    {
        char link[101];
        printf("Please give the link name:\n");
        printf("STANDARD INPUT: ");
        scanf("%100s", link);
        if (symlink(argv, link))
        {
            printf("STANDARD OUTPUT: the link was not created\n");
        }
        else
        {
            printf("STANDARD OUTPUT: the link was created\n");
        }
    }
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

void printFileName(char *file)
{
    int first=0, last=strlen(file)-1;
    while (file[last]=='/')
    {
        last--;
    }
    for (int i=0; i<last; i++)
    {
        if (file[i]=='/')
        {
            first=i+1;
        }
    }
    printf("\e[1mName of directory:\e[0m %.*s\n", last-first+1, file+first);
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

void displayDirectory(char *dir)
{
    char options[6];
    int i=0;
    while (1)
    {
        i=0;
        scanf("%5s", options);
        if (options[i]!='-')
        {
            continue;
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
        if (options[i]=='\0')
        {
            break;
        }
    }
    i=1;
    struct stat dirStat;
    lstat(dir, &dirStat);
    if (options[i]=='n')
    {
        printFileName(dir);
        i++;
    }
    if (options[i]=='d')
    {
        printf("\e[1mSize of directory:\e[0m %ld bytes\n", dirStat.st_size);
        i++;
    }
    if (options[i]=='a')
    {
        char string[256];
        strftime(string, sizeof(string), "%d/%m/%y %T", gmtime(&dirStat.st_atime));
        printf("\e[1mTime of last access:\e[0m %s\n", string);
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
        printf("\e[1mNumber of C files in directory:\e[0m %d\n", totalCFiles(string));
        i++;
    }
}

int main(int argc, char* argv[])
{
    /*
    struct stat buffer;
    for (int i=1; i<argc; i++)
    {
        if (lstat(argv[i], &buffer)<0)
        {
            printf("Wrong file or not a file\n");
            continue;
        }

        displayFile(argv[i], buffer);

        displayMenu();

        selectedOptions(argv[i], buffer);

        printf("\n\n");
    }
    return 0;
    */


    for (int i=1; i<argc; i++)
    {
        if (!isDirectory(argv[i]))
        {
            printf("%s is not a directory\n", argv[i]);
            continue;
        }
        displayDirectory(argv[i]);
    }
    return 0;
    
}