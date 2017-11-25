
textrendering.o:
	mkdir -p bin
	g++ -std=c++11 -Wall -Wno-unused-function -g -I ./include/ -o./bin/textrendering.o -c src/textrendering.cpp 

obj.o:
	mkdir -p bin
	g++ -std=c++11 -Wall -Wno-unused-function -g -I ./include/ -o./bin/obj.o -c ./src/obj.cpp 

utils.o:
	mkdir -p bin
	g++ -std=c++11 -Wall -Wno-unused-function -g -I ./include/ -o./bin/utils.o -c ./src/utils.cpp 

matrices.o:
	mkdir -p bin
	g++ -std=c++11 -Wall -Wno-unused-function -g -I ./include/ -o./bin/matrices.o -c ./src/matrices.cpp 
	 
./bin/Linux/main: src/main.cpp src/glad.c matrices.o utils.o obj.o textrendering.o include/dejavufont.h
	mkdir -p bin/Linux
	g++ -std=c++11 -Wall -Wno-unused-function -g -I ./include/ -o ./bin/Linux/main src/main.cpp bin/matrices.o bin/utils.o bin/obj.o bin/textrendering.o src/glad.c src/tiny_obj_loader.cpp ./lib-linux/libglfw3.a -lrt -lm -ldl -lX11 -lpthread -lXrandr -lXinerama -lXxf86vm -lXcursor

.PHONY: clean run
clean:
	rm -f bin/Linux/main
	rm -f bin/*.o

run: ./bin/Linux/main
	cd bin/Linux && ./main
