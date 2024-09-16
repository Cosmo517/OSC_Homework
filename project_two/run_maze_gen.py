proc_file_path = '/proc/maze_gen'

user_input = input("What to say? ")

with open(proc_file_path, 'w') as f:
    f.write(user_input)

with open(proc_file_path, 'r') as f:
    output = f.read()
    print(output)

# kernel_module = open('/proc/maze_gen')

# greeting = kernel_module.readline()
# print(greeting)
# kernel_module.close()
