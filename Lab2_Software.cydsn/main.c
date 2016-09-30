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
#include <errno.h>

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

//CONSTANTS
//settings
#define USING_INTERRUPTS 0      //1 for using interrupts, 0 for polling
#define HUMAN_EYE_FPS 60        //at 60hz, most people can't detect difference
#define DEBOUNCE_TIME 50      //minimum time to wait for button to finish bouncing
#define RAMP_RESOLUTION 50           //RAMP_RESOLUTION for duty cycle
#define POT_MAX 4080            //max reading from DAC potentiometer @ 4V
#define FREQUENCY_RATIO 4       //divide potentiometer result by this to get a max period.  frequency_ratio of 4 = 1 s max period

//global constants
#define on 1
#define off 0

//placeholder variables
bool debounce_on = false;       //used to delay polling to debounce button
//float currentBrightness = 0;
bool dimming = false;                   //used with the 'ramp' function
uint32 toggle_light_time_ms = 500;      //how fast to blink LED in 'blink' mode, can adjust with Pot.
                                        //default to blink the light every half second

//PWM variables
uint32 total_time_ms = 500;               //period of PWM, default 500ms
uint32 on_time_ms = 250;                  //how many ms the LED is on, must be < total_time_ms
uint32 off_time_ms = 250;
float duty_cycle = 0.5;                 //what % of PWM is high, default to 50% on


//possible modes for LED
enum pwmMode {blink, ramp};
enum pwmMode currentMode;
enum pwmMode previousMode;


//functions
void write_led(int);
//void Software_PWM_Start();  //attemps to start a new thread (not used)
void PWM_Blink(uint32);     //always a 50% duty cycle
void PWM_Ramp(uint32, uint32);  //different on/off times
uint16_t GetPotentiometerValue();
CY_ISR(ToggleModes) {
    if (currentMode == blink) {
        currentMode = ramp;
        write_led(on);      //for debugging
    } else {
        currentMode = blink;
        write_led(off);     //for debugging
    }
}







//TODO:
//find out how threading works

int main()
{

    #if USING_INTERRUPTS
        //toggles the led mode by using an interrupt
    isr_1_StartEx(ToggleModes);
    CYGlobalIntEnable;
    
    #endif
    
    //set default currentMode
    currentMode = blink;
    previousMode = ramp;
    
    //start ADC for potentiometer
    ADC_SAR_1_Start();
    
    //start running our PWM thread
    //Software_PWM_Start();           //runs in a separate thread than polling
    
    
    //loop polls button for press if interrupts disabled
    //polls potentiometer for values for period of PWM
    //only runs if the threading implementation is successful
    while (true)
    {
        //have to poll the button every loop if not using interrupts
        #if !(USING_INTERRUPTS)
            if(Status_Reg_1_Read() && !debounce_on) { //if the button is pressed
                debounce_on = true;
                
                //wait until button is debounced
                CyDelay(DEBOUNCE_TIME);
                
                if (Status_Reg_1_Read()) {
                    if(currentMode != previousMode) {   //if we are indeed toggling a different value
                        if (currentMode == blink) {
                            currentMode = ramp;
                            write_led(on);              //debugging to make sure polling was correct
                        } else if (currentMode == ramp){
                            currentMode = blink;
                            write_led(off);             //for debugging
                        }
                        previousMode = currentMode;
                    }
                    
                    //debounce_on = false;
                }
                
                debounce_on = false;
            } else {
                previousMode = !currentMode;   //if we're not pushing the button, we changed modes
            }
        #endif    
        
        
        
        
        
        
        //if we're blinking, for this loop do a blink PWM cycle
            if (currentMode == blink){
                //sets toggle_light_time_ms to value proportional to pot.
                toggle_light_time_ms = GetPotentiometerValue() / FREQUENCY_RATIO;      //min val=0, max val= 1second
                PWM_Blink(toggle_light_time_ms);
            }
            else if (currentMode == ramp) {
                //calculate current ramp duty_cycle
                //PWM_Blink((float)toggle_light_time_ms / (float) 5);
                
                if (duty_cycle >= 1)    //we have reached full brightness, time to dim
                {
                    dimming = true;
                } else if (duty_cycle <= 0) {
                    dimming = false;
                }
                
                //if we're supposed to dim, lessen the duty cycle
                if (dimming) {
                    duty_cycle -= 1.0 / (float) RAMP_RESOLUTION;   //duty cycle gets decremented by small fractions
                } else {
                    duty_cycle +=  1.0 / (float) RAMP_RESOLUTION;   //duty cycle is incremented by small fractions
                }
                
                total_time_ms = (1.0 / (float) HUMAN_EYE_FPS) * 1000;       //could be a constant if real-time
                on_time_ms = duty_cycle * total_time_ms;
                off_time_ms = total_time_ms - on_time_ms;
                
                PWM_Ramp(on_time_ms, off_time_ms);
            }
            
            
            
    }
    
    return 0;
}

//returns the value from potentiometer
uint16_t GetPotentiometerValue() {
    //poll potentiometer
        ADC_SAR_1_StartConvert(); 
        ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT);   //blocks until conversion complete
        
        uint16_t result = ADC_SAR_1_GetResult16();
        
        return result;
}

//shorthand for writing to control register
void write_led(int value) {
    Control_Reg_1_Write(value);   
}

//software PWM function runs in a separate thread.  
//runs calculations for PWM parameters, then calls PWM_Blink for timing
/*
void Software_PWM_Start() {
    pid_t pid = fork();
    int errsv = errno;      //save the error code, errno is volatile
    
    
    if (pid > 0) {  //parent process returns to main loop
        return;   
    } else {
        while(true) {  //child process continues forever blinking leds
            
            //if we're blinking, for this loop do a blink PWM cycle
            if (currentMode == blink){
                //sets toggle_light_time_ms to value proportional to pot.
                toggle_light_time_ms = GetPotentiometerValue() / 4;      //min val=0, max val= 1second
                PWM_Blink(toggle_light_time_ms);
            }
            else if (currentMode == ramp) {
                //calculate current ramp duty_cycle
                //PWM_Blink((float)toggle_light_time_ms / (float) 5);
                
                if (duty_cycle >= 1)    //we have reached full brightness, time to dim
                {
                    dimming = true;
                } else if (duty_cycle <= 0) {
                    dimming = false;
                }
                
                //if we're supposed to dim, lessen the duty cycle
                if (dimming) {
                    duty_cycle -= 1.0 / (float) RAMP_RESOLUTION;   //duty cycle gets decremented by small fractions
                } else {
                    duty_cycle +=  1.0 / (float) RAMP_RESOLUTION;   //duty cycle is incremented by small fractions
                }
                
                total_time_ms = (1.0 / (float) HUMAN_EYE_FPS) * 1000;       //could be a constant if real-time
                on_time_ms = duty_cycle * total_time_ms;
                off_time_ms = total_time_ms - on_time_ms;
                
                PWM_Ramp(on_time_ms, off_time_ms);
                
            }
        }
    }
}

*/

//shorthand to wait for timing
//evenly blinks LED with equal on/off times
void PWM_Blink(uint32 halfPeriod) {
    write_led(on);
    CyDelay(halfPeriod);
    write_led(off);
    CyDelay(halfPeriod); 
}

//used for different duty cycle
void PWM_Ramp(uint32 on_time, uint32 off_time) {
    write_led(on);
    CyDelay(on_time);
    write_led(off);
    CyDelay(off_time); 
}




/* [] END OF FILE */
