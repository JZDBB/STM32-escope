/* �������غ�D1ָʾ����˸��ʾ�����������У�����K_UP�л�Ƶ����ʾ���ƺ͵�ѹ���Ƶ��ڣ�
	K_DOWN��������K_RIGHT�����ӣ�AD����˿ڲ���PA2������ʹ������һ������STM32������
	�����������׵�һ�����巢�������ȥ���ԣ�����ͨ��PC1�ڷ��ͳ���ͨ������ֱ�����뵽ʾ����
	�������ϵ�PA2�ڼ��ɼ�⵽����Ĳ��Σ����ֻ��һ������STM32����������ѿ���ʹ���ź�
	�����������ź����뵽PA2�ڡ�����ֻ������һ���򵥵Ĳɼ�����ʾ���ڣ��ɼ��ľ��Ⱥ�׼ȷ��
	���ǱȽϸߵġ�*/

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "button.h"
#include "lcd_driver.h"
#include "gui.h"
#include "tim.h"
#include "stm32f10x_it.h"
#include "adc.h"


u16 j = 0;
float temp;
float temp1;
u32 frequen = 0;
u8 arr_freq[8] = "0000000\0";

void clear_point(u16 hang)
{
	u8 index_clear_lie = 0; 
	POINT_COLOR = BLACK;
	for(index_clear_lie = 0;index_clear_lie <201;index_clear_lie++)
	{		
		lcd_huadian(hang,index_clear_lie,POINT_COLOR);
	}	
	POINT_COLOR=YELLOW;	
}
void nvic_init(void)
{
	NVIC_InitTypeDef    NVIC_InitTypeStruct;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitTypeStruct.NVIC_IRQChannel = EXTI0_IRQn; 
	NVIC_InitTypeStruct.NVIC_IRQChannelPreemptionPriority =	2;
	NVIC_InitTypeStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitTypeStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitTypeStruct);				 

	NVIC_InitTypeStruct.NVIC_IRQChannel = EXTI3_IRQn; 
	NVIC_InitTypeStruct.NVIC_IRQChannelPreemptionPriority =	2;
	NVIC_InitTypeStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitTypeStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitTypeStruct);

	NVIC_InitTypeStruct.NVIC_IRQChannel = EXTI4_IRQn; 
	NVIC_InitTypeStruct.NVIC_IRQChannelPreemptionPriority =	2;
	NVIC_InitTypeStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitTypeStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitTypeStruct);

	NVIC_InitTypeStruct.NVIC_IRQChannel = TIM2_IRQn;  		   //�����ж����ȼ�
	NVIC_InitTypeStruct.NVIC_IRQChannelPreemptionPriority =	0;
	NVIC_InitTypeStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitTypeStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitTypeStruct);

	NVIC_InitTypeStruct.NVIC_IRQChannel = TIM3_IRQn; 
	NVIC_InitTypeStruct.NVIC_IRQChannelPreemptionPriority =	0;
	NVIC_InitTypeStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitTypeStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitTypeStruct);
}


void RCC_HSE_Config(u32 div,u32 pllm) //�Զ���ϵͳʱ�䣨�����޸�ʱ�ӣ�
{
	RCC_DeInit(); //������RCC�Ĵ�������Ϊȱʡֵ
	RCC_HSEConfig(RCC_HSE_ON);//�����ⲿ���پ���HSE��
	if(RCC_WaitForHSEStartUp()==SUCCESS) //�ȴ�HSE����
	{
		RCC_HCLKConfig(RCC_SYSCLK_Div1);//����AHBʱ�ӣ�HCLK��
		RCC_PCLK1Config(RCC_HCLK_Div2);//���õ���AHBʱ�ӣ�PCLK1��
		RCC_PCLK2Config(RCC_HCLK_Div1);//���ø���AHBʱ�ӣ�PCLK2��
		RCC_PLLConfig(div,pllm);//����PLLʱ��Դ����Ƶϵ��
		RCC_PLLCmd(ENABLE); //ʹ�ܻ���ʧ��PLL
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET);//���ָ����RCC��־λ�������,PLL����
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);//����ϵͳʱ�ӣ�SYSCLK��
		while(RCC_GetSYSCLKSource()!=0x08);//��������ϵͳʱ�ӵ�ʱ��Դ,0x08��PLL��Ϊϵͳʱ��
	}
}
void rcc_init(void)
{
	RCC_HSE_Config(RCC_PLLSource_HSE_Div1,RCC_PLLMul_7);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div4);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
}

void gpio_init(void)
{
	

	GPIO_InitTypeDef GPIO_InitTypeStruct;

	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;				 //�ⲿʱ�ӵģ�������Ƶ�ʵģ�
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitTypeStruct);

	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;		 		 //adc��������
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitTypeStruct);

	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;		 		 //�ⲿ�жϵ�io����
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOE, &GPIO_InitTypeStruct);

	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;		 		//�ⲿ�жϵ�io����
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitTypeStruct);

	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_8;					 //��ʱ��1����adת����������Ǹ���
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitTypeStruct);

	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;		 		// adc3
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitTypeStruct);

	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;		 		//	adc4
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitTypeStruct);

	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;		 		//	adc5
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitTypeStruct);

	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;		 		//	adc6
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitTypeStruct);

	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;		 		// adc7
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitTypeStruct);
}


