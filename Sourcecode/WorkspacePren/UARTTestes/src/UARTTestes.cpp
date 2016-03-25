//============================================================================
// Name        : UARTTestes.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <gtest/gtest.h>

#include "./header/UARTTestes.hpp"

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

using namespace std;

void BlinkLed(int uart0_filestream) {

	//----- TX BYTES -----
	unsigned char tx_buffer[20];

	tx_buffer[0] = 'L';
	tx_buffer[1] = 'E';
	tx_buffer[2] = 'D';
	tx_buffer[3] = '1';
	tx_buffer[4] = ' ';
	tx_buffer[5] = 'o';
	tx_buffer[6] = 'n';
	tx_buffer[7] = '\r';
	tx_buffer[8] = '\n';
	tx_buffer[9] = '\0';

	if (uart0_filestream != -1)
	{
		//cout << "sending bytes" << endl;
		int check = 10;
		int count = write(uart0_filestream, &tx_buffer[0], check);		//Filestream, bytes to write, number of bytes to write
		//for (int i = 0; i<= check; i++) {
			//count = write(uart0_filestream, &p_tx_buffer[i] , 2);
			if (count < 0)
			{
				cout << "UART TX error\n" << endl;
			}
		//}

		//cout << "Bits written: " << count << " Should write: " << check << endl;
	}
}

int main() {

	pthread_t threads;
	XInitThreads();
	int rc;

	cout << "!!!Hello UART tester!!!" << endl;


	//-------------------------
	//----- SETUP USART 0 -----
	//-------------------------
	//At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively
	int uart0_filestream = -1;

	//OPEN THE UART
	//The flags (defined in fcntl.h):
	//	Access modes (use 1 of these):
	//		O_RDONLY - Open for reading only.
	//		O_RDWR - Open for reading and writing.
	//		O_WRONLY - Open for writing only.
	//
	//	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
	//											if there is no input immediately available (instead of blocking). Likewise, write requests can also return
	//											immediately with a failure status if the output can't be written immediately.
	//
	//	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.
	uart0_filestream = open("/dev/ttyACM3", O_RDWR);		//Open in non blocking read/write mode
	if (uart0_filestream == -1)
	{
		//ERROR - CAN'T OPEN SERIAL PORT
		cout << "Error - Unable to open UART.  Ensure it is not in use by another application" << endl;
	}

	//CONFIGURE THE UART
	//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE:- CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem status lines
	//	CREAD - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)
	struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = CS8 | CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	options.c_cc[VMIN] = 1;
	options.c_cc[VTIME] = 5;

    cfsetospeed(&options,B115200);            // 115200 baud
    cfsetispeed(&options,B115200);            // 115200 baud

	tcsetattr(uart0_filestream, TCSANOW, &options);
	tcsetattr(uart0_filestream, TCSAFLUSH, &options);


	UARTReciever* reciever = new UARTReciever(uart0_filestream);

	rc = pthread_create(&threads, NULL, UARTReciever::staticEntryPoint, reciever);
	if (rc) {
		cout << "Error:unable to create thread," << rc << endl;
		return(-1);
	}
	usleep(1000000);

	//while (cnt<10000000) {
		BlinkLed(uart0_filestream);
		//cnt++;
	//}

	//----- CLOSE THE UART -----
	reciever->stopReading();
	usleep(1000000);

	delete reciever;

	close(uart0_filestream);

	cout <<"Bye bye" << endl;

	return 0;
}
