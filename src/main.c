
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <direct.h>
#include <windows.h>
#include <io.h>

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n"
#define MAX_HISTORY 100
#define MAX_ALIASES 50

// Global variables
char *history[MAX_HISTORY];
int history_count = 0;
char *last_command = NULL;

// Alias structure
struct Alias {
    char *name;
    char *command;
};
struct Alias aliases[MAX_ALIASES];
int alias_count = 0;

// Function declarations
char *lsh_read_line(void);
char **lsh_split_line(char *line);
int lsh_execute(char **args);
int lsh_launch(char **args);
void lsh_loop(void);
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_history(char **args);
int lsh_clear(char **args);
int lsh_color(char **args);
int lsh_alias(char **args);
int lsh_num_builtins(void);
void add_to_history(char *line);
void show_prompt(void);
char *expand_alias(char *command);
int handle_redirection(char **args);
int handle_pipeline(char *line);

char *lsh_read_line(void)
{
    char *line = malloc(LSH_RL_BUFSIZE);
    if (!line) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }
    
    if (fgets(line, LSH_RL_BUFSIZE, stdin) == NULL) {
        if (feof(stdin)) {
            free(line);
            exit(EXIT_SUCCESS);
        } else {
            perror("readline");
            free(line);
            exit(EXIT_FAILURE);
        }
    }
    
    // Remove newline
    line[strcspn(line, "\n")] = 0;
    return line;
}

char **lsh_split_line(char *line)
{
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }
    
    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;
        
        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "history",
    "clear",
    "color",
    "alias"
};

int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit,
    &lsh_history,
    &lsh_clear,
    &lsh_color,
    &lsh_alias
};

int lsh_num_builtins(void) {
    return sizeof(builtin_str) / sizeof(char *);
}

void add_to_history(char *line) {
    if (history_count < MAX_HISTORY) {
        history[history_count] = _strdup(line);
        history_count++;
    } else {
        // Shift history and add new command
        free(history[0]);
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            history[i] = history[i + 1];
        }
        history[MAX_HISTORY - 1] = _strdup(line);
    }
}

void show_prompt(void) {
    char cwd[1024];
    char username[256];
    DWORD size = sizeof(username);
    
    GetUserName(username, &size);
    _getcwd(cwd, sizeof(cwd));
    
    // Set prompt color to green
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                           FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("%s@%s> ", username, cwd);
    
    // Reset color to white
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                           FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

char *expand_alias(char *command) {
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(command, aliases[i].name) == 0) {
            return aliases[i].command;
        }
    }
    return command;
}

/*
  Builtin function implementations.
*/
int lsh_cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else {
        if (_chdir(args[1]) != 0) {
            perror("lsh");
        }
    }
    return 1;
}

int lsh_help(char **args)
{
    int i;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                           FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    printf("=== Advanced Windows Shell ===\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                           FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    
    printf("Type program names and arguments, and hit enter.\n");
    printf("Special features:\n");
    printf("  !! - repeat last command\n");
    printf("  > file - redirect output to file\n");
    printf("  < file - redirect input from file\n");
    printf("  cmd1 | cmd2 - pipe output from cmd1 to cmd2\n\n");
    
    printf("The following are built in:\n");
    for (i = 0; i < lsh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }
    
    printf("\nUse 'help <command>' for information on other programs.\n");
    return 1;
}

int lsh_exit(char **args)
{
    // Clean up history
    for (int i = 0; i < history_count; i++) {
        free(history[i]);
    }
    
    // Clean up aliases
    for (int i = 0; i < alias_count; i++) {
        free(aliases[i].name);
        free(aliases[i].command);
    }
    
    if (last_command) {
        free(last_command);
    }
    
    printf("Goodbye!\n");
    return 0;
}

int lsh_history(char **args)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                           FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    printf("Command History:\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                           FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    
    for (int i = 0; i < history_count; i++) {
        printf("%3d  %s\n", i + 1, history[i]);
    }
    return 1;
}

int lsh_clear(char **args)
{
    system("cls");
    return 1;
}

int lsh_color(char **args)
{
    if (args[1] == NULL) {
        printf("Usage: color <number>\n");
        printf("Colors: 1=Blue, 2=Green, 3=Cyan, 4=Red, 5=Purple, 6=Yellow, 7=White\n");
        return 1;
    }
    
    int color = atoi(args[1]);
    WORD attribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // default white
    
    switch (color) {
        case 1: attribute = FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
        case 2: attribute = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
        case 3: attribute = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
        case 4: attribute = FOREGROUND_RED | FOREGROUND_INTENSITY; break;
        case 5: attribute = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
        case 6: attribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
        case 7: attribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; break;
        default: 
            printf("Invalid color code. Use 1-7.\n");
            return 1;
    }
    
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attribute);
    printf("Color changed!\n");
    return 1;
}

int lsh_alias(char **args)
{
    if (args[1] == NULL) {
        // Show all aliases
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                               FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        printf("Current aliases:\n");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                               FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        
        for (int i = 0; i < alias_count; i++) {
            printf("%s='%s'\n", aliases[i].name, aliases[i].command);
        }
        return 1;
    }
    
    if (args[2] == NULL) {
        printf("Usage: alias <name> <command>\n");
        printf("       alias <name>=<command>\n");
        return 1;
    }
    
    // Handle both "alias name command" and "alias name=command"
    char *name = args[1];
    char *command;
    
    if (strchr(args[1], '=')) {
        // Handle name=command format
        char *eq = strchr(args[1], '=');
        *eq = '\0';
        command = eq + 1;
    } else {
        command = args[2];
    }
    
    if (alias_count < MAX_ALIASES) {
        aliases[alias_count].name = _strdup(name);
        aliases[alias_count].command = _strdup(command);
        alias_count++;
        printf("Alias created: %s='%s'\n", name, command);
    } else {
        printf("Maximum number of aliases reached.\n");
    }
    
    return 1;
}

