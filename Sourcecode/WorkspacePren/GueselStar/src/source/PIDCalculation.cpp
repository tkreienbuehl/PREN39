/*
 * PIDCalculation.cpp
 *
 *  Created on: Apr 29, 2016
 *      Author: tobias
 */

#include "../header/PIDCalculation.h"

PIDCalculation::PIDCalculation() {
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

}

PIDCalculation::~PIDCalculation() {

}

void PIDCalculation::pidDoWork(int& calcVal) {
    // readSpeed from Encoder(in value nomValue)
    m_nomValueOld = m_nomValue;            //ist Wert (old)
    m_nomValue=calcVal;

    m_nomValue = (m_nomValueOld+m_nomValue) >> 1;

  if (m_setValue == 0) {
	  m_val = m_integ = m_devOld = 0;
  }
  else {
    // deviation (max devL = +1000 - -1000 = 2000)
    //dev = (setValue - nomValue) / 8;
     m_dev = (m_setValue - m_nomValue);
     if(m_dev<5 && m_dev>-5){
         //sendSpdReached();
     }
    // P-Part (max kpL =
    m_val = (m_KP * m_dev) / 32;

    // I-Part with anti-windup
    if (m_KI != 0) m_integ += m_dev;
    m_val += static_cast<int>(m_KI * m_integ) / 32;

    // D-Part
    m_val += static_cast<int>(m_KD*(m_dev-m_devOld))/32;
    m_devOld = m_dev;
    m_val=m_val/4;
    // limit control point
    if (m_val > 100)
    {
      m_val = 100;
      m_integ -= m_dev;
    }
    else if (m_val < 1)
    {
      m_val = 0;
      m_integ -= m_dev;
    }
  }
  //debugPrintfDCDrive("%i\r\n\0",val);
}