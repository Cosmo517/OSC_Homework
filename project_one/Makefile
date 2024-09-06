runme: main.c LuckyNumber.h LuckyNumber.o record.h
	   gcc main.c LuckyNumber.o -o runme

LuckyNumber.o: LuckyNumber.c LuckyNumber.h
	           gcc -c LuckyNumber.c

clean:
	rm *.o runme

install: runme
	     sudo cp ./runme /bin/

uninstall: runme
	       sudo rm /bin/runme
