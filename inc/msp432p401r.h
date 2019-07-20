/*
 * RS485_msp432p401r.h
 *
 *  Created on: 24 May 2017
 *  Author: Chia Jiun Wei
 * <J.W.Chia@tudelft.nl>
 */
 
#ifndef INCLUDE_RS485_MSP432P401R_H_
#define INCLUDE_RS485_MSP432P401R_H_

#define EUSCI_A0_PORT GPIO_PORT_P1					//GPIO_PORT_P# and GPIO_PIN# can be found in gpio.h
#define EUSCI_A0_TX   GPIO_PIN3
#define EUSCI_A0_RX   GPIO_PIN2
#define EUSCI_A0_PINS (EUSCI_A0_TX | EUSCI_A0_RX)

#define EUSCI_A1_PORT GPIO_PORT_P2				
#define EUSCI_A1_TX   GPIO_PIN3
#define EUSCI_A1_RX   GPIO_PIN2
#define EUSCI_A1_PINS (EUSCI_A1_TX | EUSCI_A1_RX)

#define EUSCI_A2_PORT GPIO_PORT_P3				
#define EUSCI_A2_TX   GPIO_PIN3
#define EUSCI_A2_RX   GPIO_PIN2
#define EUSCI_A2_PINS (EUSCI_A2_TX | EUSCI_A2_RX)

#define EUSCI_A3_PORT GPIO_PORT_P9				
#define EUSCI_A3_TX   GPIO_PIN7
#define EUSCI_A3_RX   GPIO_PIN6
#define EUSCI_A3_PINS (EUSCI_A3_TX | EUSCI_A3_RX)

#endif /* INCLUDE_RS485_MSP432P401R_H_ */