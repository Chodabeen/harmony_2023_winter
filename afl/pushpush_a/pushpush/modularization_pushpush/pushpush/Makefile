JSON = "../game_data/map1.json"
IP = "127.0.0.1"
PORT = "9990"

LIBS = `pkg-config --libs --cflags gtk+-2.0`
CC = gcc
CFlAGS = -O0 --coverage
TEST_TARGET = test_read_write test_loadJson test_disconnect test_send_msg_all

compile: server client

pushpush_server.o: pushpush_server.c
	$(CC) $(CFlAGS) -c $^ -DTEST -pthread

pushpush_client.o: pushpush_client.c
	$(CC) $(CFlAGS) -c $^ -DTEST $(LIBS) -pthread

.c.o :
	$(CC) -c -o $@ $<

test_read_write : test_read_write.o pushpush_server.o cJSON.o
	$(CC) $(CFlAGS) $^ -o $@

test_loadJson : test_loadJson.o pushpush_server.o cJSON.o
	$(CC) $(CFlAGS) $^ -o $@

test_disconnect : test_disconnect.o pushpush_server.o cJSON.o
	$(CC) $(CFlAGS) $^ -o $@

test_send_msg_all : test_send_msg_all.o pushpush_server.o cJSON.o
	$(CC) $(CFlAGS) $^ -o $@ -pthread


server: pushpush_server.c cJSON.o
	$(CC) $(CFlAGS) -o $@  $^  -pthread 
client: pushpush_client.o cJSON.o
	$(CC) $(CFlAGS) -o $@ $^  $(LIBS) -DDEBUG
rs:
	echo $(JSON) | ./server $(PORT)
rc:
	./client $(IP) $(PORT)


check_gocv:
	gcov pushpush_server.gcda -bf
clean:
	rm *.o
clean_exec:
	rm server client
clean_gcov:
	rm *.gc*
clean_test_target:
	rm $(TEST_TARGET)

