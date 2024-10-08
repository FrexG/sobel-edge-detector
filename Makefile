CC := clang
CFLAGS := -Wall -Wextra

LIBS := `pkg-config --libs --cflags raylib`


SRC  := main.c
OBJ  := sobel.o
MAIN := sobel 

$(MAIN):$(OBJ)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJ) $(LIBS)

$(OBJ):$(SRC)
	$(CC) $(CFLAGS) -o $(OBJ) -c $(SRC) $(LIBS)

clean:
	rm *.o && rm sobel 
