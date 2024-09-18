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
 * Date: 9/17/2024
 * Description: This function will return true if the cell has visited
 * neighbors, and false otherwise.
*/ 
void has_visited_neighbors(int check_row, int check_col, int* neighbor_array)
{
    int i;
    for (i = 0; i < 4; i++)
        neighbor_array[i] = 0;

    if (!is_edge_maze(check_row - 2, check_col))
        if (maze[check_row - 2][check_col] == ' ')
            neighbor_array[0] = 1;
    if (!is_edge_maze(check_row + 2, check_col))
        if (maze[check_row + 2][check_col] == ' ')
            neighbor_array[1] = 1;
    if (!is_edge_maze(check_row, check_col - 2))
        if (maze[check_row][check_col - 2] == ' ')
            neighbor_array[2] = 1;
    if (!is_edge_maze(check_row, check_col + 2))
        if (maze[check_row][check_col + 2] == ' ')
            neighbor_array[3] = 1;
}

int choose_rand_direction(int* neighbors)
{
    int i;
    int num_neighbors = 0;
    int rand_index = -1;
    
    // Check valid neighbors
    for (i = 0; i < 4; i++)
    {
        if (neighbors[i])
            num_neighbors++;
    }
    
    // If no valid neighbors
    if (num_neighbors == 0)
    {
        return -1;
    }

    do 
    {
        rand_index = random_number(4, 0);
    } while (neighbors[rand_index] != 1);

    return rand_index;
}


void get_dx_dy(int direction, int* dxdy)
{
    dxdy[0] = 0;
    dxdy[1] = 0;
   
    // Based on the direction, choose the correct
    // dxdy value
    switch (direction) {
        case 0:
            dxdy[0] = -1;
            break;
        case 1:
            dxdy[0] = 1;
            break;
        case 2:
            dxdy[1] = -1;
            break;
        case 3:
            dxdy[1] = 1;
            break;
        default:
            break;
    }
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
    int vis_neighbor[3] = {0};
    int dxdy[2] = {0}; // Index 0 is y, index 1 is x
    int direction;
    // Scan the maze for an unvisited cell (excluding wall cells)
    for (y = 0; y < maze_height; y++)
    {
        for (x = 0; x < maze_width; x++)
        {
            // Skip cell if on the edge
            if (is_edge_maze(y, x))
                continue;
            // Skip cell if its already visited
            if (maze[y][x] == ' ')
                continue;

            // Check for visited neighbors
            has_visited_neighbors(y, x, vis_neighbor);
            
            // Check to see if we have visited neighbors. Returns -1 if we dont
            // else, it will return the direction to go
            direction = choose_rand_direction(vis_neighbor);

            // If there are no visited neighbors, skip this cell
            if (direction == -1)
                continue;

            // There are visited neighbors, and we have a direction to
            // a random one
            get_dx_dy(direction, dxdy);

            // There is a visited neighbor so we want to 
            // carve a passage between visited neighbor & currently
            // unvisited cell
            if (maze[y + dxdy[0]][x + dxdy[1]] == '#')
            {
                // Set to visited, and set as new starting location
                maze[y + dxdy[0] * 2][x + dxdy[1] * 2] = ' ';
                maze[y + dxdy[0]][x + dxdy[1]] = ' ';
                hunt_array[1] = y + dxdy[0] * 2;
                hunt_array[2] = x + dxdy[1] * 2;
            }
        }
    }
    // If we get here, that means that the maze is finished
    hunt_array[0] = 1;
}


void random_walk(int start_y, int start_x)
{
    int walk_x = start_x;
    int walk_y = start_y;
    int new_x;
    int new_y;
    int rand_direction;
    int dxdy[2] = {0};
    while (has_unvisited_neighbor(walk_y, walk_x))
    {
        // Choose a random direction
        rand_direction = random_number(4, 0);
        // Get the direction values
        get_dx_dy(rand_direction, dxdy);
        
        // Make sure that its not out of bounds or the edge of the maze
        new_y = walk_y + dxdy[0] * 2;
        new_x = walk_x + dxdy[1] * 2;
        if (is_edge_maze(new_y, new_x))
            continue;

        // Check to make sure its a wall and the connection is a wall
        if (maze[new_y][new_x] == '#' && maze[new_y - dxdy[0]][new_x - dxdy[1]]) 
        {
            maze[new_y][new_x] = ' ';
            maze[new_y - dxdy[0]][new_x - dxdy[1]] = ' ';
            walk_y = new_y;
            walk_x = new_x;
        }
    } 
}


/*
 * Name: Ethan Bielecki
 * Date: 9/17/2024
 * Description: This function will generate the maze using a hunt and kill approach
*/
void generate_maze(void)
{
    int x;
    int y;
    int rand_row = 0;
    int rand_col = 0;

    // Index 0 = maze finished or not
    // Index 1/2 = row/col of the next cell to hunt 
    int hunt_array[3] = {0};
 
    // Fill the maze with walls   
    for (y = 0; y < maze_height; y++)
    {
        for (x = 0; x < maze_width; x++)
        {
            maze[y][x] = '#';
        }
    }

    // Start generating the maze:

    // Choose a random location to start
    do {
        rand_row = random_number(maze_height, 0);
        rand_col = random_number(maze_width, 0);
    } while (!is_edge_maze(rand_row, rand_col));

    maze[rand_row][rand_col] = ' ';

    // Perform the generation loop
    do {
        random_walk(hunt_array[1], hunt_array[2]);
        hunt_mode(hunt_array);
    } while (!hunt_array[0]);
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
    char buffer[512];

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

