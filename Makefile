.PHONY: all mandelbrot clean

all: mandelbrot

mandelbrot:
	@echo "Building Mandelbrot..."
	$(MAKE) -C hw1/1_mandelbrot

n_body:
	@echo "Building N-body..."
	$(MAKE) -C hw1/2_nbody

clean:
	@echo "Cleaning up..."
	@rm -rf build
	$(MAKE) -C hw1/1_mandelbrot clean
	$(MAKE) -C hw1/2_nbody clean
