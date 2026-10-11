CC = clang
CFLAGS = -Wall -Wextra -O3 -I../../include
LDFLAGS = -lm -lpthread

BUILD_DIR = ../../build

TARGETS = $(BUILD_DIR)/mandelbrot.elf $(BUILD_DIR)/mandelbrot_scalar.elf

all: $(TARGETS)

# mandelbrot_parallel
$(BUILD_DIR)/mandelbrot.elf: mandelbrot.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# mandelbrot_scalar
$(BUILD_DIR)/mandelbrot_scalar.elf: mandelbrot_scalar.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(BUILD_DIR)/mandelbrot.elf $(BUILD_DIR)/mandelbrot_scalar.elf
