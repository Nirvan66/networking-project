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
#include "functions.h"
#include <project.h>


int getString(char *c)
{
    
    int size=0;
    int count=0;
    int brk=1;
    uint8_t buffer[10];
    do
    {
        if (0u != USBUART_IsConfigurationChanged())
        {
            /* Initialize IN endpoints when device is configured. */
            if (0u != USBUART_GetConfiguration())
            {
                /* Enumeration is done, enable OUT endpoint to receive data 
                 * from host. */
                USBUART_CDC_Init();
            }
        }
        /* Service USB CDC when device is configured. */
        if (0u != USBUART_GetConfiguration())
        {
            /* Check for input data from host. */
            if (0u != USBUART_DataIsReady())
            {
                /* Read received data and re-enable OUT endpoint. */
                count = USBUART_GetAll(buffer);

                if (0u != count)
                {
                    /* Wait until component is ready to send data to host. */
                    //while (0u == USBUART_CDCIsReady());
                    //LCD_ClearDisplay();
                    //LCD_Position(0,0);
                    for(int i=0;i<count;i++)
                    {
                        //LCD_PrintNumber(buffer[i]);
                        //LCD_PrintString(",");
                        if(buffer[i]==13)
                        {
                            brk=!brk;
                            USBUART_PutCRLF();
                        }
                        else
                        {
                            c[size]=buffer[i];
                            size++;
                        }
                    }
                    //LCD_PrintNumber(count);
                    //USBUART_PutData(buffer, count);
                }
            }
        }
    }while(brk);
    c[size]='\0';
    size++;
    return size;
}
/* [] END OF FILE */
