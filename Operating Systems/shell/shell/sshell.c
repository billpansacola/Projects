#include <stdio.h>
// for exit()
#include <stdlib.h>
#include <sys/types.h>

// for fork() and exec() function
#include <unistd.h>

// for waitpid()
#include <sys/wait.h>

#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_LINE_CHARS 512
#define MAX_LINE_ARGS 16

struct CommandArgs
{
    char *command;
    char **args;
    int inputRedirection;
    int outputRedirection;
    char *file;
    int pipeline;
    int error;
    int missingcommanderror;
    int background_process;
};

enum
{
    /*
        Enum containing possible errors listed in phase 2
    */
    TOO_MANY_ARGS,
    NO_SUCH_DIR,
    COMMAND_NOT_FOUND,
    MISSING_COMMAND,
    MISLOCATED_INPUT_REDIRECTION,
    NO_INPUT_FILE,
    CANNOT_OPEN_INPUT_FILE,
    MISLOCATED_OUTPUT_REDIRECTION,
    NO_OUTPUT_FILE,
    CANNOT_OPEN_OUTPUT_FILE,
    MISLOCATED_BACKGROUND_SIGN,
    ACTIVE_JOBS_RUNNING
};

void errorMessages(int errorType)
{
    /*
        Function that prints out the error messages.
        errorType can be implemented once we get the other functions working.
    */
    switch (errorType)
    {
    case TOO_MANY_ARGS:
        fprintf(stderr, "Error: too many process arguments\n");
        break;
    case NO_SUCH_DIR:
        fprintf(stderr, "Error: no such directory\n");
        break;
    case COMMAND_NOT_FOUND:
        fprintf(stderr, "Error: command not found\n");
        break;
    case MISSING_COMMAND:
        fprintf(stderr, "Error: missing command\n");
        break;
    case MISLOCATED_INPUT_REDIRECTION:
        fprintf(stderr, "Error: mislocated input redirection\n");
        break;
    case NO_INPUT_FILE:
        fprintf(stderr, "Error: no input file\n");
        break;
    case CANNOT_OPEN_INPUT_FILE:
        fprintf(stderr, "Error: cannot open input file\n");
        break;
    case MISLOCATED_OUTPUT_REDIRECTION:
        fprintf(stderr, "Error: mislocated output redirection\n");
        break;
    case NO_OUTPUT_FILE:
        fprintf(stderr, "Error: no output file\n");
        break;
    case CANNOT_OPEN_OUTPUT_FILE:
        fprintf(stderr, "Error: cannot open output file\n");
        break;
    case MISLOCATED_BACKGROUND_SIGN:
        fprintf(stderr, "Error: mislocated background sign\n");
        break;
    case ACTIVE_JOBS_RUNNING:
        fprintf(stderr, "Error: active jobs still running\n");
        break;
    }
}

// Phase 7
int inputPipeline(char *input, int i)
{
    char *pPosition = strchr(input, '|');
    // if it is NULL
    if (!pPosition)
    {
        return -1;
    }
    else
    {
        // use read function
        return i;
    }
}

// Phase 5 & 6
int IO_Redirection(char *input, int i)
{
    // check if the input string has either the '>' or '<' to signify redirection
    if (!strchr(input, '<') && !strchr(input, '>'))
    {
        return -1;
    }
    else
    {
        return i;
    }
}

void IO_RedirectionOpenFile(struct CommandArgs obj, int newfd)
{
    // Checks if there is an input redirection symbol
    if (obj.inputRedirection != -1)
    {
        // open a new file descriptor with the file
        newfd = open(obj.file, O_RDWR);
        // Checks if successfully opened file and replaces with std input for input redirection.
        if (newfd == -1)
        {
            // Error Management
            close(newfd);
            if (!strcmp(obj.file, ""))
            {
                errorMessages(NO_INPUT_FILE);
            }
            else
            {
                errorMessages(CANNOT_OPEN_INPUT_FILE);
            }
            exit(2);
        }
        else
        {
            dup2(newfd, STDIN_FILENO);
            close(newfd);
        }
    }
    else if (obj.outputRedirection != -1)
    {
        // open a new file descriptor with the file
        newfd = open(obj.file, O_CREAT | O_TRUNC | O_WRONLY);
        if (newfd == -1)
        {
            // Error Management
            close(newfd);
            if (!strcmp(obj.file, ""))
            {
                errorMessages(NO_OUTPUT_FILE);
            }
            else
            {
                errorMessages(CANNOT_OPEN_OUTPUT_FILE);
            }
            exit(2);
        }
        else
        {
            dup2(newfd, STDOUT_FILENO);
            close(newfd);
        }
    }
}

struct CommandArgs *parseInput(char *input)
{
    char newString[512];
    strcpy(newString, input);

    int n = strlen(input);
    int original_length = strlen(input);
    int kc = 0;
    // Inserting spaces before and after special chars

