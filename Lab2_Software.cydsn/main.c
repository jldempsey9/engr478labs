/******************************************************************************
* Project Name		: CE95352 Blinking LED
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
*******************************************************************************/

#include <device.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  Software-controller PWM controls and LED to blink in 1s interval
                           as well as control the brightness
*
* Parameters:
*  None.
*
* Return:
*  None.
*

Partner Names: John Dempsey, Haitham Alzerma, Kwesi
Partner Lab Group name: 3GL (3 Guys in a Lab)
*******************************************************************************/


#define on 1
#define off 0
#define resolution 50

enum pwmMode {blink, ramp};

enum pwmMode currentMode;

void write_led(int);

CY_ISR(ToggleModes) {
    if (currentMode == blink) {
        currentMode = ramp;
        write_led(on);
    } else {
        currentMode = blink;
        write_led(off);
    }
}

#define userFrequency 60        //frequency in hertz
#define unseenFrequency 60      //at 60hz, most people can't detect difference
#define secondPeriod 500        //toggle light every 500ms

#define USING_INTERRUPTS 0      //1 for using interrupts, 0 for polling
#define DEBOUNCE_TIME 200

float currentBrightness = 0;
uint32 on_time_ms = 0;
uint32 off_time_ms = 1;

uint32 total_time_ms = 5;
float duty_cycle = 0.1;

void Software_PWM_Start();
void PWM_Blink(uint32);
void PWM_Ramp(float, uint32);

bool debounce_on = false;
//TODO:
//refactor
//debounce
//add potentiometer

int main()
{

    #if USING_INTERRUPTS
    
        //toggles the led mode by using an interrupt
    isr_1_StartEx(ToggleModes);
    CYGlobalIntEnable;
    
    #endif
    
    Software_PWM_Start();
    
    //on necessary if polling for button input
    for (;;)
    {
        
        #if !(USING_INTERRUPTS)
            if(Status_Reg_1_Read() && !debounce_on) { //if the button is pressed
                debounce_on = true;
                
                CyDelay(DEBOUNCE_TIME);
                
                if (Status_Reg_1_Read()) {
                    if (currentMode == blink) {
                        currentMode = ramp;
                        write_led(on);    //debugging to make sure polling was correct
                    } else if (currentMode == ramp){
                        currentMode = blink;
                        write_led(off);
                    }
                    
                    debounce_on = false;
                }
                
            }
            
        #endif
        
    }
    
    return 0;
}

void write_led(int value) {
    Control_Reg_1_Write(value);   
}

void Software_PWM_Start() {
    pid_t pid = fork();
    if (pid > 0) {  //parent process returns
        return;   
    } else {
        while(1) {  //child process continues forever blinking leds
            
            if (currentMode == blink){
                PWM_Blink(secondPeriod);
            }
            else if (currentMode == ramp) {
                //calculate current ramp duty_cycle
                PWM_Blink((float)secondPeriod / (float) 5);
                
                /*
                if (duty_cycle >= 1)
                {
                 duty_cycle = 0;   
                }
                duty_cycle += 1 / (float) resolution;
                
                
                total_time_ms = ((float)1 / (float)userFrequency) * 1000;
                on_time_ms = duty_cycle * total_time_ms;
                off_time_ms = total_time_ms - on_time_ms;
                
                PWM_Ramp(0.5, 5000);
                */
            }
        }
    }
}

void PWM_Blink(uint32 halfPeriod) {
    write_led(on);
    CyDelay(halfPeriod);
    write_led(off);
    CyDelay(halfPeriod); 
}

void PWM_Ramp(float dutyCycle, uint32 frequency){
    
}




/* [] END OF FILE */
