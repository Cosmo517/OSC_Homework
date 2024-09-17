/*
 * maze_gen.c
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 128

#define PROC_NAME "maze_gen"

static char user_input[BUFFER_SIZE] = {0};
int **maze;

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

/* This function is called when the module is removed. */

/*
 * Name: Ethan Bielecki
 * Date: 9/16/2024
 * Description: This function cleans up the maze generation
*/ 
void gen_cleanup(void) 
{
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
void allocate_maze_space(int rows, int cols)
{
    int i;
    // GFP_Kernel is a common flag to dynamically allocate memory
    maze = kmalloc(rows * sizeof(int*), GFP_KERNEL);

    for (i = 0; i < cols; i++)
    {
        maze[i] = kmalloc(cols * sizeof(int), GFP_KERNEL);
    }
}

/*
 * Name: Ethan Bielecki
 * Date: 9/17/2024
 * Description: This function frees the memory from the maze
 * when the program ends
*/
void free_maze_memory(int rows)
{
    int i;
    for (i = 0; i < rows; i++)
    {
        kfree(maze[i]);
    }

    kfree(maze);
}

void generate_maze(int rows, int cols)
{

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
    int maze_width = 0;
    int maze_height = 0;
    char buffer[BUFFER_SIZE];
    sscanf(user_input, "%d %d", &maze_width, &maze_height);
    rv = sprintf(buffer, "Width: %d\nHeight: %d", maze_width, maze_height);

    if (completed) 
    {
        completed = 0;
        return 0;
    }

    completed = 1;

    //rv = strlen(user_input);

    // copies the contents of buffer to userspace usr_buf
    //copy_to_user(usr_buf, user_input, rv);
    copy_to_user(usr_buf, buffer, rv);

    return rv;
}


/* Macros for registering module entry and exit points. */
module_init( init_gen );
module_exit( gen_cleanup );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("This module will generate a maze for the user");
MODULE_AUTHOR("Ethan Bielecki");

