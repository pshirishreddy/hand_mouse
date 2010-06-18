edit : main.o
main.o : HandGesture.cpp haar_detect.h mouse.h
	g++ -lX11 `pkg-config opencv --cflags --libs` HandGesture.cpp -o main.o
clean :
	rm main.o 
