all: custom_shell

custom_shell: main.c
	gcc -Wall -g -o custom_shell main.c

install: custom_shell
	sudo cp custom_shell /bin/

uninstall:
	sudo rm -f /bin/custom_shell

clean:
	rm -f custom_shell
