COMPILE=g++ src/main.cpp src/processing/ImageProcessor.cpp src/processing/ConnectedComponents.cpp src/QRCodeReader.cpp -o pqrcode.out -lpng -lm -O3

all:
	${COMPILE}

time:
	${COMPILE} -DTIME

large:
	${COMPILE} -DTIME -DCOMPONENT_CENTER_DIST_MARGIN=3

very_large:
	${COMPILE} -DTIME -DCOMPONENT_CENTER_DIST_MARGIN=5

debug:
	${COMPILE} -g

clean:
	rm *.out