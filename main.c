#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "beep.h"
#include "OLED.h"
#include "PWM.h"
#include "IC.h"

 
/************************************************
 ALIENTEK战舰STM32开发板实验3
 按键输入实验  
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

#define ADC_REF_VOLTAGE  3.155f 

int mode = 1; 

int main(void)
{
    uint16_t adValue;
    float voltage;
    uint8_t i;
    
    uint8_t wave_buff[128];     
    uint16_t max_adc_val = 0;   
    uint16_t min_adc_val = 4095; 
    float max_voltage = 0.0;    
    
    
    uint32_t timeout;
    
    
    uint16_t trigger_level = 2048; 

    delay_init();
    OLED_Init();
    AD_Init(); 
    
    OLED_Clear();

    while (1)
    {
        if (mode == 1)
        {
            
            adValue = AD_GetAverage(50); 
            voltage = (float)adValue / 4095 * ADC_REF_VOLTAGE;
            
            OLED_ShowString(1, 1, "Mode: Voltmeter "); 
            OLED_ShowString(3, 1, "V:      ");         
            
            OLED_ShowNum(3, 4, (int)voltage, 1);
            OLED_ShowChar(3, 5, '.');
            OLED_ShowNum(3, 6, (int)(voltage * 100) % 100, 2);
            OLED_ShowString(3, 9, "V");
            
            delay_ms(50);
        }
        else if (mode == 2)
        {
            
            if(trigger_level < 100 || trigger_level > 4000) trigger_level = 2048;

            timeout = 50000; 
            
            while ((AD_GetValue() > trigger_level) && (timeout > 0)) timeout--;
            
            timeout = 50000;
            
            while ((AD_GetValue() < trigger_level) && (timeout > 0)) timeout--;


            
            max_adc_val = 0;   
            min_adc_val = 4095; 

            for (i = 0; i < 128; i++)
            {
                adValue = AD_GetValue();
                
                
                if (adValue > max_adc_val) max_adc_val = adValue;
                if (adValue < min_adc_val) min_adc_val = adValue; 
                
                
                uint8_t y = adValue / 64;
                if(y > 63) y = 63;
                wave_buff[i] = 63 - y; 
                
                delay_us(400); 
            }
            
           
            trigger_level = (max_adc_val + min_adc_val) / 2;
            
            
            max_voltage = (float)max_adc_val / 4095 * ADC_REF_VOLTAGE;

            
            OLED_Clear_Gram(); 
            
            for (i = 0; i < 128; i++)
            {
                OLED_DrawPoint(i, wave_buff[i], 1);
                
                if (i < 127) {
                    uint8_t y1 = wave_buff[i];
                    uint8_t y2 = wave_buff[i+1];
                    uint8_t min = (y1 < y2) ? y1 : y2;
                    uint8_t max = (y1 < y2) ? y2 : y1;
                    for(uint8_t k=min; k<=max; k++) OLED_DrawPoint(i, k, 1);
                } 
            }
            
            OLED_Refresh_Gram(); 
            
            OLED_ShowString(1, 1, "Max:");
            OLED_ShowNum(1, 5, (int)max_voltage, 1);
            OLED_ShowChar(1, 6, '.');
            OLED_ShowNum(1, 7, (int)(max_voltage * 100) % 100, 2);
            OLED_ShowString(1, 9, "V");
            
            delay_ms(100); 
        }
    }
}