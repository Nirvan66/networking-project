/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
int InterruptCnt;
/*******************************************************************************
* Define Interrupt service routine and allocate an vector to the Interrupt
********************************************************************************/
CY_ISR(InterruptHandler)
{
	/* Read Status register in order to clear the sticky Terminal Count (TC) bit 
	 * in the status register. Note that the function is not called, but rather 
	 * the status is read directly.
	 */
   	Timer_1_STATUS;
    
	/* Increment the Counter to indicate the keep track of the number of 
     * interrupts received */
    InterruptCnt++;    
}
CY_ISR(rise)
{

}
 
CY_ISR(fall)
{

}
int main(void)
{
    CyGlobalIntEnable;
    TimerISR_StartEx(InterruptHandler);
    InterruptCnt=0;
    LCD_Start();
    
    //isr_1_Start();
    //isr_2_Start();
    isr_1_StartEx(rise);
    isr_2_StartEx(fall);

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
