kernel_module = open('/proc/maze_gen')

greeting = kernel_module.readline()
print(greeting)
kernel_module.close()
