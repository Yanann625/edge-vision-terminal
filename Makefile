# 边缘视觉采集终端 Makefile
# 本地编译:   make
# 交叉编译:   make CC=arm-linux-gnueabihf-gcc

CC      ?= gcc
CFLAGS  ?= -std=c99 -Wall -Wextra -O2 -g -pthread
# 性能剖析可加 -pg（gprof）；需要 perf 时移除 -O2 用 -O0 更准
LDFLAGS ?= -pthread

TARGET  := edge-vision
SRCDIR  := src
SRCS    := $(wildcard $(SRCDIR)/*.c)
OBJS    := $(SRCS:.c=.o)
DEPS    := $(SRCS:.c=.d)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 依赖自动生成（头文件改动触发重编）
-include $(DEPS)
%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# 交叉编译快捷方式（用法: make cross）
cross: CC=arm-linux-gnueabihf-gcc
cross: all

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)
