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
 * Date: 9/20/2024
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

    for (i = 0; i < maze_height; i++)
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
int random_number(int bound, int offset)
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
    if (check_row <= 0 || check_row >= maze_height - 1)
        return 1;

    if (check_col <= 0 || check_col >= maze_width - 1)
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
    if (!is_edge_maze(check_row - 2, check_col))
        if (maze[check_row - 2][check_col] == '#')
            return 1;
    if (!is_edge_maze(check_row + 2, check_col))
        if (maze[check_row + 2][check_col] == '#')
            return 1;
    if (!is_edge_maze(check_row, check_col - 2))
        if (maze[check_row][check_col - 2] == '#')
            return 1;
    if (!is_edge_maze(check_row, check_col + 2))
        if (maze[check_row][check_col + 2] == '#')
            return 1;

    return 0;
}

/*
 * Name: Ethan Bielecki
 * Date: 9/19/2024
 * Description: This function will perform the hunt mode of the hunt-and-kill algorithm
 * It will return a value of 1 if the maze is unfinished (i.e. found an unvisited cell
 * with a visited neighbor). Otherwise, if the maze is complete, it will return 0.
*/ 
int hunt_mode(int* walk_y, int* walk_x, int directions[4][2])
{
    int i;
    int j;
    int k;
    int dx;
    int dy;
    // We need to search for an unvisited cell that has a neighbor that has been visited
    // It's best to iterate through odd numbers so the maze maintains a clean look
    for (i = 1; i < maze_height - 1; i+= 2)
    {
        for (j = 1; j < maze_width - 1; j+= 2)
        {
            // Although we are excluding edge cells, we will still double check
            // to make sure its not out of bounds or on the edge.
            // If the cell is unvisited, we need to check its neighbors
            if (!is_edge_maze(i, j) && maze[i][j] == '#')
            {
                for (k = 0; k < 4; k++)
                {
                    dy = i + directions[k][1];
                    dx = j + directions[k][0];
                    // Again, we will do bounds checking, and see if the cell is visited
                    // If it has been visited, mark the previous unvisited cell as visited, 
                    // and make it our new starting point
                    if (!is_edge_maze(dy, dx) && maze[dy][dx] == ' ')
                    {
                        maze[i][j] = ' ';
                        maze[i + directions[k][1] / 2][j + directions[k][0] / 2] = ' ';
                        *walk_y = i;
                        *walk_x = j;
                        return 1;
                    }
                }
            }
        }
    }

    // If we make it here, then the maze has been completed
    return 0;
}

/*
 * Name: Ethan Bielecki
 * Date: 9/18/2024
 * Description: This function will perform the walk state of the hunt and kill algorithm.
*/ 
void random_walk(int* walk_y, int* walk_x, int directions[4][2])
{
    // We want to walk until there are no more neighbors we can visit (dead end)
    while (has_unvisited_neighbor(*walk_y, *walk_x))
    {
        int i;
        int valid_directions[4][2];
        int total_valid_directions = 0;
        int dx;
        int dy;

        // For our current position, calculate all possible directions in which
        // we can move. A move is viable if its not an edge cell, and that cell
        // hasn't been visited yet
        for (i = 0; i < 4; i++)
        {
            // Calculate the change in direction
            dy = *walk_y + directions[i][1];
            dx = *walk_x + directions[i][0];

            // Check to make sure the cell is a valid cell to travel to
            if (!is_edge_maze(dy, dx) && maze[dy][dx] == '#')
            {
                valid_directions[total_valid_directions][0] = directions[i][0];
                valid_directions[total_valid_directions][1] = directions[i][1];
                total_valid_directions++;
            }
        }

        // If there are any valid directions to travel to, we should choose 
        // a direction randomly, and travel that way
        if (total_valid_directions > 0)
        {
            int random_index = random_number(total_valid_directions, 0);
            int dy = valid_directions[random_index][1];
            int dx = valid_directions[random_index][0]; 
            int middle_cell_y = *walk_y + dy / 2;
            int middle_cell_x = *walk_x + dx / 2;
            
            maze[middle_cell_y][middle_cell_x] = ' ';
            maze[*walk_y + dy][*walk_x + dx] = ' ';

            *walk_y += dy;
            *walk_x += dx;
        }
    }
}


/*
 * Name: Ethan Bielecki
 * Date: 9/19/2024
 * Description: This function will generate the maze using a hunt and kill approach
*/
void generate_maze(void)
{
    int x;
    int y;
    int walk_x;
    int walk_y;
    int directions[4][2] = {{2, 0}, {-2, 0}, {0, 2}, {0, -2}};
    int maze_unfinished;


    // Fill the maze with walls   
    for (y = 0; y < maze_height; y++)
    {
        for (x = 0; x < maze_width; x++)
        {
            maze[y][x] = '#';
        }
    }

    // Start generating the maze:

    // Choose a random location to start, make sure its an odd number
    do {
        walk_y = random_number(maze_height / 2, 0) * 2 + 1;
        walk_x = random_number(maze_width / 2, 0) * 2 + 1;
    } while (is_edge_maze(walk_y, walk_x));

    maze[walk_y][walk_x] = ' ';

    // Perform the generation loop
    do {
        random_walk(&walk_y, &walk_x, directions);
        maze_unfinished = hunt_mode(&walk_y, &walk_x, directions);
    } while (maze_unfinished);
}

/*
 * Name: Ethan Bielecki
 * Date: 9/20/2024
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
    char buffer[896];

    // If the program has already been completed, then we can end

    if (completed) 
    {
        completed = 0;
        return 0;
    }

    // Read user input to get the width and height of the maze

    sscanf(user_input, "%d %d", &maze_width, &maze_height);
    

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
MODULE_DESCRIPTION("This module will generate a maze for the user given width/height");
MODULE_AUTHOR("Ethan Bielecki");