    int background_process = 0;
    int background_sign_error = 0;
    for (int i = 0; i < n; i++)
    {
        if (input[i] == '<' || input[i] == '|' || input[i] == '>' || input[i] == '&')
        {
            // inserts before char and after the char
            for (int j = n - 1; j >= kc; j--)
            {
                newString[j + 1] = newString[j];
            }
            newString[kc] = ' ';

            for (int j = n + 1; j >= kc + 1; j--)
            {
                newString[j + 1] = newString[j];
            }
            newString[kc + 2] = ' ';
            newString[n + 2] = '\0';

            kc += 3;
            n = n + 4;
        }
        else
        {
            kc++;
        }

        if (input[i] == '&')
        {
            if (i != original_length - 2)
            {
                errorMessages(MISLOCATED_BACKGROUND_SIGN);
                background_sign_error = 1;
            }
            else
            {
                background_process = 1;
            }
        }
    }

    strcpy(input, newString);

    // need the \t for white spaces
    char delim[] = " \t\n";
    //int init_size = strlen(input);

    char *ptr = strtok(input, delim);

    // creating object

    struct CommandArgs *commands = malloc(sizeof(struct CommandArgs) * MAX_LINE_ARGS);
    for (int obji = 0; obji < MAX_LINE_ARGS; obji++)
    {
        commands[obji].command = "";
        commands[obji].args = malloc(sizeof(char *) * MAX_LINE_ARGS);
        commands[obji].inputRedirection = -1;
        commands[obji].outputRedirection = -1;
        commands[obji].file = "";
        commands[obji].error = 0;
        commands[obji].missingcommanderror = -1;
        if (background_sign_error)
        {
            commands[obji].error = 1;
            background_sign_error = 0;
        }
        else
        {
            commands[obji].error = 0;
        }
        commands[obji].background_process = background_process;
    }

    // i is the index of strings
    int i = 0;

    // j will count the number of pipelines
    int j = 0;
    // k is index of arguments
    int k = 0;

    int indexOfPipeline = 0;
    while (ptr != NULL)
    {

        // checks if there is a '<' within a string
        // Adding struct

        if (inputPipeline(ptr, i) != -1)
        {
            indexOfPipeline = i;

            // j++;
            // i = 0;
        }
        else if (!strcmp("&", ptr))
        {
        }
        // if there was a space
        else if (indexOfPipeline + 1 == i && indexOfPipeline != 0)
        {
            j++;
            k = 0;
            commands[j].command = ptr;
            commands[j].args[k] = ptr;

            k++;
        }
        else if (i == 0)
        {

            commands[j].args[k] = ptr;
            commands[j].command = ptr;
            k++;
        }

        else if (IO_Redirection(ptr, i) != -1)
        {
            // checks if < is on its own string
            if (!strcmp(ptr, "<"))
            {
                commands[j].inputRedirection = i;
            }
            else
            {
                commands[j].outputRedirection = i;
            }
        }
        // obtain the file name proceeding the inputredirection
        else if (commands[j].inputRedirection + 1 == i)
        {
            // Is there a pipe in the file name.
            char *addressOfPipe = strchr(ptr, '|');
            int indexOfPipe = (addressOfPipe == NULL ? -1 : addressOfPipe - ptr);
            if (indexOfPipe == -1)
            {
                commands[j].file = ptr;
            }
            else
            {
                char *subbuff = malloc(indexOfPipe);
                memcpy(subbuff, &ptr[0], indexOfPipe);
                subbuff[indexOfPipe] = '\0';
                commands[j].file = subbuff;
            }
        }
        else if (commands[j].outputRedirection + 1 == i)
        {
            // Is there a pipe in the file name.
            char *addressOfPipe = strchr(ptr, '|');
            int indexOfPipe = (addressOfPipe == NULL ? -1 : addressOfPipe - ptr);
            if (indexOfPipe == -1)
            {
                commands[j].file = ptr;
            }
            else
            {
                char *subbuff = malloc(indexOfPipe);
                memcpy(subbuff, &ptr[0], indexOfPipe);
                subbuff[indexOfPipe] = '\0';
                commands[j].file = subbuff;
            }
        }
        else
        {
            commands[j].args[k] = ptr;
            k++;
        }

        ptr = strtok(NULL, delim);
        i++;
        if (!strcmp("", commands[j].command) || !strcmp("|", commands[j].command) || !strcmp("&", commands[j].command))
        {
            commands[j].missingcommanderror = 1;
        }
    }

    // checks how many arguments there are
    int numOfArgs = i;
    if (numOfArgs > 16)
    {
        errorMessages(TOO_MANY_ARGS);
        commands[j].error = 1;
    }

    return commands;
}

