.PHONY: clean
CFLAGS=-Wall -Wextra

quick_nes: main.c cpu.o cpu.h tests.o tests.h
	$(CC) main.c cpu.o tests.o $(CFLAGS) -o quick_nes $(LDFLAGS)

cpu.o: cpu.c cpu.h
	$(CC) -c cpu.c $(CFLAGS) -o cpu.o $(LDFLAGS)

tests.o: tests.c tests.h
	$(CC) -c tests.c $(CFLAGS) -o tests.o $(LDFLAGS)

clean:
	rm -rf ./*.o
	rm -rf ./quick_nes
