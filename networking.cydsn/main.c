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

typedef enum state {IDLE, COLLISION, BUSY_HIGH, BUSY_LOW} State;
State cur_state = IDLE;
int check_for_low = 0;

CY_ISR(InterruptHandler)
{
	/* Read Status register in order to clear the sticky Terminal Count (TC) bit 
	 * in the status register. Note that the function is not called, but rather 
	 * the status is read directly.
	 */
   	Timer_1_STATUS;
    
    if (check_for_low == 0){
        cur_state = IDLE;
    } else {
        cur_state = COLLISION;
    }
}

CY_ISR(rise)
{
    if (check_for_low == 0){
        Timer_1_WritePeriod(60);
        Timer_1_WriteCounter(59);
        Timer_1_Start();
        check_for_low = 1;
        cur_state = BUSY_HIGH;
    }
}
 
CY_ISR(fall)
{
    if (check_for_low == 1){
        Timer_1_WritePeriod(770);
        Timer_1_WriteCounter(769);
        Timer_1_Start();
        check_for_low = 0;
        cur_state = BUSY_LOW;
    }
}

int main(void)
{
    CyGlobalIntEnable;
    TimerISR_StartEx(InterruptHandler);
    InterruptCnt=0;
    LCD_Start();
    
    isr_1_StartEx(rise);
    isr_2_StartEx(fall);

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        switch(cur_state){
            case IDLE :;
                LCD_Position(0,0);
                LCD_PrintString("      IDLE      ");
                
                IDLE_Write(1);
                BUSY_Write(!IDLE_Read());
                COLLISION_Write(!IDLE_Read());
            break;
            
            case BUSY_HIGH:;
                LCD_Position(0,0);
                LCD_PrintString("      BUSY      ");
                
                BUSY_Write(1);
                IDLE_Write(!BUSY_Read());
                COLLISION_Write(!BUSY_Read());
            break;
            
            case BUSY_LOW:;
                LCD_Position(0,0);
                LCD_PrintString("      BUSY      ");
                
                BUSY_Write(1);
                IDLE_Write(!BUSY_Read());
                COLLISION_Write(!BUSY_Read());
            break;
            
            case COLLISION:;
                LCD_Position(0,0);
                LCD_PrintString("   COLLISION    ");
                
                COLLISION_Write(1);
                IDLE_Write(!COLLISION_Read());
                BUSY_Write(!COLLISION_Read());
            break;
        }
    }
}

/* [] END OF FILE */
