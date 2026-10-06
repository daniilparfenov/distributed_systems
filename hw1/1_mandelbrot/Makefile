CC=clang
CFLAGS=-Wall -Wextra -lm -lpthread

all: mandelbrot mandelbrot_scalar

mandelbrot: mandelbrot.c
	$(CC) $(CFLAGS) mandelbrot.c -o mandelbrot.elf

mandelbrot_scalar: mandelbrot_scalar.c
	$(CC) $(CFLAGS) mandelbrot_scalar.c -o mandelbrot_scalar.elf

clean:
	rm -rf *.o *.elf
