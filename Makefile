CC = gcc

CFLAGS = -Wall -Wextra

SRC = \
src/main.c \
src/commands/commands.c \
src/file/file_ops.c \
src/filter/filter.c \
src/permissions/permissions.c

OUT = city_manager

all: $(OUT) monitor city_hub scorer

$(OUT):
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

monitor:
	$(CC) $(CFLAGS) src/monitor_reports.c -o monitor_reports

city_hub:
	$(CC) $(CFLAGS) src/city_hub.c -o city_hub

scorer:
	$(CC) $(CFLAGS) src/scorer.c -o scorer

clean:
	rm -f \
	$(OUT) \
	monitor_reports \
	city_hub \
	scorer