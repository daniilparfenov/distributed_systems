BUILD_DIR = ../../build
INCLUDE_DIR = ../../include

CC = clang
CFLAGS = -Wall -Wextra -O3 -I$(INCLUDE_DIR)
LDFLAGS = -lm -lpthread


TARGETS = $(BUILD_DIR)/mandelbrot.elf $(BUILD_DIR)/mandelbrot_scalar.elf

all: $(TARGETS)

# mandelbrot_parallel
$(BUILD_DIR)/mandelbrot.elf: mandelbrot.c $(INCLUDE_DIR)/my_rand.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# mandelbrot_scalar
$(BUILD_DIR)/mandelbrot_scalar.elf: mandelbrot_scalar.c $(INCLUDE_DIR)/my_rand.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(BUILD_DIR)/mandelbrot.elf $(BUILD_DIR)/mandelbrot_scalar.elf
