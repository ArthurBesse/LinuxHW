include ../global_makevars


all: a.cpp b.cpp
	$(CC) $(CFLAGS) -c a.cpp
	$(CC) $(CFLAGS) a.o $(LFLAGS) -o a
	$(CC) $(CFLAGS) -c b.cpp
	$(CC) $(CFLAGS) b.o $(LFLAGS) -o b
clean: 
	rm a b
