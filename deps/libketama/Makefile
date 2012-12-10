PREFIX=$(DESTDIR)/usr
CFLAGS+=-fPIC -W -Wall -Werror

build:
	gcc $(CFLAGS) -O3 -c md5.c
	gcc $(CFLAGS) -O3 -c ketama.c
	
debug:
	gcc $(CFLAGS) -g -DDEBUG -o md5-debug.o -c md5.c
	gcc $(CFLAGS) -g -DDEBUG -o ketama-debug.o -c ketama.c
	
test: build
	LD_LIBRARY_PATH=. gcc $(CFLAGS) -I. -O3 -o ketama_test ketama_test.c md5.o ketama.o
	@./test.sh
	@./downed_server_test.sh

clean:
	rm -f *.o
	rm -f ketama_test
