/*******************************************************************************
* File Name: dma_interrrupt.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_dma_interrrupt_H)
#define CY_ISR_dma_interrrupt_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void dma_interrrupt_Start(void);
void dma_interrrupt_StartEx(cyisraddress address);
void dma_interrrupt_Stop(void);

CY_ISR_PROTO(dma_interrrupt_Interrupt);

void dma_interrrupt_SetVector(cyisraddress address);
cyisraddress dma_interrrupt_GetVector(void);

void dma_interrrupt_SetPriority(uint8 priority);
uint8 dma_interrrupt_GetPriority(void);

void dma_interrrupt_Enable(void);
uint8 dma_interrrupt_GetState(void);
void dma_interrrupt_Disable(void);

void dma_interrrupt_SetPending(void);
void dma_interrrupt_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the dma_interrrupt ISR. */
#define dma_interrrupt_INTC_VECTOR            ((reg32 *) dma_interrrupt__INTC_VECT)

/* Address of the dma_interrrupt ISR priority. */
#define dma_interrrupt_INTC_PRIOR             ((reg8 *) dma_interrrupt__INTC_PRIOR_REG)

/* Priority of the dma_interrrupt interrupt. */
#define dma_interrrupt_INTC_PRIOR_NUMBER      dma_interrrupt__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable dma_interrrupt interrupt. */
#define dma_interrrupt_INTC_SET_EN            ((reg32 *) dma_interrrupt__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the dma_interrrupt interrupt. */
#define dma_interrrupt_INTC_CLR_EN            ((reg32 *) dma_interrrupt__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the dma_interrrupt interrupt state to pending. */
#define dma_interrrupt_INTC_SET_PD            ((reg32 *) dma_interrrupt__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the dma_interrrupt interrupt. */
#define dma_interrrupt_INTC_CLR_PD            ((reg32 *) dma_interrrupt__INTC_CLR_PD_REG)


#endif /* CY_ISR_dma_interrrupt_H */


/* [] END OF FILE */
