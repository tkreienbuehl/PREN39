CC = g++
IDIR1 = -I/usr/local/include/opencv
IDIR2 = -I/usr/include
PARA = -O3 -Wall -c -fmessage-length=0
HEAD = $(CC) $(IDIR1) $(IDIR2) $(PARA)
SOURCE = ./src/source/
LIBS = -lopencv_photo -lgtest -lpthread -lX11 -lncurses -lopencv_video -lopencv_imgcodecs -lopencv_videoio -lopencv_highgui -lopencv_features2d -lopencv_calib3d -lopencv_videostab -lopencv_superres -lopencv_objdetect -lopencv_stitching -lopencv_imgproc -lopencv_ml -lopencv_shape -lopencv_hal -lopencv_core -lopencv_flann

all: $(SOURCE)GueselStar.cpp
	$(HEAD) $(SOURCE)RouteCalculation.cpp 
#-lopencv_core

	$(HEAD) ./src/source/RouteCalcMath.cpp 
#-lopencv_core

	$(HEAD) ./src/source/UARTSenderLogger.cpp

	$(HEAD) ./src/source/UARTReceiverLogger.cpp

	$(HEAD) ./src/source/RouteFinder.cpp 

	$(HEAD) ./src/source/ObjectFinder.cpp 

	$(HEAD) ./src/source/PictureCreator.cpp 

	$(HEAD) ./src/source/PrenController.cpp 

	$(HEAD) ./src/source/UARTHandler.cpp 

	$(HEAD) ./src/source/UARTReciever.cpp 

	$(HEAD) ./src/source/UARTSender.cpp 

	$(HEAD) ./src/source/PictureViewer.cpp 

	$(HEAD) ./src/source/DebugServer.cpp 

	$(HEAD) ./src/source/ConsoleView.cpp 

	$(HEAD) ./src/source/ConfigParser.cpp

	$(HEAD) ./src/source/PrenConfiguration.cpp

	$(HEAD) ./src/source/PIDCalculation.cpp

	$(HEAD) ./src/source/Gradient.cpp

	$(HEAD) ./src/test/ObjectFinderTest.cpp 

	$(HEAD) ./src/test/PictureCreator_test.cpp 

	$(HEAD) ./src/source/GueselStar.cpp 

	$(HEAD) ./src/source/Line.cpp 

	g++ -L/usr/local/lib -L/usr/lib -o GueselStar ObjectFinderTest.o PictureCreator_test.o UARTSenderLogger.o UARTReceiverLogger.o Gradient.o GueselStar.o ObjectFinder.o PictureCreator.o PictureViewer.o PrenController.o RouteFinder.o DebugServer.o ConfigParser.o PrenConfiguration.o ConsoleView.o PIDCalculation.o RouteCalculation.o RouteCalcMath.o UARTSender.o UARTReciever.o UARTHandler.o Line.o $(LIBS)

#rm *.o

clean:
	rm *o GueselStar

