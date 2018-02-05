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
    
    //used to check for new line at the end of data entry
    int brk=1;
    //used to check for ',' to seprate address and data
    int addr_data=0;

    //used to store data
    int data_size=0;
    char data[44];
    
    //used to store address
    int addr_size=0;
    char addr[5];
    
    uint8_t buffer[10];
    int count=0;
    
    c[size++]=0;
    c[size++]=1;
    c[size++]=(char)103;
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
                            break;
                        }
                        if(buffer[i]==',')
                        {
                            addr_data=!addr_data;
                        }
                        else
                        {
                            if(addr_data)
                            {
                                data[data_size++]=buffer[i];
                            }
                            else
                            {
                                addr[addr_size++]=buffer[i];
                            }
                        }
                    }
                }
            }
        }
    }while(brk);
    addr[addr_size++]='\0';
    c[size++]=(char)atoi(addr);
    c[size++]=(char)data_size;
    c[size++]=(char)128;
    c[size++]=(char)0b1110111;
    for(int i=0;i<data_size;i++)
    {
        c[size++]=data[i];
    }
    c[size++]='\0';
    return size;
}


int putString(char * buffer, uint16 count)
{
    while(0u != USBUART_IsConfigurationChanged());
    USBUART_CDC_Init();
    while(0u == USBUART_CDCIsReady());
    USBUART_PutData(buffer, count);
    *(buffer) = '\0';
    return 0;
}
/* [] END OF FILE */
