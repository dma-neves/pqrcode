all:
	g++ src/main.cpp src/processing/ImageProcessor.cpp src/processing/ConnectedComponents.cpp src/QRCodeReader.cpp -o pqrcode.out -lpng -lm -fopenmp

time:
	g++ src/main.cpp src/processing/ImageProcessor.cpp src/processing/ConnectedComponents.cpp src/QRCodeReader.cpp -o pqrcode.out -lpng -lm -fopenmp -DTIME

large:
	g++ src/main.cpp src/processing/ImageProcessor.cpp src/processing/ConnectedComponents.cpp src/QRCodeReader.cpp -o pqrcode.out -lpng -lm -fopenmp -DTIME -DTIME -DCOMPONENT_CENTER_DIST_MARGIN=5

debug:
	g++ src/main.cpp src/processing/ImageProcessor.cpp src/processing/ConnectedComponents.cpp src/QRCodeReader.cpp -o pqrcode.out -lpng -lm -fopenmp -g

clean:
	rm *.out