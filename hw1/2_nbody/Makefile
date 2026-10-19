BUILD_DIR = ../../build
INCLUDE_DIR = ../../include

CC = clang
CFLAGS = -Wall -Wextra -O3 -I$(INCLUDE_DIR)
LDFLAGS = -lm -lpthread

TARGETS = $(BUILD_DIR)/n_body_scalar.elf $(BUILD_DIR)/n_body.elf

all: $(TARGETS)

# n_body_scalar
$(BUILD_DIR)/n_body_scalar.elf: n_body_scalar.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# n_body_parallel
$(BUILD_DIR)/n_body.elf: n_body.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(BUILD_DIR)/n_body_scalar.elf
	rm -f $(BUILD_DIR)/n_body.elf
