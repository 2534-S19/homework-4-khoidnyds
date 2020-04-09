#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "homework4.h"

typedef enum {SX, S2, S5, S3} microcontroller;

int main(void)
{
    char rChar;
    char *response = "\n\n\r2534 is the best course in the curriculum!\r\n\n";

    // TODO: Declare the variables that main uses to interact with your state machine.

    // Stops the Watchdog timer.
    initBoard();

    // TODO: Declare a UART config struct as defined in uart.h.
    //       To begin, configure the UART for 9600 baud, 8-bit payload (LSB first), no parity, 1 stop bit.
    // Division factor: 3,000,000 / 9,600 = 312.5
    const eUSCI_UART_ConfigV1 myUART = {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                // SMCLK Clock Source
        19,                                            // BRDIV = 312 / 16 = 19
        8,                                             // UCxBRF = 312 % 16 = 8
        85,                                            // UCxBRS = 85
        EUSCI_A_UART_NO_PARITY,                        // No Parity
        EUSCI_A_UART_LSB_FIRST,                        // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                     // One stop bit
        EUSCI_A_UART_MODE,                             // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION, // Oversampling
        EUSCI_A_UART_8_BIT_LEN                         // Character length
    };

    initUART(EUSCI_A0_BASE, &myUART);

    while(1)
    {
        // TODO: Check the receive interrupt flag to see if a received character is available.
        //       Return 0xFF if no character is available.
        rChar = UARTGetChar(EUSCI_A0_BASE);

        // TODO: If an actual character was received, echo the character to the terminal AND use it to update the FSM.
        //       Check the transmit interrupt flag prior to transmitting the character.
        if(rChar != '\xFF'){
            UARTPutChar(EUSCI_A0_BASE,rChar);

        // TODO: If the FSM indicates a successful string entry, transmit the response string.
        //       Check the transmit interrupt flag prior to transmitting each character and moving on to the next one.
        //       Make sure to reset the success variable after transmission.
            if(charFSM(rChar)){
                UARTPutString(EUSCI_A0_BASE,response);
            }
        }
    }
}
uint8_t UARTGetChar(uint32_t moduleInstance){
    if(UARTHasChar(moduleInstance)){
        return UART_receiveData(moduleInstance);
    }else{
        return '\xFF';
    }
}
bool UARTHasChar(uint32_t moduleInstance){
    return (UART_getInterruptStatus(moduleInstance,EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) == EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);
}
void UARTPutChar(uint32_t moduleInstance, uint8_t transmittedChar){
    if(UARTCanSend(moduleInstance)){
        UART_transmitData(moduleInstance, transmittedChar);
    }
}
bool UARTCanSend(uint32_t moduleInstance){
    return (UART_getInterruptStatus (moduleInstance, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) == EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG);
}
void UARTPutString(uint32_t moduleInstance, uint8_t *str){
    uint8_t i = 0;
    for (i ; i < strlen(str); i++) {
        UART_transmitData(moduleInstance, str[i]);
    }
}
void initUART(uint32_t moduleInstance, const eUSCI_UART_ConfigV1 *config)
{
    // TODO: Initialize EUSCI_A0
    UART_initModule(moduleInstance, config);

    // TODO: Enable EUSCI_A0
    UART_enableModule(moduleInstance);

    // TODO: Make sure Tx AND Rx pins of EUSCI_A0 work for UART and not as regular GPIO pins.
    // UCA0RXD P1.2
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                                               GPIO_PIN2,
                                               GPIO_PRIMARY_MODULE_FUNCTION);
    // UCA0TXD P1.3
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1,
                                                GPIO_PIN3,
                                                GPIO_PRIMARY_MODULE_FUNCTION);
}

void initBoard()
{
    WDT_A_hold(WDT_A_BASE);
}

// TODO: FSM for detecting character sequence.
bool charFSM(char rChar)
{
    static microcontroller current = SX;
    bool finished = false;
    switch(current){
        case SX:
            if(rChar == '2'){
                current = S2;
            }
            break;
        case S2:
            if(rChar == '5'){
                current = S5;
            }
            else if(rChar == '2'){
                current = S2;
            }
            else{
                current = SX;
            }
            break;
        case S5:
            if(rChar == '3'){
                current = S3;
            }
            else if(rChar == '2'){
                current = S2;
            }
            else{
                current = SX;
            }
            break;
        case S3:
            if(rChar == '4'){
                finished = true;
                current = SX;
            }
            else if(rChar == '2'){
                current = S2;
            }
            else{
                current = SX;
            }
            break;
    }
    return finished;
}
