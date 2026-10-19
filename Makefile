.PHONY: all mandelbrot clean

all: mandelbrot n_body my_rwlock

mandelbrot:
	@echo "Building Mandelbrot..."
	$(MAKE) -C hw1/1_mandelbrot

n_body:
	@echo "Building N-body..."
	$(MAKE) -C hw1/2_nbody

my_rwlock:
	@echo "Building my_rwlock..."
	$(MAKE) -C hw1/3_my_rwlock

clean:
	@echo "Cleaning up..."
	@rm -rf build
	$(MAKE) -C hw1/1_mandelbrot clean
	$(MAKE) -C hw1/2_nbody clean
	$(MAKE) -C hw1/3_my_rwlock clean
