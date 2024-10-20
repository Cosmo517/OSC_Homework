#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

/*
 * Name: Ethan Bielecki
 * Date: 10/20/2024
 * Description: Clears the screen and prints the message of the day randomly from
 * a list
*/
void motd()
{
    // Some fun game related messages
    const char* messages[] = {
        "Exploding Creepers!",
        "The cake is a lie.",
        "Companion Cube.",
        "Were you blinded by its majesty?",
        "Were it so easy."
    };
    
    srand(time(NULL));
        
    // Chose one at random
    int random_sentence = rand() % 5;

    system("clear");
    printf("%s\n", messages[random_sentence]);
}

/*
 * Name: Ethan Bielecki
 * Date: 10/20/2024
 * Description: This function will change the users directory when they
 * type the cd command
*/ 
void change_directory(char* command)
{
    // This gets the home path of the user. "HOME" is a
    // UNIX environmental variable
    char* home_path = getenv("HOME");

    // Check if the user just typed cd
    if (strcmp(command, "cd") == 0)
    {
        // The user only typed "cd", so send them to home directory
        chdir(home_path);
        return;
    }
    
    // Otherwise, we need to grab the path they are giving us
    // This will skip the cd and the space to get a pointer
    // to the start of the path
    char* user_path = command + 3;
    
    if (chdir(user_path) != 0)
    {
        printf("\x1B[31mUnable to locate directory.\x1B[0m\n");
    }
}

/*
 * Name: Ethan Bielecki
 * Date: 10/20/2024
 * Descrpition: This functions main purpose is to print the 
 * current working directory. This will allow the user to see what 
 * location of the OS they are in
*/ 
void print_cwd()
{
    char cwd[1024];
    // Get the current working directory and store it in cwd
    getcwd(cwd, sizeof(cwd));

    // Print out the cwd with ANSI color codes to separate it from
    // the users own input
    printf("\033[1;32m%s>\033[0m", cwd);
}


/*
 * Name: Ethan Bielecki
 * Date: 10/20/2024
 * Description: This function will perform any other command
 * that the user types that isn't cd or exit 
*/
void perform_user_command(char* command, int background)
{
    // This will store the command arguments
    char* command_args[64];
    // These are for input redirections
    char* output_file = NULL;
    char* input_file = NULL;
    int fd;

    // We need to check to see if the command the user entered
    // has an output redirection
    if (strstr(command, ">") != NULL)
    {
        // The first strtok will tokenize the command by >
        // The second strtok will parse the original string by spaces
        output_file = strtok(command, ">");
        output_file = strtok(NULL, " ");

        // If the redirecton was used correctly (given a file name)
        if (output_file != NULL)
        {
            // Loop until we reach a character in the file name
            // that doesn't start with a space. I.e. if the user
            // put two spaces in front of the > like:
            // ls >  test.txt
            while (*output_file == ' ')
            {
                output_file++;
            }
        }
    }

    // We also need to check to see if the command the user
    // entered has an input redirection
    if (strstr(command, "<") != NULL)
    {
        // The code below works exactly like the output
        // redirection above
        input_file = strtok(command, "<");
        input_file = strtok(NULL, " ");
        if (input_file != NULL)
        {
            while (*input_file == ' ')
            {
                input_file++;
            }
        }
    }


    // Fork the process so we can run a user command
    int pid = fork();

    if (pid == 0)
    {
        // Child process will run the command the user specified

        // Check to see if the user used an output redirection
        if (output_file != NULL)
        {
            // Try and open the output file as write only, and if it doesn't
            // exist, create it
            fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            
            // If fd == -1 the file couldn't be opened
            if (fd == -1)
            {
                printf("Failed to open the output file");
                exit(EXIT_FAILURE);
            }
            
            // dup2 allows us to copy the terminal output into
            // the file.
            dup2(fd, STDOUT_FILENO);
            
            // Close the file
            close(fd);
        }

        // Check to see if the user used an input file redirection
        if (input_file != NULL)
        {
            // Try to open the input file as read only
            fd = open(input_file, O_RDONLY);

            // If fd == -1 the file couldn't be opened
            if (fd == -1)
            {
                printf("Failed to open the input file");
                exit(EXIT_FAILURE);
            }

            // dup2 here allows us to redirect reading from the keyboard
            // to reading from the file.
            dup2(fd, STDIN_FILENO);

            // Close the file
            close(fd);
        }

        // Perform the command the user requested
        char* args[] = {"/bin/sh", "-c", command, NULL};
        if (execvp(args[0], args) == -1)
        {
            printf("Failed to execute the command");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // Parent process, it will either wait until the 
        // child process is done, or continue if background == 1
        if (!background)
        {
            // Background is 0, meaning we need to wait
            printf("------------------------Starting Program---------------------\n");
            wait(NULL);
            printf("------------------------Program Ended------------------------\n");
        }
    }
}


/*
 * Name: Ethan Bielecki
 * Date: 10/20/2024
 * Description: The main method to run the program and listen to user input
*/ 
int main()
{
    // Call motd once upon startup
    motd();

    // This is where we will store user input to later
    // parse out the command the user wants to perform
    char user_input[1024];

    // We want to repeat until the user exits the program themselves
    while (1)
    {
        // Always print the cwd before allowing user input
        print_cwd();
        
        // Now we need to get user input. fgets lets us grab a whole line
        fgets(user_input, 1024, stdin);
        
        // Search for the new line character and remove it
        for (int i = 0; i < 1024; i++)
        {
            if (user_input[i] == '\n')
                user_input[i] = '\0';
        }

        // Now lets check for the commands the user types
        if (strcmp(user_input, "exit") == 0)
        {
            printf("\x1B[31mExiting shell!\n");
            break;
        }
        else if (strncmp(user_input, "cd", 2) == 0)  // Only check first 2 chars for cd
        {
            // We can simply pass the full command to user input and it
            // will handle all the parsing
            change_directory(user_input);
        }
        else
        {
            // Check if the user wants to run the command in the background
            // If they do, set background to 1, and remove the & 
            // from the command
            int background = (user_input[strlen(user_input) - 1] == '&');
            if (background)
                user_input[strlen(user_input) - 1] = '\0';
            perform_user_command(user_input, background);
        }
    }
}
