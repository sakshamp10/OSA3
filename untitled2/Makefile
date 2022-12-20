CC=gcc -Wall
run=P1

exec: $(run)

# make run="P1"
P1: p1.c
	$(CC) p1.c -o P1
	./P1

# make run="P2"
P2: p2.c
	$(CC) p2.c -o P2
	./P2

clean:
	find . -maxdepth 1 -type p -delete
	rm -rf P1
	rm -rf P2
