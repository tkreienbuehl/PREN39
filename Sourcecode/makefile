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
#-lpthread -lX11 -lopencv_core

	$(HEAD) ./src/source/ObjectFinder.cpp 
#-lpthread -lX11 -lopencv_core

	$(HEAD) ./src/source/PictureCreator.cpp 
#-lpthread -lX11 -lopencv_core

	$(HEAD) ./src/source/PrenController.cpp 
#-lgtest -lpthread -lX11 -lopencv_core

	$(HEAD) ./src/source/UARTHandler.cpp 
#-lpthread -lX11

	$(HEAD) ./src/source/UARTReciever.cpp 
#-lpthread -lX11

	$(HEAD) ./src/source/UARTSender.cpp 
#-lpthread -lX11

	$(HEAD) ./src/source/PictureViewer.cpp 
#-lpthread -lX11 -lopencv_core -lopencv_highgui

	$(HEAD) ./src/source/DebugServer.cpp 
#-lpthread -lX11

	$(HEAD) ./src/source/ConsoleView.cpp 
#-lpthread -lX11 -lncurses

	$(HEAD) ./src/source/ConfigParser.cpp

	$(HEAD) ./src/source/PrenConfiguration.cpp

	$(HEAD) ./src/source/PIDCalculation.cpp

	$(HEAD) ./src/source/Gradient.cpp

	$(HEAD) ./src/test/ObjectFinderTest.cpp 
#-lgtest -lpthread -lX11 -lopencv_highgui -lopencv_core

	$(HEAD) ./src/test/PictureCreator_test.cpp 
#-lgtest -lpthread -lX11 -lopencv_highgui -lopencv_core

	$(HEAD) ./src/source/GueselStar.cpp 
#-lopencv_photo -lgtest -lpthread -lX11 -lncurses -lopencv_video -lopencv_imgcodecs -lopencv_videoio -lopencv_highgui -lopencv_features2d -lopencv_calib3d -lopencv_videostab -lopencv_superres -lopencv_objdetect -lopencv_stitching -lopencv_imgproc -lopencv_ml -lopencv_shape -lopencv_hal -lopencv_core -lopencv_flann

	g++ -L/usr/local/lib -L/usr/lib -o GueselStar ObjectFinderTest.o PictureCreator_test.o UARTSenderLogger.o UARTReceiverLogger.o Gradient.o GueselStar.o ObjectFinder.o PictureCreator.o PictureViewer.o PrenController.o RouteFinder.o DebugServer.o ConfigParser.o PrenConfiguration.o ConsoleView.o PIDCalculation.o RouteCalculation.o RouteCalcMath.o UARTSender.o UARTReciever.o UARTHandler.o $(LIBS)

#rm *.o

clean:
	rm *o GueselStar

