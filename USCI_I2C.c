#include "cc430x513x.h"
#include "USCI_I2C.h"


void USCI_I2C_Init(void) {
	PMAPPWD = 0x02D52;							// Get write-access to port mapping registers
	P1MAP3 = PM_UCB0SDA;						// Map UCB0SDA output to P1.3
	P1MAP2 = PM_UCB0SCL;						// Map UCB0SCL output to P1.2
	PMAPPWD = 4;								// Lock port mapping registers
}


void USCI_I2C_RX_Init(unsigned char slaveAddr, unsigned char prescale) {
	P1SEL |= BIT2 + BIT3;						// Select P1.2 & P1.3 to I2C function
	UCB0CTL1 |= UCSWRST;						// Enable SW reset
	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;		// I2C Master, synchronous mode
	UCB0CTL1 = UCSSEL_2 + UCSWRST;				// Use SMCLK, keep SW reset
	UCB0BR0 = prescale;							// fSCL = SMCLK/prescale
	UCB0BR1 = 0;
	UCB0I2CSA = slaveAddr;						// Load slave address
	UCB0CTL1 &= ~UCSWRST;						// Disable SW reset
	UCB0IE |= UCNACKIE;							// Enable NACK interrupt
	UCB0IE |= UCRXIE;							// Enable RX interrupt
}


void USCI_I2C_TX_Init(unsigned char slaveAddr, unsigned char prescale) {
	P1SEL |= BIT2 + BIT3;						// Select P1.2 & P1.3 to I2C function
	UCB0CTL1 |= UCSWRST;						// Enable SW reset
	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;		// I2C Master, synchronous mode
	UCB0CTL1 = UCSSEL_2 + UCSWRST;				// Use SMCLK, keep SW reset
	UCB0BR0 = prescale;							// fSCL = SMCLK/prescale
	UCB0BR1 = 0;
	UCB0I2CSA = slaveAddr;						// Load slave address
	UCB0CTL1 &= ~UCSWRST;						// Disable SW reset
	UCB0IE |= UCNACKIE;							// Enable NACK interrupt
	UCB0IE |= UCTXIE;							// Enable TX interrupt
}


void USCI_I2C_RX(unsigned char RXdataCtr, unsigned char *RXdataPtr) {
	USCI_I2C_ptr_RX = RXdataPtr;				// Load data pointer
	if (RXdataCtr == 1) {
		USCI_I2C_ctr_RX = 0;					// Load data counter
		__disable_interrupt();					// GI disable while send conditions
		UCB0CTL1 |= UCTXSTT;					// I2C start condition
		while (UCB0CTL1 & UCTXSTT);				// Start condition sent?
		UCB0CTL1 |= UCTXSTP;					// I2C stop condition
		__enable_interrupt();					// GI enable again
		__bis_SR_register(LPM0_bits + GIE);		// Enter LPM0, enable interrupts
	}
	else if (RXdataCtr > 1) {
		USCI_I2C_ctr_RX = RXdataCtr - 2;		// Load data counter
		UCB0CTL1 |= UCTXSTT;					// I2C start condition
		__bis_SR_register(LPM0_bits + GIE);		// Enter LPM0, enable interrupts
	}
	else {
		while(1);								// Trap if illegal parameter
	}
}


void USCI_I2C_TX(unsigned char TXdataCtr, unsigned char *TXdataPtr) {
	USCI_I2C_ptr_TX = TXdataPtr;				// Load data pointer
	USCI_I2C_ctr_TX = TXdataCtr;				// Load data counter
	//UCB0CTL1 |= UCTXSTT;						// I2C start condition
	UCB0CTL1 |= UCTR + UCTXSTT;					// I2C TX, start condition
	__bis_SR_register(LPM0_bits + GIE);			// Enter LPM0, enable interrupts
}


unsigned char USCI_I2C_NotReady(void) {
  return (UCB0STAT & UCBBUSY);
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	switch(__even_in_range(UCB0IV,12))	{

		case  0: break;														// Vector  0: No interrupts
		case  2: break;														// Vector  2: ALIFG

		case  4:															// Vector  4: NACKIFG
			// TODO delivering status check
			//USCI_I2C_ACKed = 0;
			UCB0CTL1 |= UCTXSTP;											// I2C stop condition
			UCB0IFG &= ~UCNACKIFG;											// Clear USCI_B0 NACK interrupt flag
			__bic_SR_register_on_exit(LPM0_bits);							// Exit active CPU
			break;

		case  6: break;														// Vector  6: STTIFG
		case  8: break;														// Vector  8: STPIFG

		case 10:															// Vector 10: RXIFG
			if (USCI_I2C_ctr_RX == 0) {
				UCB0CTL1 |= UCTXSTP;										// I2C stop condition
				*USCI_I2C_ptr_RX = UCB0RXBUF;								// Load data from buffer
				USCI_I2C_ptr_RX++;											// Increment data pointer
				__bic_SR_register_on_exit(LPM0_bits);						// Exit active CPU
			}
			else {
				*USCI_I2C_ptr_RX = UCB0RXBUF;								// Load data from buffer
				USCI_I2C_ptr_RX++;											// Increment RX data pointer
				USCI_I2C_ctr_RX--;											// Decrement RX data counter
			}
			break;

		case 12:															// Vector 12: TXIFG
			if (USCI_I2C_ctr_TX == 0) {
				UCB0CTL1 |= UCTXSTP;										// I2C stop condition
				UCB0IFG &= ~UCTXIFG;										// Clear USCI_B0 TX interrupt flag
				__bic_SR_register_on_exit(LPM0_bits);						// Exit active CPU
			}
			else {
				UCB0TXBUF = *USCI_I2C_ptr_TX;								// Load TX buffer
				USCI_I2C_ptr_TX++;											// Increment TX data pointer
				USCI_I2C_ctr_TX--;											// Decrement TX data counter
			}
			break;

		default: break;
	}
}
