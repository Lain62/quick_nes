.PHONY: clean
CFLAGS=-Wall -Wextra

quick_nes: main.c cpu.o cpu.h
	$(CC) main.c cpu.o $(CFLAGS) -o quick_nes $(LDFLAGS)

cpu.o: cpu.c cpu.h
	$(CC) -c cpu.c $(CFLAGS) -o cpu.o $(LDFLAGS)


clean:
	rm -rf ./*.o
	rm -rf ./quick_nes
