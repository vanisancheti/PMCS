CC = g++ -w -g -Wall -I ../boost_1_74_0

pmcs: main.o Graph.o Vector2D.o Triplet.o
	$(CC) -o pmcs main.o Vector2D.o Triplet.o Graph.o

main.o: main.cpp Graph.h Vector2D.h
	$(CC) -c main.cpp

Graph.o: Graph.cpp Graph.h Triplet.h
	$(CC) -c Graph.cpp

Triplet.o: Triplet.cpp Triplet.h
	$(CC) -c Triplet.cpp

Vector2D.o: Vector2D.cpp Vector2D.h
	$(CC) -c Vector2D.cpp

clean:
	rm -f pmcs *.o