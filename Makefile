all:
	g++ src/main.cpp src/processing/ImageProcessor.cpp src/processing/ConnectedComponents.cpp src/QRCodeReader.cpp -o pqrcode.out -lpng -lm

debug:
	g++ src/main.cpp src/processing/ImageProcessor.cpp src/processing/ConnectedComponents.cpp src/QRCodeReader.cpp -o pqrcode.out -lpng -lm -g

clean:
	rm *.out