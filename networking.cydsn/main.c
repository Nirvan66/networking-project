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
#include <stdlib.h>
#include <sys/timeb.h>
#include "project.h"
#include "functions.h"
#define USBFS_DEVICE    (0u)
int InterruptCnt;
/*******************************************************************************
* Define Interrupt service routine and allocate an vector to the Interrupt
********************************************************************************/
typedef enum state {IDLE, COLLISION, BUSY_HIGH, BUSY_LOW} State;
State cur_state = IDLE;
int check_for_low = 0;
unsigned char rx_bits[16];
int rx_bit_idx = 0;
int rx_buffer_idx = 0;;
uint8 rx_buffer[50];
int receiving = 0;

CY_ISR(InterruptHandler)
{
	/* Read Status register in order to clear the sticky Terminal Count (TC) bit 
	 * in the status register. Note that the function is not called, but rather 
	 * the status is read directly.
	 */
   	Timer_1_STATUS;
    
    if (check_for_low == 0){
        cur_state = IDLE;
        //Timer_1_Stop();
    } else {
        cur_state = COLLISION;
        //Timer_1_Stop();
    }
}

CY_ISR(rise)
{
    if (USBUART_CDCIsReady())
    {
        
    }
    
    if (check_for_low == 0){
        //Timer_1_WritePeriod(60);
        //Timer_1_WriteCounter(59);
        Timer_1_WritePeriod(52);
        Timer_1_WriteCounter(51);
        Timer_1_Start();
        check_for_low = 1;
        cur_state = BUSY_HIGH;
    }
    
    if (receiving == 0)
    {
        Timer_2_Start();
        receiving = 1;
    }
}
 
CY_ISR(fall)
{
    if (check_for_low == 1){
        //Timer_1_WritePeriod(770);
        //Timer_1_WriteCounter(769);
        Timer_1_WritePeriod(830);
        Timer_1_WriteCounter(829);
        Timer_1_Start();
        check_for_low = 0;
        cur_state = BUSY_LOW;
    }
}

CY_ISR(Rx)
{
    char next = 0;
    int shift_count = 7;
    
    Timer_2_STATUS;
    Timer_2_Start();
    
    rx_bits[rx_bit_idx] = Rx_Read();
    ++rx_bit_idx;
    
    if (rx_bit_idx >= 15)
    {
        rx_bit_idx = 0;
        Timer_2_Stop();
        receiving = 0;
        
        for (int i = 15; i >= 0; i--)
        {
            if (i == 15)
            {
                next = 0x80;
            }
            
            if (i % 2 != 0)
            {
                next = (next | (rx_bits[i] << shift_count));
                --shift_count;
            }
        }
        
        rx_buffer[rx_buffer_idx] = next;
        ++rx_buffer_idx;
    }
}

int main(void)
{
    CyGlobalIntEnable;
    TimerISR_StartEx(InterruptHandler);
    InterruptCnt=0;
    LCD_Start();
    USBUART_Start(USBFS_DEVICE, USBUART_5V_OPERATION);
    
    
    
    isr_1_StartEx(rise);
    isr_2_StartEx(fall);
    RxISR_StartEx(Rx);

    uint16 delay = 500;
    char input[44]; 
    uint8 idx = 0;
    char transmit;
    NETWORK_OUT_Write(0);
    uint8 no_collision = 1;
    int8 transmission_complete = 0;
    int size;
    int from_collision = 0;
    
    for(;;)
    {
        LCD_Position(1,0);
        LCD_PrintString(" READING STRING");
        
        getString(input);

        transmission_complete=0;
        idx=0;
        
        while(cur_state != IDLE);
        while(!transmission_complete)
        {
            switch(cur_state)
            {
                case IDLE :
                    putString(rx_buffer, rx_buffer_idx);
                    rx_buffer_idx = 0;
                    if (from_collision == 1)
                    {
                        srand(Timer_1_ReadCounter());
                        uint32 delay = rand() % 129;
                        CyDelay((uint32) (((delay)/128.0) * 1000.0));
                        from_collision = 0;
                    }
                    LCD_ClearDisplay();
                    LCD_Position(0,0);
                    LCD_PrintString("IDLE: ");
                    LCD_PrintString(input);
                    LCD_PrintString("At: ");
                    LCD_PrintNumber(idx);
                    
                    if((transmit = * (input + idx)) != '\0')
                    {
                        NETWORK_OUT_Write(1);
                        CyDelayUs(delay);
                        NETWORK_OUT_Write(0);
                        CyDelayUs(delay);
                        
                        char binary[8];
                    
                        // Convert to binary
                        itoa(transmit, binary, 2);
                        for (int i = 0; i < 7; ++i)
                        {
                            if (binary[i] == '0')
                            {
                                NETWORK_OUT_Write(0);
                                CyDelayUs(delay * 2);
                            }
                            else
                            {
                                NETWORK_OUT_Write(1);
                                CyDelayUs(delay);
                                NETWORK_OUT_Write(0);
                                CyDelayUs(delay);
                            }
                        }
                        ++idx;
                    }
                    else
                    {
                        transmission_complete=1;
                        LCD_ClearDisplay();
                        LCD_Position(0,0);
                        LCD_PrintString("Sent:");
                        LCD_PrintString(input);
                    }
                    
                    
                break;

                case COLLISION:
                    
                    //LCD_ClearDisplay();
                    LCD_Position(1,0);
                    LCD_PrintString("                 ");
                    LCD_Position(1,0);
                    LCD_PrintString(" COLLISION ");
                    
                    idx=0;
                    NETWORK_OUT_Write(0);
                    
                    from_collision = 1;
                
                    uint32 delay = rand() % 129;
                    CyDelay((uint32) (((delay)/128) * 1000));
                break;
                    
                case BUSY_HIGH:
                    //LCD_ClearDisplay();
                    LCD_Position(1,0);
                    LCD_PrintString("                 ");
                    LCD_Position(1,0);
                    LCD_PrintString(" BUSY_LOW ");
                break;
                    
                case BUSY_LOW:
                    //LCD_ClearDisplay();
                    LCD_Position(1,0);
                    LCD_PrintString("                 ");
                    LCD_Position(1,0);
                    LCD_PrintString(" BUSY_HIGH ");
                break;
            }
        }  
    }
}   
    
    /*
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
    */
    

/* [] END OF FILE */
