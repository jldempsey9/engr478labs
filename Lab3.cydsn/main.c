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
#include <stdbool.h>




/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  main() performs following functions:
*  1: Starts the ADC, UART, and 2 different DAC components.
*  2: Receives UART Input to sample ADC and return sample to UART
*     On 'C' or 'c' received: continuously transmits samples as they are completed.
*     On 'R' or 'r' received: single Read command, polls ADC and prints reading.
*     On 'X' or 'x' received: stops continuously transmitting samples.
*     On 'S' or 's' received: Switches between waveform DAC and constant voltage DAC
      On 'W' or 'w' received: toggle waveform generated by waveform DAC
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/


#define TRANSMIT_BUFFER_SIZE  16


uint8 recvChar = 0;     //stores received char from UART on interrupt
uint32 adcOut = 0;      //stores ADC result
bool continuousSending = false;     //polls ADC and sends Serial data continuously
uint8 dacState = 0;     //0 or 1 to choose between waveform DAC or constant voltage DAC
uint8 waveForm = 0;     //changes the waveform generated by waveform DAC

char TransmitBuffer[TRANSMIT_BUFFER_SIZE] = {0};  //transmit buffer for sending strings


//ISR triggered upon UART receiving data
CY_ISR(ReadSerial) {
    //interpret the data
    //S or s = switch between waveform and constant Voltage
    //R or r = read voltage
    //C or c = constant output data
    //X or x = stop constantly outputting data
    //W or w = switch waveform
    
    isr_1_ClearPending();   //apparently do this at beginning of ISR
    
    
    
    recvChar = UART_1_GetChar();
    switch(recvChar)
    {
        case 0:
            //no new data
            break;
        case 'S':
        case 's':
            //PWM_1_write(90);
            
            
            
            //Switches between waveform DAC and voltage DAC
            dacState ^= 1;      //XOR with 1 toggles dacState between 0 and 1
            AMux_1_Select(dacState);
            sprintf(TransmitBuffer, "This is dacState: %d\r\n", dacState);
            UART_1_PutString(TransmitBuffer);
            break;
        case 'R':
        case 'r':
            //Reads data and sends it to UART
            if(ADC_DelSig_1_IsEndConversion(ADC_DelSig_1_RETURN_STATUS)) {
                //adcOut = ADC_DelSig_1_CountsTo_mVolts(ADC_DelSig_1_GetResult16());
                adcOut = ADC_DelSig_1_CountsTo_mVolts(ADC_DelSig_1_GetResult8());
                sprintf(TransmitBuffer, "Single ADC Read: %lu\r\n", adcOut);
                UART_1_PutString(TransmitBuffer);
            }
            break;
        case 'C':
        case 'c':
            //Continuously sends data
            continuousSending = true;
            break;
        case 'X':
        case 'x':
            //Cancels continuously sending data
            continuousSending = false;
            break;
        case 'W':
        case 'w':
            //toggles waveform dac input wavesignal
            waveForm ^= 1;      //XOR with 1 toggles dacState between 0 and 1
            Waveform_Reg_Write(waveForm);
            sprintf(TransmitBuffer, "This is waveForm: %d\r\n", waveForm);
            UART_1_PutString(TransmitBuffer);
            break;
        default:
            // Place error handling code here
            //lol no
            
            
            //ok maybe we'll print something
            UART_1_PutString("Invalid command!\r\n");
            break;    
    }
        
}



int main()
{
    isr_1_StartEx(ReadSerial);      //attach the interrupt
    CYGlobalIntEnable;              //this one line wasted like an hour
    
    //start the stupid hardware thingies
    ADC_DelSig_1_Start();
    UART_1_Start();                             //does anyone even read these?
    UART_1_SetRxInterruptMode(UART_1_RX_STS_FIFO_NOTEMPTY);     //enable interrupts via software
    VDAC8_1_Start();
    WaveDAC8_1_Start();
    AMux_1_Start();
    AMux_1_Select(0);       //start at constant voltage
    
    //initialize variables
    
    
    //Start ADC Conversion(s)
    ADC_DelSig_1_StartConvert();
    Waveform_Reg_Write(waveForm);         //initialize wavesignal for waveform DAC

    
    // Send message to verify COM port is connected properly 
    UART_1_PutString("COM Port Open\r\n");
    
    adcOut = ADC_DelSig_1_CountsTo_mVolts(ADC_DelSig_1_GetResult8());
    sprintf(TransmitBuffer, "Initialized read: %lu mV\r\n", adcOut);
    UART_1_PutString(TransmitBuffer);
    
    //for loop polls ADC continuously if the proper mode is selected
    for(;;)
    {     
        //check for proper mode
        if(continuousSending) {
            if(ADC_DelSig_1_IsEndConversion(ADC_DelSig_1_RETURN_STATUS))    //poll ADC
            {

                //adcOut = ADC_DelSig_1_CountsTo_mVolts(ADC_DelSig_1_GetResult16());  //save output
                adcOut = ADC_DelSig_1_CountsTo_mVolts(ADC_DelSig_1_GetResult8());
                //print data to UART
                sprintf(TransmitBuffer, "Continuous ADC Read: %lu mV\r\n", adcOut);
                UART_1_PutString(TransmitBuffer);
                
            }
        }
    }
    return 0;
}


/* [] END OF FILE */
