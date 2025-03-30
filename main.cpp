#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <pwd.h>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include "mycp.h"

std::vector<std::string> history;  // 保存历史命令
size_t history_index = 0;  // 当前显示的历史命令索引

// 自定义命令实现
void myls(const std::string& path = ".") {
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        perror("myls");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;

        struct stat st;
        std::string fullPath = path + "/" + name;
        if (stat(fullPath.c_str(), &st) == -1) {
            perror("stat");
            continue;
        }

        std::cout << (S_ISDIR(st.st_mode) ? "d" : "-") << name << "\n";
    }

    closedir(dir);
}

void mycat(const std::string& filename) {
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
        perror("mycat");
        return;
    }

    char buffer[1024];
    ssize_t bytesRead;
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, bytesRead);
    }

    if (bytesRead == -1) {
        perror("read");
    }

    close(fd);
}

// 自定义命令处理
void handleCustomCommand(const std::vector<std::string>& args) {
    if (args.empty()) return;

    if (args[0] == "myls") {
        myls(args.size() > 1 ? args[1] : ".");
    } else if (args[0] == "mycat") {
        if (args.size() < 2) {
            std::cerr << "Usage: mycat <filename>\n";
        } else {
            mycat(args[1]);
        }
    } else if (args[0] == "mycp") {
        if (args.size() < 3) {
            std::cerr << "Usage: mycp <source> <destination>\n";
        } else {
            mycp(args[1], args[2]);
        }
    } else {
        std::cerr << "Unknown command: " << args[0] << std::endl;
    }
}

void handleCdCommand(const std::vector<std::string>& args) {
    if (args.size() == 1) {
        // 切换到用户主目录
        const char* home = getenv("HOME");
        if (!home) {
            std::cerr << "cd: HOME environment variable not set" << std::endl;
        } else if (chdir(home) != 0) {
            perror("cd");
        }
    } else if (args.size() == 2) {
        // 切换到指定路径
        if (chdir(args[1].c_str()) != 0) {
            perror("cd");
        }
    } else {
        // 参数过多
        std::cerr << "cd: too many arguments" << std::endl;
    }
}

// 获取提示符信息
std::string getUsername() {
    struct passwd* pw = getpwuid(getuid());
    return pw ? pw->pw_name : "user";
}

std::string getHostname() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return std::string(hostname);
    }
    return "hostname";
}

std::string getCwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        return std::string(cwd);
    }
    return "/unknown";
}

void showPrompt() {
    std::cout << "[" << getUsername() << "@" << getHostname() << " " << getCwd() << "]$ ";
}

// 命令解析
std::vector<std::string> parseCommand(const std::string& input) {
    std::vector<std::string> args;
    size_t pos = 0, found;
    while ((found = input.find_first_of(' ', pos)) != std::string::npos) {
        args.push_back(input.substr(pos, found - pos));
        pos = found + 1;
    }
    if (pos < input.length()) {
        args.push_back(input.substr(pos));
    }
    return args;
}

// 处理历史命令
void handleHistoryInput(const std::string& input) {
    if (!input.empty()) {
        // 只有非空命令才保存到历史
        if (history.empty() || history.back() != input) {
            history.push_back(input);
        }
        history_index = history.size();  // 新命令时重置索引
    }
}

// 处理键盘输入
void handleArrowKeys(char c) {
    if (c == 27) {  // ESC
        char next = getchar();  // [
        if (next == '[') {
            next = getchar();  // A 或 B
            if (next == 'A') {  // 向上箭头
                if (history_index > 0) {
                    history_index--;
                    std::cout << "\r" << history[history_index];
                    std::flush(std::cout);
                }
            } else if (next == 'B') {  // 向下箭头
                if (history_index < history.size()) {
                    history_index++;
                    std::cout << "\r" << (history_index == history.size() ? "" : history[history_index]);
                    std::flush(std::cout);
                }
            }
        }
    }
}

// Main函数
int main() {
    while (true) {
        showPrompt();
        std::string input;
        char c;

        while (true) {
            c = getchar();
            if (c == '\n') {
                std::cout << std::endl;
                break;
            } else if (c == 27) {  // 检测箭头键
                handleArrowKeys(c);
            } else {
                input.push_back(c);
                std::cout << c;
                std::flush(std::cout);
            }
        }

        if (input.empty()) continue;
        if (input == "exit") break;

        // 处理历史命令
        handleHistoryInput(input);

        auto args = parseCommand(input);

        if (args.empty()) continue; // 确保命令有效

        // 特殊处理 `cd` 命令
        if (args[0] == "mycd") {
            handleCdCommand(args);
        } 
        // 处理自定义命令
        else if (args[0] == "myls" || args[0] == "mycat" || args[0] == "mycp") {
            handleCustomCommand(args);
        } 
        // 检查系统命令
        else {
            // 检查命令是否存在于系统路径中
            if (system(nullptr)) { // 确保系统支持 system 调用
                std::string command = "command -v " + args[0] + " > /dev/null 2>&1";
                if (system(command.c_str()) != 0) {
                    std::cerr << "Unknown command: " << args[0] << std::endl;
                    continue;
                }
            } else {
                std::cerr << "System command execution not supported." << std::endl;
                continue;
            }

            // 执行系统命令
            pid_t pid = fork();
            if (pid == 0) { // 子进程
                std::vector<char*> c_args;
                for (const auto& arg : args) {
                    c_args.push_back(const_cast<char*>(arg.c_str()));
                }
                c_args.push_back(nullptr);

                execvp(c_args[0], c_args.data());
                perror("execvp"); // 如果执行失败，打印错误
                exit(EXIT_FAILURE);
            } else if (pid > 0) { // 父进程
                int status;
                waitpid(pid, &status, 0);
            } else {
                perror("fork");
            }
        }
    }
    return 0;
}