void pwd()
{
    // arguements for getcwd()
    size_t size = MAX_LINE_CHARS;
    char *buffer = NULL;
    char *working_directory = getcwd(buffer, size);
    fprintf(stdout, "%s\n", working_directory);
}
void pipelineCommand(struct CommandArgs *obj, int numCommands)
{
    // create 2 array for read and write of pipe
    int fd[2];

    int input = 0;

    for (int i = 0; i < numCommands - 1; i++)
    {
        // Pipe
        pipe(fd);

        struct CommandArgs *proceedingObj = obj + i;
        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process

            if (fd[1] != STDOUT_FILENO)
            {

                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
            }
            if (input != STDIN_FILENO)
            {
                dup2(input, STDIN_FILENO);
                close(input);
            }
            int newfd = -1;

            if (obj[i].inputRedirection != -1 && i > 0)
            {
                errorMessages(MISLOCATED_INPUT_REDIRECTION);
                exit(2);
            }
            if (obj[i].outputRedirection != -1 && i != numCommands - 1)
            {
                errorMessages(MISLOCATED_OUTPUT_REDIRECTION);
                exit(2);
            }
            IO_RedirectionOpenFile(obj[i], newfd);

            if (execvp(proceedingObj->command, proceedingObj->args) == -1)
            {
                errorMessages(COMMAND_NOT_FOUND);
                exit(1);
            }
        }

        // Close the write end of the pipe of the last process
        close(fd[1]);
        // Need to read from the pipe
        input = fd[0];
    }

    // Last command- set stdin be the read end of the previous pipe
    // and output to the original file descriptor 1.
    int lastIndex = numCommands - 1;
    if (input != STDIN_FILENO)
        dup2(input, STDIN_FILENO);

    // print if there is the wront input redirection
    if (obj[lastIndex].inputRedirection != -1 && lastIndex > 0)
    {
        errorMessages(MISLOCATED_INPUT_REDIRECTION);
        exit(2);
    }

    // runing the last command with the current process.
    if (execvp(obj[lastIndex].command, obj[lastIndex].args) == -1)
    {
        errorMessages(COMMAND_NOT_FOUND);
        exit(1);
    }
}

int main(int argc, char *argv[])
{

    // Creating fork and pid for child
    while (-1 == -1)
    {
        pid_t pid;

        char *input = NULL;
        char origInput[MAX_LINE_CHARS];

        int numCommands = 0;
        printf("sshell$ ");
        // Initialize to null so getline can handle allocation. This fixed segmentation fault errors.
        char text[MAX_LINE_CHARS];

        // Never exceeds 512 characters
        fgets(text, MAX_LINE_CHARS, stdin);

        if (!isatty(STDIN_FILENO))
        {
            printf("%s", text);
            fflush(stdout);
        }
        input = text;
        strcpy(origInput, input);

        struct CommandArgs *obj;

        obj = parseInput(input);

        int missingCommandError = -1;

        for (int i = 0; i < sizeof(obj); i++)
        {

            if (strcmp(obj[i].command, ""))
            {
                numCommands++;
            }

            if (obj[i].missingcommanderror == 1)
            {
                missingCommandError = 1;
                break;
            }
        }


        if (missingCommandError == 1)
        {
            errorMessages(MISSING_COMMAND);
            continue;
        }

        if (!strcmp(obj[0].command, ""))
        {
            continue;
        }
        if (obj[0].error == 1)
        {
            continue;
        }

        // Max 16 arguments from 5.3.1

        int chdirError;

        if (!strcmp(obj[0].command, "exit"))
        {
            // Phase 4
            fprintf(stderr, "Bye...\n");
            exit(0);
        }
        else if (!strcmp(obj[0].command, "pwd"))
        {
            pwd();
        }
        else if (!strcmp(obj[0].command, "cd"))
        {
            chdirError = chdir(obj[0].args[1]);
            if (chdirError == -1)
            {
                errorMessages(NO_SUCH_DIR);
            };
        }

        int newfd = -1;

        pid = fork();

        // for execvp, first arg should point to the filename associated with the file being executed
        int status;

        if (pid == 0)
        {
            // This is the child
            // execv. want to use execvp for PATH Environment variable

            // This function creates and replaces the STDIN file descriptor with the file on inputRedirection.
            if (numCommands > 1)
            {
                pipelineCommand(obj, numCommands);
            }
            else
            {
                IO_RedirectionOpenFile(obj[0], newfd);

                // We need this exit function for these functions to properly obtain status.
                if (!strcmp(obj[0].command, "pwd"))
                {
                    exit(0);
                }
                else if (!strcmp(obj[0].command, "cd"))
                {
                    if (chdirError == -1)
                    {
                        exit(1);
                    };
                    exit(0);
                }
                if (obj[0].background_process)
                {
                    waitpid(-1, &status, WNOHANG);
                }
                if (execvp(obj[0].command, obj[0].args) == -1)
                {
                    errorMessages(COMMAND_NOT_FOUND);
                    exit(1);
                }
                perror("execvp");
                exit(1);
            }
            // run exec here
        }
        else if (pid > 0)
        {
            if (obj[0].background_process)
            {
                //waitpid(-1, &status, WNOHANG);
            }
            else
            {

                waitpid(-1, &status, 0);
                strtok(origInput, "\n");
                if (WEXITSTATUS(status) != 2)
                {
                    fprintf(stderr, "+ completed '%s' [%d]\n", origInput, WEXITSTATUS(status));
                }
            }
        }
        else
        {
            perror("There was an error with fork.");
        }
    }
    return 0;
}
