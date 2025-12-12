CC = gcc
CFLAGS = -Wall -ansi -pedantic -g

TARGET = assembler

SRC_DIRS = Src/Assembler Src/File Src/TextEditor Src/McroReader Src/Architecture Src/LineParser Src/FirstPass Src/SecondPass Src/FilesCreator Src/MemoryCleaner Src/Common

SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

OBJS = $(SRCS:.c=.o)

INCLUDES = -ISrc/Common -ISrc/File -ISrc/TextEditor -ISrc/Assembler -ISrc/McroReader -ISrc/Architecture -ISrc/LineParser -ISrc/FirstPass -ISrc/SecondPass -ISrc/FilesCreator -ISrc/MemoryCleaner

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
