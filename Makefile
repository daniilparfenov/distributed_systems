.PHONY: all mandelbrot clean

all: mandelbrot

mandelbrot:
	@echo "Building Mandelbrot..."
	$(MAKE) -C hw1/1_mandelbrot

clean:
	@echo "Cleaning up..."
	@rm -rf build
	$(MAKE) -C hw1/1_mandelbrot clean
