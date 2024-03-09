CC=gcc

make: main.o
	$(CC) main.o -L/lib/wiiuse.lib -lwiiuse -o mouse_controller

main.o: main.c
	$(CC) -c main.c

clean:
	-del main.o mouse_controller.exe