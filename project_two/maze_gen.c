/*
 * maze_gen.c
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 128

#define PROC_NAME "maze_gen"

static char user_input[BUFFER_SIZE] = {0};
char **maze;
int maze_height = 0;
int maze_width = 0;


ssize_t maze_gen_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);
ssize_t start_maze_gen(struct file *file, char *buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
        .read = start_maze_gen,
        .write = maze_gen_write,
};

/*
 * Name: Ethan Bielecki
 * Date: 9/16/2024
 * Description: This function initializes the maze generation
*/ 
int init_gen(void)
{
    proc_create(PROC_NAME, 0, NULL, &proc_ops);
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

	return 0;
}

/*
 * Name: Ethan Bielecki
 * Date: 9/17/2024
 * Description: This function frees the memory from the maze
 * when the program ends
*/
void free_maze_memory(void)
{
    int i;
    for (i = 0; i < maze_height; i++)
    {
        kfree(maze[i]);
    }

    kfree(maze);
}


/*
 * Name: Ethan Bielecki
 * Date: 9/16/2024
 * Description: This function cleans up the maze generation
*/ 
void gen_cleanup(void) 
{
    free_maze_memory();
    remove_proc_entry(PROC_NAME, NULL);
    printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/*
 * Name: Ethan Bielecki
 * Date: 9/16/2024
 * Description: This function accepts user input
*/
ssize_t maze_gen_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos)
{
    if (count > BUFFER_SIZE - 1)
    {
        count = BUFFER_SIZE - 1;
    }

    if (copy_from_user(user_input, usr_buf, count))
    {
        return -EFAULT;
    }

    user_input[count] = '\0';

    return count;
}

/*
 * Name: Ethan Bielecki
 * Date: 9/17/2024
 * Description: This function allocates the memory for the maze
 * as the size comes from user input
*/ 
void allocate_maze_space(void)
{
    int i;
    // GFP_Kernel is a common flag to dynamically allocate memory
    maze = kmalloc(maze_height * sizeof(char*), GFP_KERNEL);

    for (i = 0; i < maze_width; i++)
    {
        maze[i] = kmalloc(maze_width * sizeof(char), GFP_KERNEL);
    }
}

/*
 * Name: Ethan Bielecki
 * Date: 9/17/2024
 * Description: This function will generate a random number given an upper
 * bound, and return the positive version, including an offset
*/ 
int random_number(int bound, int offset);
{
    int i;
    int rand_num;

    // Generate random number
    get_random_bytes(&i, sizeof(i));
    rand_num = i % bound;
    
    if (rand_num < 0)
        rand_num = rand_num * -1 + offset;
    else
        rand_num += offset;

    return rand_num;
}

/*
 * Name: Ethan Bielecki
 * Date: 9/17/2024
 * Description: This function will check if the row & column is on the edge of the maze
 * (the outside border). It will return true if it is, and false otherwise
*/
int is_edge_maze(int check_row, int check_col)
{
    if (check_row == 0 || check_row == rows - 1)
        return 1;

    if (check_col == 0 || check_col == cols - 1)
        return 1;

    return 0;
}

/*
 * Name: Ethan Bielecki
 * Date: 9/17/2024
 * Description: This function checks if the cell has an unvisisted neighbor in the
 * 4 cardinal directions
*/ 
int has_unvisited_neighbor(int check_row, int check_col)
{
    if (!is_edge_maze(check_row - 1, check_col))
        if (maze[check_row - 1][check_col] == '#')
            return 1;
    if (!is_edge_maze(check_row + 1, check_col))
        if (maze[check_row + 1][check_col] == '#')
            return 1;
    if (!is_edge_maze(check_row, check_col - 1))
        if (maze[check_row][check_col - 1] == '#')
            return 1;
    if (!is_edge_maze(check_row, check_col + 1))
        if (maze[check_row][check_col + 1] == '#')
            return 1;

    return 0;
}

/*
 * Name: Ethan Bielecki
 * Date: 9/17/2024
 * Description: This function will return true if the cell has visited
 * neighbors, and false otherwise.
*/ 
int has_visited_neighbors(int check_row, int check_col)
{

    if (!is_edge_maze(check_row - 1, check_col))
        if (maze[check_row - 1][check_col] == ' ')
            return 1;
    if (!is_edge_maze(check_row + 1, check_col))
        if (maze[check_row + 1][check_col] == ' ')
            return 1;
    if (!is_edge_maze(check_row, check_col - 1))
        if (maze[check_row][check_col - 1] == ' ')
            return 1;
    if (!is_edge_maze(check_row, check_col + 1))
        if (maze[check_row][check_col + 1] == ' ')
            return 1;

    return 0;
}

/*
 * Name: Ethan Bielecki
 * Date: 9/17/2024
 * Description: This function will perform the hunt mode of the hunt-and-kill algorithm
 * it will modify the hunt_array, to identify if the maze is finished or not, and the
 * next visited square
*/ 
void hunt_mode(int *hunt_array)
{
    int x;
    int y;
    // Scan the maze for an unvisited cell (excluding wall cells)
    for (y = 0; y < rows; y++)
    {
        for (x = 0; x < cols; x++)
        {
            if (is_edge_maze(y, x))
                continue;

            if (maze[y][x] == '#' && has_visited_neighbors(y, x))
            {
                // Set to visited, and set as new starting location
                maze[y][x] = ' ';
                hunt_array[1] = y;
                hunt_array[2] = x;
            }
        }
    }
    // If we get here, that means that the maze is finished
    hunt_array[0] = 1;
}

/*
 * Name: Ethan Bielecki
 * Date: 9/17/2024
 * Description: This function will generate the maze using a hunt and kill approach
*/
void generate_maze(void)
{
    // Fill the maze with walls
    int x;
    int y;

    // Index 0 = maze finished or not
    // Index 1/2 = row/col of the next cell to hunt 
    int hunt_array[3] = {0};
    
    for (y = 0; y < maze_height; y++)
    {
        for (x = 0; x < maze_width; x++)
        {
            maze[y][x] = '#';
        }
    }

    // Start generating the maze:
    
}

/*
 * Name: Ethan Bielecki
 * Date: 9/16/2024
 * Description: This function will generate a maze and return it
*/ 
ssize_t start_maze_gen(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    // rv holds the length of the string to be returned
    int rv = 0;
    static int completed = 0;
    
    // Maze functionality below:
    int x;
    int y;
    char buffer[700];

    sscanf(user_input, "%d %d", &maze_width, &maze_height);

    if (completed) 
    {
        completed = 0;
        return 0;
    }

    // Allocate space for the maze:
    allocate_maze_space();

    // Generate the maze
    generate_maze();
    
    // Convert the maze 2d array into a string to be printed
    for (y = 0; y < maze_height; y++)
    {
        for (x = 0; x < maze_width; x++)
        {
            rv += sprintf(buffer + rv, "%c", maze[y][x]);
        }
        rv += sprintf(buffer + rv, "\n");
    }

    completed = 1;

    copy_to_user(usr_buf, buffer, rv);

    return rv;
}


/* Macros for registering module entry and exit points. */
module_init( init_gen );
module_exit( gen_cleanup );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("This module will generate a maze for the user");
MODULE_AUTHOR("Ethan Bielecki");

