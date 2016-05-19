/*
 * PIDCalculation.cpp
 *
 *  Created on: Apr 29, 2016
 *      Author: tobias
 */

#include "../header/PIDCalculation.hpp"

PIDCalculation::PIDCalculation(PrenController* controller) {
	PrenConfiguration conf;
	m_actSpeedValue=0;
	m_nomValue=0;       //OFF
	m_nomValueOld=0;    //istwert
	m_setValue=0;
	m_setValueOld=0;    //sollwert
	m_integ=0;
	m_devOld=0;
	m_val= 0;
	m_dev = 0;
	m_KP=conf.P_PART;	//71
	m_KI=conf.I_PART;	//28
	m_KD=conf.D_PART;	//5
	m_Controller = controller;
}

PIDCalculation::~PIDCalculation() {

}

void PIDCalculation::pidDoWork(int& calcVal) {
	// readSpeed from Encoder(in value nomValue)
	m_nomValueOld = m_nomValue;            //ist Wert (old)
	m_nomValue=calcVal;

	//m_nomValue = (m_nomValueOld+m_nomValue)/2;

	m_dev = (m_setValue + m_nomValue);
	// P-Part (max kpL =
	m_val = (m_KP * m_dev);

	// I-Part with anti-windup
	if (m_KI != 0) m_integ += m_dev;
	m_val += static_cast<int>(m_KI * m_integ);

	// D-Part
	m_val += static_cast<int>(m_KD * (m_dev-m_devOld));
	m_devOld = m_dev;
	m_val /= 4;

	char str[30];
	bzero(str, sizeof(str));
	sprintf(str, "Calculated Value: %d", m_val);
	m_Controller->printString(str, m_Controller->ROUTE_FINDER, 8);

	// limit control point
	if (m_val > 125)
	{
		m_val = 125;
		m_integ -= m_dev;
	}
	else if (m_val < -125)
	{
		m_val = -125;
		m_integ -= m_dev;
	}
	m_Controller->setSteeringAngle((m_val+125));
}