int handle_redirection(char **args)
{
    int i = 0;
    char *output_file = NULL;
    char *input_file = NULL;
    char command[1024] = "";
    
    // Look for redirection operators and build command string
    int j = 0;
    while (args[j] != NULL) {
        if (strcmp(args[j], ">") == 0) {
            if (args[j + 1] != NULL) {
                output_file = args[j + 1];
                break; // Stop building command here
            }
        } else if (strcmp(args[j], "<") == 0) {
            if (args[j + 1] != NULL) {
                input_file = args[j + 1];
                break; // Stop building command here
            }
        } else {
            // Add to command string
            if (j > 0) strcat(command, " ");
            strcat(command, args[j]);
        }
        j++;
    }
    
    // Build full command with redirection
    char full_command[1024];
    strcpy(full_command, command);
    
    if (output_file) {
        strcat(full_command, " > ");
        strcat(full_command, output_file);
    }
    
    if (input_file) {
        strcat(full_command, " < ");
        strcat(full_command, input_file);
    }
    
    // Execute using system()
    system(full_command);
    return 1;
}

int handle_pipeline(char *line)
{
    // Simple pipeline implementation using temporary files
    char *pipe_pos = strchr(line, '|');
    if (!pipe_pos) return 0; // No pipe found
    
    *pipe_pos = '\0'; // Split the command
    char *cmd1 = line;
    char *cmd2 = pipe_pos + 1;
    
    // Remove leading/trailing spaces
    while (*cmd1 == ' ') cmd1++;
    while (*cmd2 == ' ') cmd2++;
    
    // Create temporary file
    char temp_file[] = "temp_pipe.txt";
    
    // Execute first command with output redirection
    char full_cmd1[512];
    sprintf(full_cmd1, "%s > %s", cmd1, temp_file);
    system(full_cmd1);
    
    // Execute second command with input redirection
    char full_cmd2[512];
    sprintf(full_cmd2, "%s < %s", cmd2, temp_file);
    system(full_cmd2);
    
    // Clean up temporary file
    remove(temp_file);
    
    return 1; // Pipeline handled
}

int lsh_launch(char **args)
{
    // Check for redirection first
    int has_redirection = 0;
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0 || strcmp(args[i], "<") == 0) {
            has_redirection = 1;
            break;
        }
    }
    
    if (has_redirection) {
        return handle_redirection(args);
    }
    
    // Simple approach using system() - rebuild command string
    char command[1024] = "";
    int i = 0;
    
    while (args[i] != NULL) {
        strcat(command, args[i]);
        if (args[i + 1] != NULL) {
            strcat(command, " ");
        }
        i++;
    }
    
    int result = system(command);
    return 1;
}

int lsh_execute(char **args)
{
    int i;

    if (args[0] == NULL) {
        return 1;
    }
    
    // Handle !! (repeat last command)
    if (strcmp(args[0], "!!") == 0) {
        if (last_command) {
            printf("Executing: %s\n", last_command);
            char *line_copy = _strdup(last_command);
            char **new_args = lsh_split_line(line_copy);
            int result = lsh_execute(new_args);
            free(line_copy);
            free(new_args);
            return result;
        } else {
            printf("No previous command found.\n");
            return 1;
        }
    }
    
    // Expand aliases
    char *expanded = expand_alias(args[0]);
    if (expanded != args[0]) {
        // Replace first argument with expanded alias
        char expanded_line[1024];
        strcpy(expanded_line, expanded);
        for (i = 1; args[i] != NULL; i++) {
            strcat(expanded_line, " ");
            strcat(expanded_line, args[i]);
        }
        char **new_args = lsh_split_line(expanded_line);
        int result = lsh_execute(new_args);
        free(new_args);
        return result;
    }

    // Check built-in commands
    for (i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return lsh_launch(args);
}

void lsh_loop(void)
{
    char *line;
    char **args;
    int status;

    do {
        show_prompt();
        line = lsh_read_line();
        
        // Skip empty lines
        if (strlen(line) == 0) {
            free(line);
            continue;
        }
        
        // Handle pipelines
        if (strchr(line, '|')) {
            handle_pipeline(line);
            add_to_history(line);
            if (last_command) free(last_command);
            last_command = _strdup(line);
            free(line);
            continue;
        }
        
        // Add to history
        add_to_history(line);
        
        // Update last command
        if (last_command) free(last_command);
        last_command = _strdup(line);
        
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);
    } while (status);
}

int main(void)
{
    // Set console to UTF-8 for better character support
    SetConsoleOutputCP(CP_UTF8);
    
    // Clear screen and show welcome message
    system("cls");
    
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                           FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("╔══════════════════════════════════════════════╗\n");
    printf("║          Advanced Windows Shell              ║\n");
    printf("║                                              ║\n");
    printf("║  Features: History, Aliases, Redirection,   ║\n");
    printf("║  Pipelines, Colors, and more!                ║\n");
    printf("║                                              ║\n");
    printf("║  Type 'help' for available commands         ║\n");
    printf("╚══════════════════════════════════════════════╝\n\n");
    
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                           FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    
    // Add some default aliases
    aliases[alias_count].name = _strdup("ll");
    aliases[alias_count].command = _strdup("dir");
    alias_count++;
    
    aliases[alias_count].name = _strdup("ls");
    aliases[alias_count].command = _strdup("dir");
    alias_count++;
    
    lsh_loop();
    return 0;
}