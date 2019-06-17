all: exe run

exe: main.cpp RgbImage.cpp
	g++ -o exe main.cpp RgbImage.cpp -framework GLUT -framework OpenGL -Wno-deprecated

run: exe
	./exe

clean:
	rm exe