SRC = main.c src/physics.c src/drawing.c src/entities.c src/maths.c src/quadtree.c src/unigrid.c
RAY_FLAGS = -lraylib -lm -ldl -pthread
FLAGS = -Wall -Wextra -Werror -g


all: $(SRC)
	gcc $(FLAGS) -o rpart $(SRC) $(RAY_FLAGS)

addr: main.c src/physics.c src/drawing.c src/entities.c src/maths.c src/quadtree.c
	gcc -Wall -Wextra -Werror -o rpart main.c src/drawing.c src/physics.c src/entities.c src/maths.c src/quadtree.c -lraylib -lm -ldl -pthread -fsanitize=address -fno-omit-frame-pointer -g

clean: rpart
	rm rpart

run: rpart
	./rpart

valgrind: rpart
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./rpart
