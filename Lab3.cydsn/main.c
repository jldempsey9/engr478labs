/******************************************************************************
* Project Name		: Lab 3
* File Name			: main.c
* Version 			: **
* Device Used		: CY8C5888LTI-LP097
* Software Used		: PSoC Creator 3.1 SP2
* Compiler    		: ARM GCC 4.8.4, ARM RVDS Generic, ARM MDK Generic
* Related Hardware	: CY8CKIT059 PSoC 5 LP Prototyping Kit 
* Owner				: KLMZ
*
********************************************************************************
* Copyright (2015), Cypress Semiconductor Corporation. All Rights Reserved.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress)
* and is protected by and subject to worldwide patent protection (United
* States and foreign), United States copyright laws and international treaty
* provisions. Cypress hereby grants to licensee a personal, non-exclusive,
* non-transferable license to copy, use, modify, create derivative works of,
* and compile the Cypress Source Code and derivative works for the sole
* purpose of creating custom software in support of licensee product to be
* used only in conjunction with a Cypress integrated circuit as specified in
* the applicable agreement. Any reproduction, modification, translation,
* compilation, or representation of this software except as specified above 
* is prohibited without the express written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH 
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the 
* materials described herein. Cypress does not assume any liability arising out 
* of the application or use of any product or circuit described herein. Cypress 
* does not authorize its products for use as critical components in life-support 
* systems where a malfunction or failure may reasonably be expected to result in 
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of 
* such use and in doing so indemnifies Cypress against all charges. 
*
* Use of this Software may be limited by and subject to the applicable Cypress
* software license agreement. 


3GL, John Dempsey, Kwesi, Haitham
*******************************************************************************/

#include <device.h>
#include "stdio.h"

/* Project Defines */
#define FALSE  0
#define TRUE   1
#define TRANSMIT_BUFFER_SIZE  16


/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  main() performs following functions:
*  1: Starts the ADC and UART components.
*  2: Checks for ADC end of conversion.  Stores latest result in output
*     if conversion complete.
*  3: Checks for UART input.
*     On 'C' or 'c' received: transmits the last sample via the UART.
*     On 'S' or 's' received: continuously transmits samples as they are completed.
*     On 'X' or 'x' received: stops continuously transmitting samples.
*     On 'E' or 'e' received: transmits a dummy byte of data.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/



uint8 recvChar;     //stores received char from UART on interrupt
uint8 ledState;     //turns on and off the LED
uint32 adcOut;      //stores ADC result

/* Variable used to send emulated data */
uint8 EmulatedData;
/* Flags used to store transmit data commands */
uint8 ContinuouslySendData;
uint8 SendSingleByte;
uint8 SendEmulatedData;
/* Transmit Buffer */
char TransmitBuffer[TRANSMIT_BUFFER_SIZE];


CY_ISR(ReadSerial) {
    //interpret the data
    //S or s = switch between waveform and constant Voltage
    //R or r = read voltage
    //C or c = constant output data
    //X or x = stop constantly outputting data
    
    
    Debug_LED_ctrl_Write(ledState);
    if (ledState) {
        ledState = 0;   
    } else {
        ledState = 1;   
    }
    
    
    
    recvChar = UART_1_GetChar();
    switch(recvChar)
    {
        case 0:
            //no new data
            break;
        case 'S':
        case 's':
            //Switches between waveform DAC and voltage DAC
            AMux_1_Select(ledState);
            break;
        case 'R':
        case 'r':
            //Reads data and sends it to UART
            Control_Reg_1_Write(1);
            
            adcOut = ADC_DelSig_1_CountsTo_mVolts(ADC_DelSig_1_GetResult16());
            sprintf(TransmitBuffer, "adc reads: %lu\r\n", adcOut);
            UART_1_PutString(TransmitBuffer);
            break;
        case 'C':
        case 'c':
            //Continuously sends data
            Control_Reg_1_Write(1);
            break;
        case 'X':
        case 'x':
            //Cancels continuously sending data
            Control_Reg_1_Write(1);
            break;
        default:
            // Place error handling code here
            //lol no
            break;    
    }
        
}



int main()
{
    
   
    isr_1_StartEx(ReadSerial);      //attach the interrupt
    CYGlobalIntEnable;              //this one line wasted like an hour
    
    /* Start the components */
    ADC_DelSig_1_Start();
    UART_1_Start();
    UART_1_SetRxInterruptMode(UART_1_RX_STS_FIFO_NOTEMPTY);     //enable interrupts via software
    VDAC8_1_Start();
    WaveDAC8_1_Start();
    AMux_1_Start();
    AMux_1_Select(1);
    //amux_1_
    
    //initialize variables
    ledState = 1;
    
    Debug_LED_ctrl_Write(ledState);
    
    

    
    
    /* Start the ADC conversion */
    ADC_DelSig_1_StartConvert();
    
    /* Send message to verify COM port is connected properly */
    UART_1_PutString("COM Port Open");
    
    for(;;)
    {     
        
        /*
        //UART_1_PutString("COM Port Open");
        if(ADC_DelSig_1_IsEndConversion(ADC_DelSig_1_RETURN_STATUS))
        {

            Output = ADC_DelSig_1_CountsTo_mVolts(ADC_DelSig_1_GetResult16());
            
            if(SendSingleByte || ContinuouslySendData)
            {
                sprintf(TransmitBuffer, "Sample: %lu mV\r\n", Output);
                UART_1_PutString(TransmitBuffer);
                SendSingleByte = FALSE;
            }
            else if(SendEmulatedData)
            {
                sprintf(TransmitBuffer, "Emulated Data: %x \r\n", EmulatedData);

                UART_1_PutString(TransmitBuffer);
                EmulatedData++;
                SendEmulatedData = FALSE;   
            }
        }
        */
    }
    return 0;
}


/* [] END OF FILE */
