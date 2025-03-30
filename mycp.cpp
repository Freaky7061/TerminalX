#include "mycp.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

void mycp(const std::string& src, const std::string& dest) {
    int srcFd = open(src.c_str(), O_RDONLY);
    if (srcFd == -1) {
        perror("mycp (source)");
        return;
    }

    int destFd = open(dest.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (destFd == -1) {
        perror("mycp (destination)");
        close(srcFd);
        return;
    }

    char buffer[1024];
    ssize_t bytesRead;
    while ((bytesRead = read(srcFd, buffer, sizeof(buffer))) > 0) {
        if (write(destFd, buffer, bytesRead) != bytesRead) {
            perror("write");
            break;
        }
    }

    if (bytesRead == -1) {
        perror("read");
    }

    close(srcFd);
    close(destFd);
}
