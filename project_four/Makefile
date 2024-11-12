custom_scheduler: main.c
	gcc -g -o custom_scheduler main.c

install: custom_shell
	sudo cp custom_scheduler /bin/

uninstall:
	sudo rm -f /bin/custom_scheduler

clean:
	rm -f custom_scheduler
