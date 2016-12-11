/*******************************************************************************
* File Name: Speaker_PWM_PM.c
* Version 3.30
*
* Description:
*  This file provides the power management source code to API for the
*  PWM.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Speaker_PWM.h"

static Speaker_PWM_backupStruct Speaker_PWM_backup;


/*******************************************************************************
* Function Name: Speaker_PWM_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  Speaker_PWM_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Speaker_PWM_SaveConfig(void) 
{

    #if(!Speaker_PWM_UsingFixedFunction)
        #if(!Speaker_PWM_PWMModeIsCenterAligned)
            Speaker_PWM_backup.PWMPeriod = Speaker_PWM_ReadPeriod();
        #endif /* (!Speaker_PWM_PWMModeIsCenterAligned) */
        Speaker_PWM_backup.PWMUdb = Speaker_PWM_ReadCounter();
        #if (Speaker_PWM_UseStatus)
            Speaker_PWM_backup.InterruptMaskValue = Speaker_PWM_STATUS_MASK;
        #endif /* (Speaker_PWM_UseStatus) */

        #if(Speaker_PWM_DeadBandMode == Speaker_PWM__B_PWM__DBM_256_CLOCKS || \
            Speaker_PWM_DeadBandMode == Speaker_PWM__B_PWM__DBM_2_4_CLOCKS)
            Speaker_PWM_backup.PWMdeadBandValue = Speaker_PWM_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(Speaker_PWM_KillModeMinTime)
             Speaker_PWM_backup.PWMKillCounterPeriod = Speaker_PWM_ReadKillTime();
        #endif /* (Speaker_PWM_KillModeMinTime) */

        #if(Speaker_PWM_UseControl)
            Speaker_PWM_backup.PWMControlRegister = Speaker_PWM_ReadControlRegister();
        #endif /* (Speaker_PWM_UseControl) */
    #endif  /* (!Speaker_PWM_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Speaker_PWM_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  Speaker_PWM_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Speaker_PWM_RestoreConfig(void) 
{
        #if(!Speaker_PWM_UsingFixedFunction)
            #if(!Speaker_PWM_PWMModeIsCenterAligned)
                Speaker_PWM_WritePeriod(Speaker_PWM_backup.PWMPeriod);
            #endif /* (!Speaker_PWM_PWMModeIsCenterAligned) */

            Speaker_PWM_WriteCounter(Speaker_PWM_backup.PWMUdb);

            #if (Speaker_PWM_UseStatus)
                Speaker_PWM_STATUS_MASK = Speaker_PWM_backup.InterruptMaskValue;
            #endif /* (Speaker_PWM_UseStatus) */

            #if(Speaker_PWM_DeadBandMode == Speaker_PWM__B_PWM__DBM_256_CLOCKS || \
                Speaker_PWM_DeadBandMode == Speaker_PWM__B_PWM__DBM_2_4_CLOCKS)
                Speaker_PWM_WriteDeadTime(Speaker_PWM_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(Speaker_PWM_KillModeMinTime)
                Speaker_PWM_WriteKillTime(Speaker_PWM_backup.PWMKillCounterPeriod);
            #endif /* (Speaker_PWM_KillModeMinTime) */

            #if(Speaker_PWM_UseControl)
                Speaker_PWM_WriteControlRegister(Speaker_PWM_backup.PWMControlRegister);
            #endif /* (Speaker_PWM_UseControl) */
        #endif  /* (!Speaker_PWM_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: Speaker_PWM_Sleep
********************************************************************************
*
* Summary:
*  Disables block's operation and saves the user configuration. Should be called
*  just prior to entering sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  Speaker_PWM_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Speaker_PWM_Sleep(void) 
{
    #if(Speaker_PWM_UseControl)
        if(Speaker_PWM_CTRL_ENABLE == (Speaker_PWM_CONTROL & Speaker_PWM_CTRL_ENABLE))
        {
            /*Component is enabled */
            Speaker_PWM_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            Speaker_PWM_backup.PWMEnableState = 0u;
        }
    #endif /* (Speaker_PWM_UseControl) */

    /* Stop component */
    Speaker_PWM_Stop();

    /* Save registers configuration */
    Speaker_PWM_SaveConfig();
}


/*******************************************************************************
* Function Name: Speaker_PWM_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration. Should be called just after
*  awaking from sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  Speaker_PWM_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Speaker_PWM_Wakeup(void) 
{
     /* Restore registers values */
    Speaker_PWM_RestoreConfig();

    if(Speaker_PWM_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        Speaker_PWM_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
