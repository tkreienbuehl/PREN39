#include "../header/Gradient.hpp"

	// Class Gradient

	Gradient::Gradient(short partX, short partY) {
		m_xVal = partX;
		m_yVal = partY;
	}

	Gradient::Gradient() {
		m_xVal = 0;
		m_yVal = 0;
	}

	Gradient::~Gradient() {

	}

	void Gradient::setValue(short partX, short partY) {
		m_xVal = partX;
		m_yVal = partY;
	}

	short Gradient::getXValue() {
		return m_xVal;
	}

	short Gradient::getYValue() {
		return m_yVal;
	}

	short Gradient::getLength() {
		return static_cast<short>(sqrt(pow(m_xVal,2) + pow(m_yVal,2)));
	}
