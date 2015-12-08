#include "../header/GradientMat.hpp"

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

	// Class GradientMat <<singleton>>

	GradientMat* GradientMat::m_TheInstance = 0;

	GradientMat::GradientMat(short rows, short cols) {
		if (rows==0 || cols==0 ) {
			cout << "Error in constructing matrice, rows or cold are zero" << endl;
		}
		else {
			m_Rows = rows;
			m_Cols = cols;
			m_matPtr = new Gradient*[rows];
			for (int i = 0 ; i<rows ; i++) {
				m_matPtr[i] = new Gradient[cols];
				for (int j = 0; j< cols; j++) {
					m_matPtr[i][j] = Gradient();
				}
			}
		}
	}

	GradientMat::~GradientMat() {
		for (int i = 0; i<m_Rows; i++) {
			delete[] m_matPtr[i];
		}
		delete[] m_matPtr;
	}

	GradientMat* GradientMat::getInstance(short rows, short cols) {
		if (m_TheInstance == 0) {
			m_TheInstance = new GradientMat(rows, cols);
		}
		return m_TheInstance;
	}

	void GradientMat::setValue(short row, short col, Gradient val) {
		m_matPtr[row][col] = val;
	}

	Gradient GradientMat::getValue(short row, short col) {
		return m_matPtr[row][col];
	}
