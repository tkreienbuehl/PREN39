all: ./src/source/GueselStar.cpp
	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/RouteCalculation.cpp -lopencv_core

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/RouteCalcMath.cpp -lopencv_core

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/UARTLogger.cpp

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/RouteFinder.cpp -lpthread -lX11 -lopencv_core

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/ObjectFinder.cpp -lpthread -lX11 -lopencv_core

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/PictureCreator.cpp -lpthread -lX11 -lopencv_core

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/PrenController.cpp -lgtest -lpthread -lX11 -lopencv_core

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/UARTHandler.cpp -lpthread -lX11

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/UARTReciever.cpp -lpthread -lX11

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/UARTSender.cpp -lpthread -lX11

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/PictureViewer.cpp -lpthread -lX11 -lopencv_core -lopencv_highgui

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/DebugServer.cpp -lpthread -lX11

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/ConsoleView.cpp -lpthread -lX11 -lncurses

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/ConfigParser.cpp

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/PrenConfiguration.cpp

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/PIDCalculation.cpp

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/Gradient.cpp

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/test/ObjectFinderTest.cpp -lgtest -lpthread -lX11 -lopencv_highgui -lopencv_core

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/test/PictureCreator_test.cpp -lgtest -lpthread -lX11 -lopencv_highgui -lopencv_core

	g++ -I/usr/local/include/opencv -I/usr/include -Wall -c ./src/source/GueselStar.cpp -lopencv_photo -lgtest -lpthread -lX11 -lncurses -lopencv_video -lopencv_imgcodecs -lopencv_videoio -lopencv_highgui -lopencv_features2d -lopencv_calib3d -lopencv_videostab -lopencv_superres -lopencv_objdetect -lopencv_stitching -lopencv_imgproc -lopencv_ml -lopencv_shape -lopencv_hal -lopencv_core -lopencv_flann

	g++ -L/usr/local/lib -L/usr/lib -o "GueselStar"  ObjectFinderTest.o PictureCreator_test.o UARTLogger.o Gradient.o GueselStar.o ObjectFinder.o PictureCreator.o PictureViewer.o PrenController.o RouteFinder.o DebugServer.o ConfigParser.o PrenConfiguration.o ConsoleView.o PIDCalculation.o RouteCalculation.o RouteCalcMath.o UARTSender.o UARTReciever.o UARTHandler.o -lopencv_photo -lgtest -lpthread -lX11 -lncurses -lopencv_video -lopencv_imgcodecs -lopencv_videoio -lopencv_highgui -lopencv_features2d -lopencv_calib3d -lopencv_videostab -lopencv_superres -lopencv_objdetect -lopencv_stitching -lopencv_imgproc -lopencv_ml -lopencv_shape -lopencv_hal -lopencv_core -lopencv_flann

