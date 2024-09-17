proc_file_path = '/proc/maze_gen'

width = str(raw_input("Maze width? "))
height = str(raw_input("Maze Height? "))


print('Width of maze is: ' + width)
print("Height of maze is: " + height)

with open(proc_file_path, 'w') as f:
    f.write(width + " " + height)

with open(proc_file_path, 'r') as f:
    output = f.read()
    print(output)

# kernel_module = open('/proc/maze_gen')

# greeting = kernel_module.readline()
# print(greeting)
# kernel_module.close()
