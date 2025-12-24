#include "stm32f10x.h"

#define ADC_DEFAULT_CHANNEL ADC_Channel_1
#define ADC_DEFAULT_SAMPLE_TIME ADC_SampleTime_239Cycles5
#define ADC_VREFINT_CHANNEL ADC_Channel_17
#define ADC_VREFINT_VOLTAGE 1.20f

void AD_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // ????
    // ---------------------------------------------------------
    // ??? 1:? Pin_0 ?? Pin_1
    // ---------------------------------------------------------
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;     
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // ---------------------------------------------------------
    // ??? 2:? ADC_Channel_0 ?? ADC_Channel_1
    // ---------------------------------------------------------
    // ????????????? (239.5??),????
    ADC_RegularChannelConfig(ADC1, ADC_DEFAULT_CHANNEL, 1, ADC_DEFAULT_SAMPLE_TIME);
    
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;     
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    
    ADC_Cmd(ADC1, ENABLE);

    ADC_TempSensorVrefintCmd(ENABLE);
    
    // ??ADC
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1) == SET);
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1) == SET);
}


uint16_t AD_GetValue(void)
{
    ADC_RegularChannelConfig(ADC1, ADC_DEFAULT_CHANNEL, 1, ADC_DEFAULT_SAMPLE_TIME);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    return ADC_GetConversionValue(ADC1);
}


uint16_t AD_GetAverage(uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;
    for (t = 0; t < times; t++)
    {
        temp_val += AD_GetValue();
        __NOP(); __NOP(); __NOP();
			//delay_us(10);
    }
    return temp_val / times;
}


float AD_GetVddaVoltage(void)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < 16; t++)
    {
        ADC_RegularChannelConfig(ADC1, ADC_VREFINT_CHANNEL, 1, ADC_DEFAULT_SAMPLE_TIME);
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        temp_val += ADC_GetConversionValue(ADC1);
    }

    if (temp_val == 0)
    {
        return 0.0f;
    }

    return (ADC_VREFINT_VOLTAGE * 4095.0f) / (temp_val / 16.0f);
}
