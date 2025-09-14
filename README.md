# Advanced Windows Shell (C Implementation)

![Shell Banner](https://img.shields.io/badge/Language-C-blue.svg) ![Platform](https://img.shields.io/badge/Platform-Windows-green.svg) ![Status](https://img.shields.io/badge/Status-Complete-brightgreen.svg)

## 📋 Overview

This project is a **feature-rich custom shell for Windows**, implemented entirely in C. It provides an interactive command-line interface with modern shell features including command history, aliases, I/O redirection, pipelines, colorized output, and much more.

The shell combines the simplicity of traditional command-line interfaces with advanced features typically found in modern shells like bash or PowerShell.

## 🎯 Project Structure

```
Advanced-Windows-Shell/
├── src/
│   └── main.c           # Complete shell implementation
├── README.md            # This documentation
├── LICENSE              # Project license
└── docs/
    └── examples.md      # Usage examples and tutorials
```

## ✨ Features

### 🔹 **Core Shell Functionality**
- **Command Execution**: Run any Windows system command with full argument support
- **Interactive Prompt**: Beautiful colored prompt showing `username@directory>`
- **Error Handling**: Comprehensive error management with helpful messages
- **Cross-compiler Support**: Works with GCC, MinGW, and Microsoft Visual Studio

### 🔹 **Built-in Commands**
| Command | Description | Example |
|---------|-------------|---------|
| `cd <directory>` | Change current directory | `cd Documents` |
| `help` | Display help and available commands | `help` |
| `exit` | Exit the shell gracefully | `exit` |
| `history` | Show command history (up to 100) | `history` |
| `clear` | Clear the console screen | `clear` |
| `color <1-7>` | Change console text color | `color 4` |
| `alias <name> <cmd>` | Create command shortcuts | `alias ll dir` |

### 🔹 **Advanced Features**

#### 📚 **Command History**
- Stores up to **100 previous commands**
- **`!!`** command to repeat the last executed command
- Automatic history rotation when limit is reached
- Memory-efficient implementation with proper cleanup

#### 🎯 **Alias System**
- Create custom command shortcuts
- Support for multiple syntax formats:
  - `alias name command`
  - `alias name=command`
- Pre-loaded aliases: `ll` and `ls` → `dir`
- View all aliases with `alias` command

#### 📤 **I/O Redirection**
- **Output Redirection**: `command > file.txt`
- **Input Redirection**: `command < input.txt`
- **Append Mode**: `command >> file.txt` (via system integration)
- Proper file handling and error management

#### 🔗 **Pipeline Support**
- Connect commands with pipes: `dir | sort`
- Multi-stage pipelines supported
- Implemented using temporary files for reliability
- Automatic cleanup of temporary resources

#### 🎨 **Colorized Interface**
- **Green prompts** with username and directory
- **Color-coded help text** and system messages
- **Customizable text colors** (8 color options)
- **Consistent color theming** throughout the interface

### 🔹 **User Experience Features**
- **Welcome screen** with ASCII art banner
- **UTF-8 support** for international characters
- **Smart prompt formatting** with current directory
- **Intuitive command syntax** familiar to Windows users
- **Graceful error recovery** - shell continues running after errors

## 🚀 Installation & Usage

### Prerequisites
- Windows operating system
- C compiler (GCC, MinGW, or MSVC)
- Basic knowledge of command-line interfaces

### Compilation

#### Using GCC/MinGW:
```bash
gcc -o advanced_shell.exe src/main.c
```

#### Using Microsoft Visual Studio:
```bash
cl src/main.c
```

#### With static linking (recommended):
```bash
gcc -static -o advanced_shell.exe src/main.c
```

### Running the Shell
```bash
./advanced_shell.exe
```

## 📖 Usage Examples

### Basic Commands
```bash
# Navigate directories
username@C:\> cd Documents
username@C:\Documents> dir

# View command history
username@C:\Documents> history
  1  cd Documents
  2  dir

# Clear screen
username@C:\Documents> clear
```

### Advanced Features
```bash
# Create and use aliases
username@C:\> alias ll "dir /w"
username@C:\> ll

# I/O Redirection
username@C:\> dir > files.txt
username@C:\> type files.txt
username@C:\> sort < input.txt > sorted.txt

# Pipelines
username@C:\> dir | sort
username@C:\> echo hello world | sort

# Repeat last command
username@C:\> dir
username@C:\> !!  # Executes 'dir' again

# Color customization
username@C:\> color 4  # Red text
username@C:\> color 2  # Green text
```

### Built-in Help System
```bash
username@C:\> help
=== Advanced Windows Shell ===
Type program names and arguments, and hit enter.
Special features:
  !! - repeat last command
  > file - redirect output to file
  < file - redirect input from file
  cmd1 | cmd2 - pipe output from cmd1 to cmd2

The following are built in:
  cd
  help
  exit
  history
  clear
  color
  alias
```

## 🛠️ Technical Implementation

### Architecture
- **Modular Design**: Each feature implemented as separate functions
- **Memory Management**: Proper allocation/deallocation with cleanup on exit
- **Error Handling**: Comprehensive error checking throughout
- **Cross-platform APIs**: Uses Windows-specific APIs where needed

### Key Components
```c
// Core shell functions
char *lsh_read_line(void);           // Read user input
char **lsh_split_line(char *line);   // Parse arguments
int lsh_execute(char **args);        // Execute commands
void lsh_loop(void);                 // Main shell loop

// Feature implementations
void add_to_history(char *line);     // History management
char *expand_alias(char *command);   // Alias expansion
int handle_redirection(char **args); // I/O redirection
int handle_pipeline(char *line);     // Pipeline processing
```

### Memory Management
- Dynamic allocation for command history
- Proper cleanup on shell exit
- Efficient string handling with `_strdup()`
- Automatic garbage collection for temporary data

### Windows Integration
- Uses Windows Console APIs for colors
- Integrates with Windows file system
- Supports Windows-style paths and commands
- Compatible with Windows command prompt conventions

## 🎨 Customization

### Adding New Built-in Commands
1. Add function declaration and implementation
2. Add command name to `builtin_str[]` array
3. Add function pointer to `builtin_func[]` array
4. Update help text in `lsh_help()` function

### Modifying Colors
Edit the `lsh_color()` function to add more color options or change the color scheme.

### Extending History Size
Modify the `MAX_HISTORY` constant to change the maximum number of stored commands.

## 🔧 Configuration

### Compile-time Options
```c
#define LSH_RL_BUFSIZE 1024      // Input buffer size
#define LSH_TOK_BUFSIZE 64       // Token buffer size  
#define MAX_HISTORY 100          // Maximum history entries
#define MAX_ALIASES 50           // Maximum alias entries
```

### Runtime Customization
- Set custom aliases for frequently used commands
- Adjust console colors with the `color` command
- Create complex command chains with pipes and redirection

## 🐛 Troubleshooting

### Common Issues

**Compilation Errors:**
- Ensure you have a compatible C compiler installed
- Check that all Windows headers are available
- Use static linking if getting runtime DLL errors

**Runtime Issues:**
- Make sure you have proper permissions for file operations
- Check that temporary files can be created in the current directory
- Verify Windows console supports UTF-8 if seeing character display issues

**Feature-Specific:**
- **History not working**: Check memory allocation in `add_to_history()`
- **Colors not displaying**: Verify console supports color attributes
- **Redirection failing**: Ensure proper file permissions and valid paths

## 🚦 Testing

### Manual Testing Checklist
- [ ] All built-in commands work correctly
- [ ] Command history stores and retrieves properly
- [ ] Aliases create and expand correctly
- [ ] I/O redirection works for input and output
- [ ] Pipelines connect commands properly
- [ ] Colors change and reset correctly
- [ ] Memory cleanup works without leaks
- [ ] Error handling responds appropriately

### Test Commands
```bash
# Test history
help
dir
history

# Test aliases  
alias test echo
test "Hello World"
alias

# Test redirection
echo "test data" > test.txt
type test.txt
sort < test.txt

# Test pipelines
dir | sort
echo "multiple words" | sort

# Test colors
color 1
color 2
color 7
```

## 🤝 Contributing

Contributions are welcome! Here are some areas for improvement:

### Planned Features
- [ ] Tab completion for commands and file names
- [ ] Persistent history across sessions
- [ ] Configuration file support
- [ ] Multiple pipe chain support (`cmd1 | cmd2 | cmd3`)
- [ ] Background process support
- [ ] Advanced globbing and wildcards
- [ ] Custom environment variable support

### How to Contribute
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes with proper testing
4. Commit your changes (`git commit -m 'Add amazing feature'`)
5. Push to the branch (`git push origin feature/amazing-feature`)
6. Open a Pull Request

### Code Style Guidelines
- Use consistent indentation (4 spaces)
- Add comments for complex logic
- Follow existing naming conventions
- Include error handling for new features
- Update documentation for new features

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Inspired by Unix shells and modern command-line interfaces
- Built as an educational project to demonstrate system programming concepts
- Uses Windows Console API for enhanced user experience
- Thanks to the open-source community for inspiration and best practices

## 📞 Support

If you encounter any issues or have questions:

1. Check the troubleshooting section above
2. Look through existing issues in the repository
3. Create a new issue with detailed information about your problem
4. Include your compiler version, Windows version, and error messages

## 🔗 Related Projects

- [GNU Bash](https://www.gnu.org/software/bash/) - The inspiration for many shell features
- [PowerShell](https://github.com/PowerShell/PowerShell) - Microsoft's advanced shell
- [Windows Terminal](https://github.com/microsoft/terminal) - Modern terminal for Windows

---

**Made with ❤️ for learning system programming and shell development**