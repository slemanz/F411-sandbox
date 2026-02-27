CC= gcc
CFLAGS = -c -Wall -std=gnu99 -O0

BUILD_DIR 	= build
ROOT_DIR = ..
COMMON_DIR = $(ROOT_DIR)/common

INCLUDES += -I mocks
INCLUDES += -I framework
INCLUDES += -I $(COMMON_DIR)/Inc

PROD 	+= $(BUILD_DIR)/simple-timer.o
PROD 	+= $(BUILD_DIR)/ring-buffer.o
PROD 	+= $(BUILD_DIR)/pool.o

TESTS 	+= $(BUILD_DIR)/AllTestRunner.o
TESTS 	+= $(BUILD_DIR)/test_simple_timer.o
TESTS 	+= $(BUILD_DIR)/test_ring_buffer.o
TESTS 	+= $(BUILD_DIR)/test_pool.o

MOCKS 	+= $(BUILD_DIR)/mock_timebase.o

UNITY 	+= $(BUILD_DIR)/unity_fixture.o
UNITY 	+= $(BUILD_DIR)/unity.o

all: $(BUILD_DIR) test

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(COMMON_DIR)/Src/shared/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$(*).o $(COMMON_DIR)/Src/shared/$(*).c

$(BUILD_DIR)/%.o: $(COMMON_DIR)/Src/core/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$(*).o $(COMMON_DIR)/Src/core/$(*).c

$(BUILD_DIR)/%.o: $(COMMON_DIR)/Src/bsp/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$(*).o $(COMMON_DIR)/Src/bsp/$(*).c

$(BUILD_DIR)/%.o: framework/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$(*).o framework/$(*).c

$(BUILD_DIR)/%.o: unit/shared/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$(*).o unit/shared/$(*).c

$(BUILD_DIR)/%.o: unit/core/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$(*).o unit/core/$(*).c

$(BUILD_DIR)/%.o: unit/bsp/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$(*).o unit/bsp/$(*).c

$(BUILD_DIR)/%.o: mocks/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$(*).o mocks/$(*).c

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$(*).o $(*).c

test: $(PROD) $(MOCKS) $(UNITY) $(TESTS) 
	$(CC) $^ -o $(BUILD_DIR)/run_tests
	./$(BUILD_DIR)/run_tests -v


clean:
	rm -rf $(BUILD_DIR)/*