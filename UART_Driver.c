#include "UART_Driver.h"

/*UARTA1 Ring Buffer Global Variables*/
volatile uint8_t UARTA1Data[UARTA1_BUFFERSIZE];
volatile uint32_t UARTA1ReadIndex;
volatile uint32_t UARTA1WriteIndex;

#define UARTA1_ADVANCE_READ_INDEX          MAP_Interrupt_disableMaster(); UARTA1ReadIndex = (UARTA1ReadIndex + 1) % UARTA1_BUFFERSIZE; MAP_Interrupt_enableMaster();
#define UARTA1_ADVANCE_WRITE_INDEX         UARTA1WriteIndex = (UARTA1WriteIndex + 1) % UARTA1_BUFFERSIZE
#define UARTA1_BUFFER_EMPTY                UARTA1ReadIndex == UARTA1WriteIndex ? true : false
#define UARTA1_BUFFER_FULL                 (UARTA1WriteIndex + 1) % UARTA1_BUFFERSIZE == UARTA1ReadIndex ? true : false

/*UARTA2 Ring Buffer Global Variables*/
volatile uint8_t UARTA2Data[UARTA2_BUFFERSIZE];
volatile uint32_t UARTA2ReadIndex;
volatile uint32_t UARTA2WriteIndex;

#define UARTA2_ADVANCE_READ_INDEX          MAP_Interrupt_disableMaster(); UARTA2ReadIndex = (UARTA2ReadIndex + 1) % UARTA2_BUFFERSIZE; MAP_Interrupt_enableMaster();
#define UARTA2_ADVANCE_WRITE_INDEX         UARTA2WriteIndex = (UARTA2WriteIndex + 1) % UARTA2_BUFFERSIZE
#define UARTA2_BUFFER_EMPTY                UARTA2ReadIndex == UARTA2WriteIndex ? true : false
#define UARTA2_BUFFER_FULL                 (UARTA2WriteIndex + 1) % UARTA2_BUFFERSIZE == UARTA2ReadIndex ? true : false
#define UARTA2_AVAILABLE                   UARTA2WriteIndex - UARTA2ReadIndex

void UART_Init(uint32_t UART, eUSCI_UART_Config UARTConfig)
{
    switch(UART)
    {
    case EUSCI_A1_BASE:
        MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
        MAP_UART_initModule(UART, &UARTConfig);
        MAP_UART_enableModule(UART);
        MAP_UART_enableInterrupt(UART, EUSCI_A_UART_RECEIVE_INTERRUPT);
        MAP_Interrupt_enableInterrupt(INT_EUSCIA1);
        break;
    case EUSCI_A2_BASE:
        MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
        MAP_UART_initModule(UART, &UARTConfig);
        MAP_UART_enableModule(UART);
        MAP_UART_enableInterrupt(UART, EUSCI_A_UART_RECEIVE_INTERRUPT);
        MAP_Interrupt_enableInterrupt(INT_EUSCIA2);
        break;
    /*Add more UART modules initialization modules here*/
    default:
        break;
    }
}

void UART_Write(uint32_t UART, uint8_t *Data, uint32_t Size)
{
    uint32_t i;
    for(i = 0; i < Size; i++)
    {
        MAP_UART_transmitData(UART, Data[i]);
    }
}

uint32_t UART_Read(uint32_t UART, uint8_t *Data, uint32_t Size)
{
    uint32_t i;
    int8_t c;

    switch(UART)
    {
        case EUSCI_A1_BASE:
            for(i = 0; i < Size; i++)
            {
                if(UARTA1_BUFFER_EMPTY)
                {
                    return i;
                }
                else
                {
                    c = UARTA1Data[UARTA1ReadIndex];
                    UARTA1_ADVANCE_READ_INDEX;

                    Data[i] = c;
                }
            }
            break;

        case EUSCI_A2_BASE:
            for(i = 0; i < Size; i++)
            {
                if(UARTA2_BUFFER_EMPTY)
                {
                    return i;
                }
                else
                {
                    c = UARTA2Data[UARTA2ReadIndex];
                    UARTA2_ADVANCE_READ_INDEX;

                    Data[i] = c;
                }
            }
            break;

        /*More UART reading modules go here*/
        default:
            return 0;
    }

    return i;
}

uint32_t UART_Available(uint32_t UART)
{
    switch(UART)
    {
    case EUSCI_A1_BASE:
        return UARTA1WriteIndex - UARTA1ReadIndex;
    case EUSCI_A2_BASE:
        return UARTA2WriteIndex - UARTA2ReadIndex;
    /*More UART available modules go here*/
    default:
        return 0;
    }
}

void UART_Flush(uint32_t UART)
{
    switch(UART)
    {
    case EUSCI_A1_BASE:
        UARTA1WriteIndex = UARTA1ReadIndex = 0;
        break;
    case EUSCI_A2_BASE:
        UARTA2WriteIndex = UARTA2ReadIndex = 0;
        break;
    }
}

void EUSCIA1_IRQHandler(void)
{
    uint8_t c;
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A1_BASE);

    MAP_UART_clearInterruptFlag(EUSCI_A1_BASE, status);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        c = MAP_UART_receiveData(EUSCI_A1_BASE);

        if(UARTA1_BUFFER_FULL)
        {
            /*TODO: Buffer Overflow, add handler here*/
        }
        else
        {
            UARTA1Data[UARTA1WriteIndex] = c;
            UARTA1_ADVANCE_WRITE_INDEX;

            /*Transmit data only if it made it to the buffer*/
            MAP_UART_transmitData(EUSCI_A1_BASE, c);
        }
    }
}

void EUSCIA2_IRQHandler(void)
{
    uint8_t c;
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);
    MAP_UART_clearInterruptFlag(EUSCI_A2_BASE, status);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT)
    {
        c = MAP_UART_receiveData(EUSCI_A2_BASE);

        if(UARTA2_BUFFER_FULL)
        {
            /*TODO: Buffer Overflow, add handler here*/
        }
        else
        {
            UARTA2Data[UARTA2WriteIndex] = c;
            UARTA2_ADVANCE_WRITE_INDEX;
        }
    }
}
