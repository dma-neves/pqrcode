all:
	g++ src/main.cpp src/processing/ImageProcessor.cpp src/processing/ConnectedComponents.cpp src/QRCodeReader.cpp -o pqrcode.out -lpng -lm

time:
	g++ src/main.cpp src/processing/ImageProcessor.cpp src/processing/ConnectedComponents.cpp src/QRCodeReader.cpp -o pqrcode.out -lpng -lm -DTIME

large:
	g++ src/main.cpp src/processing/ImageProcessor.cpp src/processing/ConnectedComponents.cpp src/QRCodeReader.cpp -o pqrcode.out -lpng -lm -DTIME -DCOMPONENT_CENTER_DIST_MARGIN=5

debug:
	g++ src/main.cpp src/processing/ImageProcessor.cpp src/processing/ConnectedComponents.cpp src/QRCodeReader.cpp -o pqrcode.out -lpng -lm -g

clean:
	rm *.out