void exti_init2() 
{

	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	 
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);

	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);		 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn; 	//??EXTI2?????
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //??????0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //??????0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		  //??
	NVIC_Init(&NVIC_InitStructure); 		
}


void time_init2()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;	 //?????????,?????GPIO
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	TIM_TimeBaseInitStructure.TIM_Period = 2000;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 27999;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);	
	TIM_Cmd(TIM3,ENABLE);

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}

void TIM3_IRQHandler()
{
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	//freq=100;
	frequen=count;
	arr_freq[0] = frequen/1000000%10+48;
		arr_freq[1] = frequen/100000%10+48;
		arr_freq[2] = frequen/10000%10+48;
		arr_freq[3] = frequen/1000%10+48;
		arr_freq[4] = frequen/100%10+48;
		arr_freq[5] = frequen/10%10+48;
		arr_freq[6] = frequen%10+48;
		arr_freq[7] = '\0';
	
	count=0;	
}


void EXTI2_IRQHandler()	
{

	if(EXTI_GetITStatus(EXTI_Line2)==SET)

	{
   		EXTI_ClearITPendingBit(EXTI_Line2);
		count++;
	}		
}
int main(void)
{	
	u8 vpp_buf[7];
	u8 v_buf[7];
	u16 vol;
	v_buf[2] = 46;
	vpp_buf[2] = 46;
	rcc_init();			   //����ʱ������	
	delay_init();
	led_init();	
	
	nvic_init();		   // �ж����ȼ�����
	gpio_init();		   	//����io������
	set_io0();
	key_init();
	ADC1_Init();	//adc����
	
	 
	time_init();			//��ʱ�����ã���Ƶ���õĶ�����ʱ��
	time_enable();			//ͬ����ʼ����
	ADC_Get_Value();
	exti_init2();
	time_init2();
	TFT_Init();
	TFT_ClearScreen(BLACK);
	set_background();	 	 //��ʼ������
	vpp = ADC_Get_Vpp();
	while(1)
	{	
	
		for(j=index;j<index+250;j++)
		{
      temp = a[j] * 3300 / 4096  *  25 /vcc_div;
			temp1 = a[j + 1] * 3300 / 4096 * 25 / vcc_div;
//			if(interval == 0)
//			{
				clear_point(j-index);
				if(temp>200)
				{
					temp=200;	
				}
				if(temp<0)
				{
					temp=0;	
				}
				if(temp1>200)
				{
					temp1=200;	
				}
				if(temp1<0)
				{
					temp1=0;	
				}
				lcd_huadian(j-index,temp,POINT_COLOR);				
				lcd_huaxian(j-index,temp,j-index+1,temp1,POINT_COLOR);
				hua_wang();	
/*			}
			else
			{
				for(i = 0;i<interval;i++)
				{
					clear_point(j-index+interval);
				}
				//clear_point(j-index);
				if(temp>200)
				{
					temp=200;	
				}
				if(temp<0)
				{ss
					temp=0;	
				}
				if(temp1>200)
				{
					temp1=200;	
				}
				if(temp1<0)
				{
					temp1=0;	
				}
				lcd_huadian(j-index,temp,POINT_COLOR);				
				lcd_huaxian(j-index,temp,j-index+interval,temp1,POINT_COLOR);
				hua_wang();
				j = j+interval;
			}*/
			vol = a[j + 1] * 3300 / 4095;
			v_buf[0] = vol/10000+48;
			v_buf[1] = vol%10000/1000+48;
			v_buf[3] = vol%10000%1000/100+48;
			v_buf[4] = vol%10000%1000%100/10+48;
			v_buf[5] = vol%10000%1000%100%10+48;
			v_buf[6] = '\0';
			GUI_Show12ASCII(262,22,v_buf,POINT_COLOR,BLACK);
			//GUI_Show12ASCII(262,22,v_buf,POINT_COLOR,BLACK);
		}
		vpp_buf[0]=vpp/10000+0x30;
		vpp_buf[1]=vpp%10000/1000+0x30;		
		vpp_buf[3]=vpp%10000%1000/100+0x30;
		vpp_buf[4]=vpp%10000%1000%100/10+0x30;
		vpp_buf[5]=vpp%10000%1000%100%10+0x30;
		vpp_buf[6]='\0';
		GUI_Show12ASCII(262,62,vpp_buf,POINT_COLOR,BLACK);	
		
		GUI_Show12ASCII(262,112,arr_freq,RED,BLACK);
		ADC_Get_Value();
		vpp = ADC_Get_Vpp();
	}
}
