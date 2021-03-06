/******************************************************************************
 
     * File:                USCI_I2C.h
     * Description:         Header file for the USCI_I2C.c
     * Created:             08 oct 2019
     * Author:              Andrey Bibea
     * Email:               andreybibea@gmail.com

******************************************************************************/

#ifndef USCI_I2C_H_
#define USCI_I2C_H_


unsigned char  USCI_I2C_ACKed;						// Flag that means success transaction
unsigned char *USCI_I2C_ptr_TX;						// Pointer to TX data
unsigned char  USCI_I2C_ctr_TX;						// Num of remaining TX data
unsigned char *USCI_I2C_ptr_RX;						// Pointer to RX data
unsigned char  USCI_I2C_ctr_RX;						// Num of remaining RX data


void USCI_I2C_Init(void);

void USCI_I2C_RX_Init(unsigned char slaveAddr, unsigned char prescale);

void USCI_I2C_TX_Init(unsigned char slaveAddr, unsigned char prescale);

void USCI_I2C_RX(unsigned char RXdataCtr, unsigned char *RXdataPtr);

unsigned char USCI_I2C_RX_single_noIV(unsigned char slaveAddr, unsigned char reg);

void USCI_I2C_TX(unsigned char TXdataCtr, unsigned char *TXdataPtr);

unsigned char USCI_I2C_NotReady(void);


#endif /* USCI_I2C_H_ */
