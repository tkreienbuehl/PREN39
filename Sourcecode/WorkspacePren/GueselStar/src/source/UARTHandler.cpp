#include "../header/UARTHandler.hpp"

	UARTHandler::UARTHandler() {
		m_uart0_filestream =-1;
		m_waitFlag = 0;
		m_prenConfig = new PrenConfiguration();
		if(!m_prenConfig->IS_ON_IDE) {
			m_ConsoleView = ConsoleView::getInstance();
		}
	}

	UARTHandler::~UARTHandler() {
		close(m_uart0_filestream);
	}

	int UARTHandler::getFileStream() {
		return m_uart0_filestream;
	}

	bool UARTHandler::openSerialIF(const char* ifName) {
		m_uart0_filestream = open(ifName, O_RDWR | O_NONBLOCK);	//Open in non blocking read/write mode
		if (m_uart0_filestream == -1)
		{
			if(!m_prenConfig->IS_ON_IDE) {
				m_ConsoleView->setUARTStateText( "Error - Unable to open UART.",0);
				m_ConsoleView->setUARTStateText( "Ensure it is not in use by another application and you have permission to use", 1);
			} else {
				cout << "Error - Unable to open UART." << endl;
				cout << "Ensure it is not in use by another application and you have permission to use" << endl;
			}
			usleep(1000000);
			return false;
		}
		return true;
	}

	bool UARTHandler::setUartConfig(BaudRateE baudRate) {
		struct termios options;

		if (tcgetattr(m_uart0_filestream, &options) !=0) {
			if(!m_prenConfig->IS_ON_IDE) {
				m_ConsoleView->setUARTStateText("Error reading config parameters",1);
			} else {
				cout << "Error reading config parameters" << endl;
			}
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
			if(!m_prenConfig->IS_ON_IDE) {
				m_ConsoleView->setUARTStateText("Error writing config parameters",1);
			} else {
				cout << "Error writing config parameters" << endl;
			}
			close(m_uart0_filestream);
			return false;
		}
		tcsetattr(m_uart0_filestream, TCSAFLUSH, &options);
		return true;
	}
