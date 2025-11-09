
/*
 * 
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <glob.h>
#include <wordexp.h>
#include <vector>
#include <string>
#include <cctype>
#include <iostream>
#include <sstream>
#include "command.h"
#include "tokenizer.h"


void parse(std::vector<Token> &tokens)
{
    Command &cmd = Command::_currentCommand; 
    SimpleCommand *&sc = Command::_currentSimpleCommand;
    

    size_t i = 0;
    while (tokens[i].type != TOKEN_EOF) {
        Token& token = tokens[i];

        if (token.type == TOKEN_COMMAND) {
            sc = new SimpleCommand(); // New simple command
            sc->insertArgument(strdup(token.value.c_str())); // Add the command as the first argument (convert from string to char*)
            
            i++; // Jump to the arguments

            // Loop to consume all the arguments
            while (tokens[i].type == TOKEN_ARGUMENT) {
                sc->insertArgument(strdup(tokens[i].value.c_str()));
                i++;
            }

            cmd.insertSimpleCommand(sc); // Add the new simple command
            
        }
        else if (cmd._numberOfSimpleCommands > 0) {
            if (token.type == TOKEN_REDIRECT) {
                i++;
                cmd._append = 0;
                if (tokens[i].type == TOKEN_ARGUMENT) {
                    cmd._outFile = strdup(tokens[i].value.c_str());
                }
                else {
                    fprintf(stderr, "Error: missing output file after '>'\n");
                    cmd.clear();
                    return;
                }
            }
            else if (token.type == TOKEN_APPEND) {
                i++;
                cmd._append = 1;
                if (tokens[i].type == TOKEN_ARGUMENT) {
                    cmd._outFile = strdup(tokens[i].value.c_str());
                }
                else {
                    fprintf(stderr, "Error: missing output file after '>>'\n");
                    cmd.clear();
                    return;
                }
            }  
            else if (token.type == TOKEN_ERROR) {
                i++;
                cmd._out_error = 1;
                if (tokens[i].type == TOKEN_ARGUMENT) {
                    cmd._errFile = strdup(tokens[i].value.c_str());
                }
                else {
                    fprintf(stderr, "Error: missing error file after '2>'\n");
                    cmd.clear();
                    return;
                }
            }
            else if (token.type == TOKEN_REDIRECT_AND_ERROR) {
                i++;
                cmd._append = 1;
                cmd._out_error = 1;
                if (tokens[i].type == TOKEN_ARGUMENT) {
                    char* file = strdup(tokens[i].value.c_str());
                    cmd._outFile = file;
                    cmd._errFile = file;
                }
                else {
                    fprintf(stderr, "Error: missing output/error file after '&>>'\n");
                    cmd.clear();
                    return;
                }
            }
            else if (token.type == TOKEN_INPUT) {
                i++;
                if (tokens[i].type == TOKEN_ARGUMENT) {
                    cmd._inputFile = strdup(tokens[i].value.c_str());;
                }
                else {
                    fprintf(stderr, "Error: missing input file after '<'\n");
                    cmd.clear();
                    return;
                }
            }
            else if (token.type == TOKEN_BACKGROUND) { 
                cmd._background = 1;
            } 
            else if (token.type != TOKEN_PIPE) {
                fprintf(stderr, "Error: unexpected token '%s'\n", token.value.c_str());
                cmd.clear();
                return;
            }

            i++;  
        }
    }

    cmd.execute();
}

SimpleCommand::SimpleCommand()
{
    _numberOfAvailableArguments = 5;
    _numberOfArguments = 0;
    _arguments = (char **)malloc(_numberOfAvailableArguments * sizeof(char *));
}

void SimpleCommand::insertArgument(char *argument)
{
    if (_numberOfAvailableArguments == _numberOfArguments + 1)
    {
        _numberOfAvailableArguments *= 2;
        _arguments = (char **)realloc(_arguments,
                                      _numberOfAvailableArguments * sizeof(char *));
    }

    _arguments[_numberOfArguments] = argument;
    _arguments[_numberOfArguments + 1] = NULL;

    _numberOfArguments++;
}

void SimpleCommand::expandWildcards() {
    std::vector<char*> expandedArgs;
    expandedArgs.push_back(strdup(_arguments[0]));

    for (int i = 1; i < _numberOfArguments; i++) {
            const char *arg = _arguments[i];

            if (strchr(arg, '*') || strchr(arg, '?')) {
                glob_t globbuf;
                memset(&globbuf, 0, sizeof(glob_t));

                // Expand
                if (glob(arg, GLOB_NOCHECK, NULL, &globbuf) == 0) {
                    for (size_t j = 0; j < globbuf.gl_pathc; j++) {
                        expandedArgs.push_back(strdup(globbuf.gl_pathv[j]));
                    }
                }
                globfree(&globbuf);
            } else {
                expandedArgs.push_back(strdup(arg));
            }
        }

        // Free old arguments
        for (int i = 0; i < _numberOfArguments; i++) {
            free(_arguments[i]);
        }
        free(_arguments);

        // Build new NULL-terminated array
        _arguments = (char **)malloc((expandedArgs.size() + 1) * sizeof(char *));
        for (size_t i = 0; i < expandedArgs.size(); i++) {
            _arguments[i] = expandedArgs[i];
        }
        _arguments[expandedArgs.size()] = NULL;

        // Update argument count
        _numberOfArguments = expandedArgs.size();
}


Command::Command()
{
    _numberOfAvailableSimpleCommands = 1;
    _simpleCommands = (SimpleCommand **)
        malloc(_numberOfAvailableSimpleCommands * sizeof(SimpleCommand *));

    _numberOfSimpleCommands = 0;
    _outFile = 0;
    _inputFile = 0;
    _errFile = 0;
    _background = 0;
}

void Command::insertSimpleCommand(SimpleCommand *simpleCommand)
{
    if (_numberOfAvailableSimpleCommands == _numberOfSimpleCommands)
    {
        _numberOfAvailableSimpleCommands *= 2;
        _simpleCommands = (SimpleCommand **)realloc(_simpleCommands,
                                                    _numberOfAvailableSimpleCommands * sizeof(SimpleCommand *));
    }

    _simpleCommands[_numberOfSimpleCommands] = simpleCommand;
    _numberOfSimpleCommands++;
}

void Command::clear()
{
    for (int i = 0; i < _numberOfSimpleCommands; i++)
    {
        for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++)
        {
            free(_simpleCommands[i]->_arguments[j]);
        }

        free(_simpleCommands[i]->_arguments);
        free(_simpleCommands[i]);
    }


    if (_outFile == _errFile && _outFile ==_inputFile)
    {
        free(_outFile);
        _outFile = 0;
        _errFile = 0;
        _inputFile = 0;
    }
    else if (_outFile == _errFile) {
        free(_outFile);
        _outFile = 0;
        _errFile = 0;
        if (_inputFile) {
            free(_inputFile);
            _inputFile = 0;
        }
    }
    else if (_outFile == _inputFile) {
        free(_outFile);
        _outFile = 0;
        _inputFile = 0;
        if (_errFile) {
            free(_errFile);
            _errFile = 0;
        }
    }
    else if (_inputFile == _errFile) {
        free(_inputFile);
        _inputFile = 0;
        _errFile = 0;
        if (_outFile) {
            free(_outFile);
            _outFile = 0;
        }
    }
    else {
        free(_inputFile);
        free(_outFile);
        free(_errFile);
        _outFile = 0;
        _inputFile = 0;
        _errFile = 0;
    }

    _numberOfSimpleCommands = 0;
    _background = 0;
}

void Command::print()
{
    printf("\n\n");
    printf("              COMMAND TABLE                \n");
    printf("\n");
    printf("  #   Simple Commands\n");
    printf("  --- ----------------------------------------------------------\n");

    for (int i = 0; i < _numberOfSimpleCommands; i++)
    {
        printf("  %-3d ", i);
        for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++)
        {
            printf("\"%s\" \t", _simpleCommands[i]->_arguments[j]);
        }
    }

    printf("\n\n");
    printf("  Output       Input        Error        Err&Out       Background\n");
    printf("  ------------ ------------ ------------ ------------ ------------\n");
    printf("  %-12s %-12s %-12s %-12s %-12s\n", _outFile ? _outFile : "default",
           _inputFile ? _inputFile : "default", _errFile ? _errFile : "default", _out_error == 1 ? _errFile : "default"
           ,_background ? "YES" : "NO");
    printf("\n\n");
}


void sigint_handler(int sig) {
    write(STDOUT_FILENO, "\nmyshell>", strlen("\nmyshell>"));
}

void sigchld_handler(int sig) {
    int saved_errno = errno; 
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        FILE *log = fopen("log.txt", "a+");
        if (log) {
            time_t now = time(nullptr);
            char *time_str = ctime(&now);
            time_str[strlen(time_str) - 1] = '\0';
            fprintf(log, "[%s] Child with PID %d terminated with status %d\n", time_str, pid, WEXITSTATUS(status));
            fclose(log);
        }
    }
    errno = saved_errno; 
}

void Command::execute()
{
    print();

    Command &cmd = Command::_currentCommand; 
    SimpleCommand *&sc = Command::_currentSimpleCommand;

    sc->expandWildcards();

    int num_cmds = cmd._numberOfSimpleCommands;
    int fd[2]; // file descriptors (read & write)
    int in_fd = 0; // Defines input stream for next command/process

    if (cmd._inputFile) {
        in_fd = open(cmd._inputFile, O_RDONLY);
        if (in_fd < 0) {
            perror("open input file");
            clear();
            prompt();
            return;
        }
    }

    for (int i = 0; i < num_cmds; i++) {
        sc = *(cmd._simpleCommands + i);

        // Handle 'exit' command
        if (strcmp(sc->_arguments[0], "exit") == 0) {
            // printf("Good bye!!\n");
            clear();
            exit(0);
        }

        // Handle 'cd' command
        if (strcmp(sc->_arguments[0], "cd") == 0) {
            const char *path;
            if (sc->_numberOfArguments == 1) {
                path = getenv("HOME"); 
            }
            else if (sc->_numberOfArguments == 2) {
                path = sc->_arguments[1];
            }
            else {
                perror("Too many arguments");
                break;
            }

            printf("Changing to directory '%s'\n", path);
            if (chdir(path) != 0) {
                perror("cd failed");
            }
            else {
                char cwd[1024];
                if (getcwd(cwd, sizeof(cwd)) != nullptr)
                    printf("You are now in %s\n", cwd);
            }
            break;
        }

        // Handle 'pwd' command
        if (strcmp(sc->_arguments[0], "pwd") == 0) {
            if (sc->_numberOfArguments == 1) {
                char cwd[1024];
                if (getcwd(cwd, sizeof(cwd)) != nullptr)
                    printf("%s\n", cwd);
                else
                    perror("pwd failed");
                }
            else {
                perror("Too many arguments");
            }
            break;
        }

        // Create a pip for all except last command
        if (i < num_cmds - 1) {
            if (pipe(fd) == -1) {
                perror("pipe failed");
                break;
            }
        }

        pid_t pid;
        pid = fork();
        
        // Child process
        if (pid == 0) {
            signal(SIGINT, SIG_DFL); // Child process don't ignore ctrl+c

            if (in_fd != 0) {
                dup2(in_fd, 0); // Redirect "input source" to STDIN
                close(in_fd);
            }

            // If last command, check for redirections
            if (i == num_cmds - 1){
                int out_fd = -1, err_fd = -1;

                // Case 1: stdout + stderr redirection (only append mode (>>&))
                if (cmd._outFile && cmd._out_error) {
                    int flags = O_WRONLY | O_CREAT | O_APPEND;
                    out_fd = open(cmd._outFile, flags, 0666);
                    if (out_fd < 0) { perror("open output&err"); exit(1); }
                    dup2(out_fd, 1); 
                    dup2(out_fd, 2);
                    close(out_fd);
                }
                // Case 2: stderr redirection (only write mode (2>))
                else if (cmd._errFile && cmd._out_error) {
                    int flags = O_WRONLY | O_CREAT | O_TRUNC;
                    err_fd = open(cmd._errFile, flags, 0666);
                    if (err_fd < 0) { perror("open err"); exit(1); }
                    dup2(err_fd, 2);
                    close(err_fd);
                }
                // Case 3: stdout redirection (write or append)
                else if (cmd._outFile) {
                    int flags = O_WRONLY | O_CREAT | (cmd._append ? O_APPEND : O_TRUNC);
                    out_fd = open(cmd._outFile, flags, 0666);
                    if (out_fd < 0) { perror("open output"); exit(1); }
                    dup2(out_fd, 1);
                    close(out_fd);
                }
            }

            // Not last command
            else {
                dup2(fd[1], 1); // Redirect "write-file-descriptor" to STDOUT
                close(fd[0]);
                close(fd[1]);
            }

            // Execute command
            if (execvp(sc->_arguments[0], sc->_arguments) == -1) {
                perror("execvp failed");
                exit(1);
            }
        }
        // Parent process
        else if (pid > 0) {
            if (!cmd._background) {
                int status;
                if (waitpid(pid, &status, 0) != pid) {
                    perror("waitpid failed");
                    break;
                }
                else {
                    FILE *log = fopen("log.txt", "a");
                    if (log) {
                        time_t now = time(nullptr);
                        char *time_str = ctime(&now);
                        time_str[strlen(time_str) - 1] = '\0';
                        fprintf(log, "[%s] Child with PID %d terminated with status %d\n", time_str, pid, WEXITSTATUS(status));
                        fclose(log);
                    }
                }
            }

            if (in_fd != 0) {
                close(in_fd);
            }

            if (i < num_cmds - 1) {
                close(fd[1]);
                in_fd = fd[0];
            }
        }
        // Process failure
        else {
            perror("Couldn't execute!\n");
            break;
        }
    }

    
    clear();
    prompt();
}

void Command::prompt()
{
    printf("\nmyshell>");
    fflush(stdout);
    std::string input;
    while (true)
    {
        std::getline(std::cin, input); 
        // fprintf(stdout, "%s\n", strdup(input.c_str()));
        std::vector<Token> tokens = tokenize(input);
        parse(tokens);
    }
}

Command Command::_currentCommand;
SimpleCommand *Command::_currentSimpleCommand;

int main()
{   
    // Ignore Ctrl+C
    signal(SIGINT, sigint_handler);

    // Log child when terminated
    signal(SIGCHLD, sigchld_handler);

    Command::_currentCommand.prompt();
    return 0;
}
