#include "header/UARTHandler.hpp"

	UARTHandler::UARTHandler() {
		m_uart0_filestream =-1;
		m_waitFlag = 0;
	}

	UARTHandler::~UARTHandler() {
		close(m_uart0_filestream);
	}

	int UARTHandler::getFileStream() {
		return m_uart0_filestream;
	}

	bool UARTHandler::openSerialIF(char* ifName) {
		m_uart0_filestream = open(ifName, O_RDWR | O_NONBLOCK);	//Open in non blocking read/write mode
		if (m_uart0_filestream == -1)
		{
			cout << "Error - Unable to open UART.  Ensure it is not in use by another application and you have permission to use" << endl;
			return false;
		}
		return true;
	}

	bool UARTHandler::setUartConfig(BaudRateE baudRate) {
		struct termios options;

		if (tcgetattr(m_uart0_filestream, &options) !=0) {
			cout << "error" << endl;
			return false;
		}
	    cfsetospeed(&options,baudRate);
	    cfsetispeed(&options,baudRate);

		options.c_cflag = CREAD | CS8;
		options.c_iflag = IGNPAR;
		options.c_oflag = 0;
		options.c_lflag = 0;
		options.c_cc[VMIN] = 0;
		options.c_cc[VTIME] = 5;

		if (tcsetattr(m_uart0_filestream, TCSANOW, &options) != 0 ) {
			cout << "no a error" << endl;
			close(m_uart0_filestream);
			return false;
		}
		tcsetattr(m_uart0_filestream, TCSAFLUSH, &options);
		return true;
	}
