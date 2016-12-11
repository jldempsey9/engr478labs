/*******************************************************************************
* File Name: Debug_LED_ctrl_PM.c
* Version 1.80
*
* Description:
*  This file contains the setup, control, and status commands to support 
*  the component operation in the low power mode. 
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "Debug_LED_ctrl.h"

/* Check for removal by optimization */
#if !defined(Debug_LED_ctrl_Sync_ctrl_reg__REMOVED)

static Debug_LED_ctrl_BACKUP_STRUCT  Debug_LED_ctrl_backup = {0u};

    
/*******************************************************************************
* Function Name: Debug_LED_ctrl_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Debug_LED_ctrl_SaveConfig(void) 
{
    Debug_LED_ctrl_backup.controlState = Debug_LED_ctrl_Control;
}


/*******************************************************************************
* Function Name: Debug_LED_ctrl_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*
*******************************************************************************/
void Debug_LED_ctrl_RestoreConfig(void) 
{
     Debug_LED_ctrl_Control = Debug_LED_ctrl_backup.controlState;
}


/*******************************************************************************
* Function Name: Debug_LED_ctrl_Sleep
********************************************************************************
*
* Summary:
*  Prepares the component for entering the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Debug_LED_ctrl_Sleep(void) 
{
    Debug_LED_ctrl_SaveConfig();
}


/*******************************************************************************
* Function Name: Debug_LED_ctrl_Wakeup
********************************************************************************
*
* Summary:
*  Restores the component after waking up from the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Debug_LED_ctrl_Wakeup(void)  
{
    Debug_LED_ctrl_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
