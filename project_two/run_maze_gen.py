proc_file_path = '/proc/maze_gen'

print("Please enter odd numbers for the width and height")

# Loop until valid width/height is entered
while True:
    # Loop until the user enters an odd number that is not less than 5
    while True:
        width = input("Maze width? ")
        try:
            if int(width) < 5:
                print("Please enter a width greater than 5")
            elif int(width) % 2 == 1:
                break
            else:
                print("The value entered is not odd")
        except:
            print("The value entered was not a number")

    # Loop until the user enters an odd number that is not less than 5
    while True:
        height = input("Maze height? ")
        try:
            if int(height) < 5:
                print("Please enter a height greater than 5")
            elif int(height) % 2 == 1:
                break
            else:
                print("The value entered is not odd")
        except:
            print("The value entered was not a number")
    
    # Make sure that the entered width/height wont generate a maze that exceeds the kernels buffer
    if int(width) * int(height) > 890:
        print("The width and height entered will make a maze too large, (width*height > 890)")
        print("Please enter a smaller width/height")
    else:
        break

# Open the proc path to send the width and height in
with open(proc_file_path, 'w') as f:
    f.write(width + " " + height)

# Read the maze from the proc file
with open(proc_file_path, 'r') as f:
    output = f.read()
    print(output)
