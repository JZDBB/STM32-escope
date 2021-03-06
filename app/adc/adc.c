#include "adc.h"
#include "math.h"
#include "tim.h"
#include "stm32f10x_it.h"
#include "lcd_driver.h"

#define ADC1_DR_Address    ((u32)0x4001244C)
#define WIDE (250)
#define HIGH (200)
typedef enum {DEC = 0, PLUS = 1} MoveStatus;

u16 a[640];
u16 index2 = 0;
u16 index1 = 0;
u8 flag_change = 0;
u8 flag_50us = 0;
int inter_b = 0;
float multiple = 1;
int mode_hard = 0;
u32 max_data = 0;
u32 min_data = 0;

void DMA1_Init(void)
{
	DMA_InitTypeDef  DMA_InitTypeStruct;

	DMA_DeInit(DMA1_Channel1);	//ADC1的DMA通道

	DMA_InitTypeStruct.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitTypeStruct.DMA_MemoryBaseAddr = (u32)a;
	DMA_InitTypeStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitTypeStruct.DMA_BufferSize = 640;
	DMA_InitTypeStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitTypeStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitTypeStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitTypeStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitTypeStruct.DMA_Mode = DMA_Mode_Normal;
	DMA_InitTypeStruct.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitTypeStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1,&DMA_InitTypeStruct); //DMA初始化，ADC结果寄存器存入SRAM中

	DMA_Cmd(DMA1_Channel1,ENABLE);	//DMA使能
}

void AD_Init(void)
{
	ADC_InitTypeDef  ADC_InitTypeStruct;

	ADC_DeInit(ADC1);  //复位ADC1

	ADC_InitTypeStruct.ADC_Mode = ADC_Mode_Independent;//ADC工作在独立模式
	ADC_InitTypeStruct.ADC_ScanConvMode = DISABLE;	   //单通道模式
	ADC_InitTypeStruct.ADC_ContinuousConvMode = DISABLE; //单次模式
	ADC_InitTypeStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;//选择定时器1来触发
	ADC_InitTypeStruct.ADC_DataAlign = ADC_DataAlign_Right; //数据右对齐
	ADC_InitTypeStruct.ADC_NbrOfChannel = 1;   //通道数目为1
	ADC_Init(ADC1, &ADC_InitTypeStruct);	  //ADC初始化

	ADC_ExternalTrigConvCmd(ADC1,ENABLE);	  //ADC外部触发使能
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_1Cycles5);//配置ADC1，通道1等
}

void ADC1_Init(void)									 //adc初始化
{
	DMA1_Init();
	AD_Init();
	ADC_DMACmd(ADC1, ENABLE);    //ADC DMA使能
	ADC_Cmd(ADC1, ENABLE);		 //ADC使能
	ADC_ResetCalibration(ADC1);	 //ADC校准复位
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);	 //ADC开始校准
	while(ADC_GetCalibrationStatus(ADC1));
}

void ADC_Get_Value(void)								 //得到数据，
{
	//float gao_pin_period = 0;
	DMA1_Init();	                  
	TIM_SetCounter(TIM1,0);	   //清空寄存器
	if(num_shao_miao>3)
	{	
		TIM_PrescalerConfig(TIM1,55,TIM_PSCReloadMode_Immediate);
		//TIM_SetCompare1(TIM1, (shao_miao_shu_du/25)-1);
		TIM_SetAutoreload(TIM1, shao_miao_shu_du/25-1); //设定扫描速度
	}
	else
	{
		//TIM_PrescalerConfig(TIM1,0,TIM_PSCReloadMode_Immediate);
		//gao_pin_period = 288000000.0/frequency +gao_pin_palus-1;
		//TIM_SetCompare1(TIM1, gao_pin_period);
		//TIM_SetAutoreload(TIM1,gao_pin_period);	
		
		TIM_PrescalerConfig(TIM1,1,TIM_PSCReloadMode_Immediate);
		//TIM_SetCompare1(TIM1, (shao_miao_shu_du/25)-1);
		TIM_SetAutoreload(TIM1, 1); //设定扫描速度
	}
	
	TIM_Cmd(TIM1,ENABLE);
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
	DMA_ClearFlag(DMA1_FLAG_TC1);
	if(DMA_GetFlagStatus(DMA1_FLAG_TE1))
	{
		DMA_ClearFlag(DMA1_FLAG_TE1);
		ADC_Get_Value();
	}
	TIM_Cmd(TIM1,DISABLE);
}

u16 ADC_Get_Vpp(void)	   
{
	u32 n=0;
	float pp=0;
	max_data=a[0];
	min_data=a[0];
	for(n = 1;n<320;n++)
	{
		if(a[n]>max_data)
		{
			max_data = a[n];
		}
		if(a[n]<min_data)
		{
			min_data = a[n];
			index2 = n;
		}			
	} 	
	pp = (float)(max_data-min_data);
	pp = pp*(3300.0* multiple /4095);
	return pp;	
}

