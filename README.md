# CC430F5137_USCI_B0_I2C #

## Discription ##

I2C master library for USCI_B0 CC430F5137

## Example of usage ##

First you should initialize hardware I2C module:

    USCI_I2C_Init();

If you want to send data to the I2C device:

    unsigned char devAddr = 0x04;                       // I2C device address
    unsigned char dataBuffer[3] = { 0x00, 0x01, 0x02 }; // Data to send
    USCI_I2C_TX_Init(devAddr, 4);                       // Set I2C module to TX mode (4 is prescaler, see funcdoc)
    while(USCI_I2C_NotReady());                         // Waiting while I2C bus sets free
    USCI_I2C_TX(3, dataBuffer);                         // Writing data to device via I2C bus

If you want to get data from the REG register of I2C device:

    unsigned char devAddr = 0x04;                       // I2C device address
    unsigned char regAddr[] = { 0x05 };                 // Register address (4 is prescaler, see funcdoc)
    unsigned char rxBuff[] = { 0x00 };                  // Buffer for the RX data
    USCI_I2C_TX_Init(devAddr, 4);                       // Set I2C module to TX mode
    while(USCI_I2C_NotReady());                         // Waiting while I2C bus sets free
    USCI_I2C_TX(1, regAddr);                            // Send register address to I2C device
    USCI_I2C_RX_Init(devAddr, 4);                       // Send read state to I2C device (4 is prescaler, see funcdoc)
    while(USCI_I2C_NotReady());                         // Waiting while I2C bus sets free
    USCI_I2C_RX(1, rxBuff);                             // Receivind data from I2C device to buffer
