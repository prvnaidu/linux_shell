#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <sstream>
#include <fcntl.h>
#include <signal.h>
using namespace std;
void handle_sigint(int )
{
    
}
void handle_sigtstp(int )
{
    
}
void reapBackgroundProcesses()
{
    pid_t finished;
    int status;
    while (0 < (finished = waitpid(-1, &status, WNOHANG)))
    {
        cout << "Background process "
             << finished
             << " finished\n";
    }
}
vector<string> tokenize(string command)
{
    stringstream ss(command);

    vector<string> tokens;
    string word;
    while (ss >> word)
    {
        tokens.push_back(word);
    }
    return tokens;
}
void createargs(vector<string> &tokens, vector<char *> &args)
{
    for (auto &it : tokens)
    {
        args.push_back(const_cast<char *>(it.c_str()));
    }
    args.push_back(NULL);
}
bool handleCD(vector<string> &tokens)
{
    if (tokens.size() == 0)
        return true;
    if (tokens[0] != "cd")
        return false;
    if (tokens.size() < 2)
    {
        cout << "cd: missing argument\n";
    }
    else
    {
        if (chdir(tokens[1].c_str()) != 0)
        {
            perror("cd");
        }
    }
    return true;
}
vector<vector<string>> commandseparator(vector<string> tokens)
{
    vector<vector<string>> commands;
    vector<string> curcommand;
    for (size_t i = 0; i < tokens.size(); i++)
    {
        if (tokens[i] == "|")
        {
            commands.push_back(curcommand);
            curcommand.clear();
        }
        else
        {
            curcommand.push_back(tokens[i]);
        }
    }
    commands.push_back(curcommand);
    return commands;
}
vector<vector<int>> createpipes(vector<vector<string>> &commands)
{
    vector<vector<int>> pipes;
    for (size_t i = 0; i < commands.size() - 1; i++)
    {
        int pipefd[2];
        if (pipe(pipefd) == -1)
        {
            perror("pipe");
            continue;
        }
        pipes.push_back({pipefd[0], pipefd[1]});
    }
    return pipes;
}
void handlepipes(vector<vector<int>> &pipes, vector<vector<string>> &commands)
{
    for (size_t i = 0; i < commands.size(); i++)
    {
        pid_t pid = fork();

        if (pid == 0)
        {

            if (i > 0)
            {
                dup2(pipes[i - 1][0], 0);
            }
            if (i < commands.size() - 1)
            {
                dup2(pipes[i][1], 1);
            }
            for (size_t j = 0; j < pipes.size(); j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            vector<char *> cmdArgs;

            for (auto &s : commands[i])
            {
                cmdArgs.push_back(const_cast<char *>(s.c_str()));
            }

            cmdArgs.push_back(NULL);
            execvp(cmdArgs[0], cmdArgs.data());
            perror("execvp");
            return;
        }
    }
}
void execute_command(vector<string> tokens)
{
    bool redirectOutput = false, appendoutput = false;
    bool redirectinput = false;
    bool background = false;
    if (tokens.back() == "&")
    {
        background = true;
        tokens.pop_back();
    }
    string filename;
    if (tokens.size() >= 3 &&
        tokens[tokens.size() - 2] == ">")
    {
        redirectOutput = true;
        filename = tokens.back();
        tokens.pop_back();
        tokens.pop_back();
    }
    if (tokens.size() >= 3 &&
        tokens[tokens.size() - 2] == ">>")
    {
        appendoutput = true;
        filename = tokens.back();
        tokens.pop_back();
        tokens.pop_back();
    }
    if (tokens.size() >= 3 &&
        tokens[tokens.size() - 2] == "<")
    {
        redirectinput = true;
        filename = tokens.back();
        tokens.pop_back();
        tokens.pop_back();
    }
    vector<char *> args;
    createargs(tokens, args);
    pid_t pid = fork();
    if (pid == 0)
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        if (redirectOutput)
        {
            int fd = open(filename.c_str(),
                          O_WRONLY | O_CREAT | O_TRUNC,
                          0644);

            if (fd == -1)
            {
                perror("open");
                return;
            }

            dup2(fd, 1);
            close(fd);
        }
        if (appendoutput)
        {
            int fd = open(filename.c_str(),
                          O_WRONLY | O_CREAT | O_APPEND,
                          0644);

            if (fd == -1)
            {
                perror("open");
                return;
            }

            dup2(fd, 1);
            close(fd);
        }
        if (redirectinput)
        {
            int fd = open(filename.c_str(), O_RDONLY);

            if (fd == -1)
            {
                perror("open");
                return;
            }

            dup2(fd, 0);
            close(fd);
        }
        execvp(args[0], args.data());
        perror("Command failed");
        return;
    }
    if (!background)
    {
        wait(NULL);
    }
}
bool hasPipe(vector<string> &tokens)
{
    for (auto &t : tokens)
    {
        if (t == "|")
            return true;
    }
    return false;
}
void execute_pipeline(vector<string> tokens)
{
    vector<vector<string>> commands = commandseparator(tokens);
    vector<vector<int>> pipes = createpipes(commands);
    handlepipes(pipes, commands);
    // closing all the pipes
    for (size_t j = 0; j < pipes.size(); j++)
    {
        close(pipes[j][0]);
        close(pipes[j][1]);
    }
    // waitng for all the children
    for (size_t i = 0; i < commands.size(); i++)
    {
        wait(NULL);
    }
}
int main()
{
    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigtstp);
    while (true)
    {
        reapBackgroundProcesses();
        cout << "myshell> ";
        string command;
        getline(std::cin, command);
        if (command == "")
            continue;
        else if (command == "exit")
            break;
        vector<string> tokens = tokenize(command);
        if (handleCD(tokens))
        {
            continue;
        }
        if (hasPipe(tokens))
        {
            execute_pipeline(tokens);
            continue;
        }
        execute_command(tokens);
    }

    return 0;
}
