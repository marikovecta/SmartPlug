//*************************

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "Smart_plug.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/uart.h"
#include "utils/uartsdio.h"

//*************************

unsigned char o;
void PortFunctionInit(void)
{
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  //
  // Enable Peripheral Clocks
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  //
  // Enable pin PB1 for UART1 U1TX
  //
  GPIOPinConfigure(GPIO_PB0_U1RX);
  GPIOPinConfigure(GPIO_PB1_U1TX); //Enable pin PF1 for UART1 U1CTS

  GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1); //Make UART Pins peripheral controlled

  //Configure PF0 as output and initialize it to logic low
  GPIOPinTypeGPIOOutput (GPIO_PORTF_BASE, GPIO_PIN_1);
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);

  //Enable pull up on PB0 and PB1
  GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

  //Sets the configuration of UART
  UARTConfigSetExpClk(UART1_BASE, 16000000, 9600,
  (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
}

void UARTIntHandler(void) //UART INTERRUPT HANDLER
{
  while(!UARTCharsAvail(UART1_BASE)); //bool determines if any char in receive FIFO
  o = UARTCharGet(UART1_BASE); //int waits for character from UART1
  UARTCharPut(UART1_BASE, o);
  if (o == '0') //if 0 pressed
    {
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00); //turn off LED
    }
  else if ( o == '1') // if 1 pressed
    {
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02); //turn on LED
    }
  else //if anything else pressed
    {
      //do nothing
    }

int main(void)
{
  PortFunctionInit();
  IntMasterEnable(); //enable processor interrupts
  IntEnable(INT_UART1); //enable the UART interrupt
  while(1)
    {
      UARTIntHandler();
    }
}
