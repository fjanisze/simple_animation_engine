all:
	g++ -std=c++1y -pthread -I../SFML/SFML/include/ -I../gmock-1.7.0/fused-src/ *.cpp -o ./bin/animation.exe -L../SFML/SFML/lib/ -L../gmock-1.7.0/ -DSFML_STATIC -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lgmock

