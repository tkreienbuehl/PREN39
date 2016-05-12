/*
 * PIDCalculation.h
 *
 *  Created on: Apr 29, 2016
 *      Author: tobias
 */

#pragma once

#include "PrenConfiguration.hpp"
#include "PrenController.hpp"
#include <stdint.h>

using namespace std;

class PIDCalculation {
public:
	PIDCalculation(PrenController* controller);
	~PIDCalculation();

	void pidDoWork(int& calcVal);

private:
	uint16_t m_actSpeedValue;
	uint16_t m_nomValue;
	uint16_t m_nomValueOld;
	uint16_t m_setValue;
	uint16_t m_setValueOld;
	int m_integ;
	int m_devOld;
	int m_val,m_dev;
	uint8_t m_KP;
	uint8_t m_KI;
	uint8_t m_KD;
	PrenController* m_Controller;
};
