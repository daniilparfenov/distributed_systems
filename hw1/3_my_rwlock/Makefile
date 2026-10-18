BUILD_DIR = ../../build
INCLUDE_DIR = ../../include

CC = clang
CFLAGS = -Wall -Wextra -O3 -I$(INCLUDE_DIR)
LDFLAGS = -lm -lpthread


TARGETS = $(BUILD_DIR)/rwlock.elf $(BUILD_DIR)/my_rwlock.elf

all: $(TARGETS)

# rwlock from pthread
$(BUILD_DIR)/rwlock.elf: pth_ll_rwl.c $(INCLUDE_DIR)/my_rand.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# own rwlock
$(BUILD_DIR)/my_rwlock.elf: my_rwlock_main.c $(INCLUDE_DIR)/my_rand.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(BUILD_DIR)/rwlock.elf $(BUILD_DIR)/my_rwlock.elf
