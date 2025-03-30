# TerminalX
TerminalX - C++ Linux Shell 模拟器

# **TerminalX - C++ Linux Shell 模拟器**  

**一个轻量级、模块化的 Linux Shell 模拟器，使用 C++ 实现核心 Shell 功能，支持基础命令、进程管理及文件操作。**  

## 🚀 **项目功能**  

### **1. Shell 终端模拟**
- ✅ **自定义提示符**：显示 `[用户名@主机名 当前目录]$`，支持动态更新  
- ✅ **命令解析与执行**：支持多参数输入，错误处理（`Command not found`）  
- ✅ **进程管理**：基于 `fork-exec-waitpid` 机制执行外部命令  
- ✅ **后台执行**（可选）：支持 `&` 后台运行  
- ✅ **历史命令**（可选）：记录用户输入，支持 `↑/↓` 键调取  

### **2. 自定义文件操作命令**（基于系统调用实现）  
| 命令  | 功能 | 关键系统调用 |  
|-------|------|-------------|  
| `myls` | 列出目录内容 | `opendir` / `readdir` / `stat` |  
| `mycat` | 输出文件内容 | `open` / `read` / `write` |  
| `mycp` | 复制/移动文件 | `rename` / `open` / `read` / `write` |  
| `myrm` | 删除文件（支持 `-r` 递归删除） | `unlink` / `rmdir` |  
| `mytouch` | 创建/更新文件时间戳 | `open(O_CREAT)` / `utime` |  

### **3. 进阶功能（可选）**
- **管道（`|`）支持**：解析 `myls | grep txt`，使用 `pipe` + 多进程实现  
- **I/O 重定向**：支持 `>`、`<` 等重定向操作  

## 🔧 **技术栈**  
- **语言**: C++11/14  
- **系统调用**: `fork` / `exec` / `waitpid` / `pipe` / `dup2`  
- **文件操作**: `open` / `read` / `write` / `stat` / `opendir`  
- **工具链**: GCC/G++、Makefile、GDB 调试  

## 📂 **项目结构**  
```
TerminalX/  
├── src/  
│   ├── shell.cpp      # Shell 主循环与命令解析  
│   ├── commands/      # 自定义命令实现  
│   │   ├── myls.cpp  
│   │   ├── mycat.cpp  
│   │   └── ...  
│   └── utils.cpp      # 通用工具函数  
├── include/           # 头文件  
├── Makefile           # 自动化编译  
└── README.md          # 使用说明  
```

## 🎯 **适用场景**  
- **Linux 系统学习**：深入理解 Shell 工作原理  
- **C++ 系统编程实践**：掌握进程管理、文件 I/O、系统调用  
- **课程设计/毕业设计**：提供完整代码框架与文档参考  

## 📜 **开源协议**  
MIT License - 自由修改与分发，欢迎提交 Issue 或 PR！  

---

**附注**：本项目为《Unix与Linux系统》课程设计实现，完整文档见 [课程报告.docx](课程报告.docx)，下载后查看。
