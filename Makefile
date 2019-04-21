CC = g++
CFLAGS = -O3 -std=c++11
EXE = floorplanner

all:
	$(CC) $(CFLAGS) main.cpp Floorplanner.cpp BinaryTree.cpp DoublyLinkedList.cpp -o $(EXE)
clean:
	rm -f $(EXE)
	rm -f initial.gp
	rm -f result.gp
plot:
	gnuplot -persist initial.gp
	gnuplot -persist result.gp
case0:
	(time ./Floorplanner 0.5 input_pa2/input.block input_pa2/input.nets input.out)
	./checker input_pa2/input.block input_pa2/input.nets input.out 0.5
case1:
	(time ./Floorplanner 0.5 input_pa2/ami33.block input_pa2/ami33.nets ami33.out)
	./checker input_pa2/ami33.block input_pa2/ami33.nets ami33.out 0.5
case2:
	(time ./Floorplanner 0.5 input_pa2/ami49.block input_pa2/ami49.nets ami49.out)
	./checker input_pa2/ami49.block input_pa2/ami49.nets ami49.out 0.5
case3:
	(time ./Floorplanner 0.5 input_pa2/apte.block input_pa2/apte.nets apte.out)
	./checker input_pa2/apte.block input_pa2/apte.nets apte.out 0.5
case4:
	(time ./Floorplanner 0.5 input_pa2/hp.block input_pa2/hp.nets hp.out)
	./checker input_pa2/hp.block input_pa2/hp.nets hp.out 0.5
case5:
	(time ./Floorplanner 0.5 input_pa2/xerox.block input_pa2/xerox.nets xerox.out)
	./checker input_pa2/xerox.block input_pa2/xerox.nets xerox.out 0.5

