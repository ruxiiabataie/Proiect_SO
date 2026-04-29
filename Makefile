CC = gcc
CFLAGS = -Wall -Wextra

SRC = \
src/main.c \
src/commands/commands.c \
src/file/file_ops.c \
src/filter/filter.c \
src/permissions/permissions.c

OUT = city_manager

all:
	$(CC) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)