u16 get_duty(void)
{
		u32 mid = 0;
	u32 i = 0;
	u16 duty = 0;
	mid = (max_data+min_data)/2;
	for(i = 1;i<640;i++)
	{
		if(a[i]>mid)
		{
			duty++;
		}
	}
	duty = duty * 10000 / 640;
	return duty;
}

void clear(void)
{
	int i;
	for(i = 0;i<WIDE;i++)
	{
		lcd_huadian(i,arr_plot[i],BLACK);				
		lcd_huaxian(i,arr_plot[i],i+1,arr_plot[i+1],BLACK);
	}
}

void ADC_print(int ver, int hor)//,MoveStatus State)
{
	int x;
	int i = 0;
	float value = 0;
	float value1 = 0;
	if(flag_50us ==0)
	{
		clear();
		POINT_COLOR = YELLOW;
		/*switch (State)
		{
			case 0:
				for(x=200-hor;x<200-hor+WIDE;x++)
				{
					value1 = a[x] * 3300 / 4096  *  25 /vcc_div - ver;
					value = a[x + 1] * 3300 / 4096 * 25 / vcc_div - ver;
					if(value1>200)
					{
						value1=200;	
					}
					if(value1<0)
					{
						value1=0;	
					}
					if(value>200)
					{
						value=200;	
					}
					if(value<0)
					{
						value=0;	
					}
					lcd_huadian(i,value1,POINT_COLOR);				
					lcd_huaxian(i,value1,i+1,value,POINT_COLOR);
					arr_plot[i] = value1;
					i++;
				}
				break;
			case 1:	
				for(x=200+hor;x<hor+200+WIDE;x++)
				{
					value1 = a[x] * 3300 / 4096  *  25 /vcc_div + ver;
					value = a[x + 1] * 3300 / 4096 * 25 / vcc_div + ver;
						if(value1>200)
						{
							value1=200;	
						}
						if(value1<0)
						{
							value1=0;	
						}
						if(value>200)
						{
							value=200;	
						}
						if(value<0)
						{
							value=0;	
						}
						lcd_huadian(i,value1,POINT_COLOR);				
						lcd_huaxian(i,value1,i+1,value,POINT_COLOR);
						arr_plot[i] = value1;
						i++;
					}
		}*/
		
		for(x=200+hor;x<hor+200+WIDE;x++)
			{
				value1 = a[x] * 3300 * multiple / 4096  *  25 /vcc_div + ver;
				value = a[x + 1] * 3300 * multiple / 4096 * 25 / vcc_div + ver;
				if(value1>HIGH)
				{
					value1=HIGH;	
				}
				if(value1<0)
				{
					value1=0;	
				}
				if(value>HIGH)
				{
					value=HIGH;	
				}
				if(value<0)
				{
					value=0;	
				}
				lcd_huadian(i,value1,YELLOW);				
				lcd_huaxian(i,value1,i+1,value,YELLOW);
				arr_plot[i] = value1;
				i++;
			}
		hua_wang();				
	}
	else
	{
		i = 0;
		clear_inter(inter_b);
		for(x=200+hor;x<hor+200+WIDE/inter;x++)
		{
			value1 = a[x] * 3300 * multiple / 4096  *  25 /vcc_div + ver;
			value = a[x + 1] * 3300 * multiple / 4096 * 25 / vcc_div + ver;
			if(value1>HIGH)
			{
				value1=HIGH;	
			}
			if(value1<0)
			{
				value1=0;	
			}
			if(value>HIGH)
			{
				value=HIGH;	
			}
			if(value<0)
			{
				value=0;	
			}
			lcd_huadian(i*inter,value1,YELLOW);
			lcd_huaxian(i*inter,value1,(i+1)*inter,value,YELLOW);
			arr_plot[i] = value1;
			inter_b = inter;
			i++;
		}
		hua_wang();
	}
}

void clear_inter(int inter)
{
	int i;
	for(i = 0;i<250/inter;i++)
	{
		lcd_huadian(i*inter,arr_plot[i],BLACK);
		lcd_huaxian(i*inter,arr_plot[i],(i+1)*inter,arr_plot[i+1],BLACK);
	}
}

void pause_plot(u16 ver, u16 hor)
{
	int x;
	int i = 0;
	float value = 0;
	float value1 = 0;
	clear();
	POINT_COLOR = YELLOW;
	for(x=200+hor;x<hor+200+WIDE;x++)
	{
		value1 = a[x] * 3300 * multiple / 4096  *  25 /vcc_div + ver;
		value = a[x + 1] * 3300 * multiple / 4096 * 25 / vcc_div + ver;
		if(value1>HIGH)
		{
			value1=HIGH;	
		}
		if(value1<0)
		{
			value1=0;	
		}
		if(value>HIGH)
		{
			value=HIGH;	
		}
		if(value<0)
		{
			value=0;	
		}
		lcd_huadian(i,value1,POINT_COLOR);				
		lcd_huaxian(i,value1,i+1,value,POINT_COLOR);
		arr_plot[i] = value1;
		i++;
	}
	/*for(x = 0;x<WIDE;x++)
	{
		for(y = 0;y<HIGH;y++)
		{
			value = a[200+x]
		}
	}*/
	hua_wang();
}
