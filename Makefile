all: main.c src/physics.c src/drawing.c src/entities.c src/maths.c
	gcc -Wall -Wextra -Werror -o rpart main.c src/drawing.c src/physics.c src/entities.c src/maths.c -lraylib -lm -ldl -pthread

o3: main.c src/physics.c src/drawing.c src/entities.c src/maths.c
	gcc -O3 -Wall -Wextra -Werror -o rpart main.c src/drawing.c src/physics.c src/entities.c src/maths.c -lraylib -lm -ldl -pthread


clean: rpart
	rm rpart

run: rpart
	./rpart

valgrind: rpart
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./rpart
