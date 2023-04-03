#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

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
        printf("STANDARD INPUT : ");
        scanf("%7s", options);
        if (options[0]=='-')
        {
            break;
        }
        printf("Wrong input\n");
    }
    putchar('\n');

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

int main(int argc, char* argv[])
{
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
}