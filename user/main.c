/* 程序下载后D1指示灯闪烁表示程序正常运行，按键K_UP切换频率显示控制和电压控制调节，
	K_DOWN按键减，K_RIGHT按键加，AD输入端口采用PA2，可以使用另外一块普中STM32开发板
	下载里面配套的一个脉冲发生程序进去测试，脉冲通过PC1口发送出来通过导线直接输入到示波器
	开发板上的PA2口即可检测到具体的波形，如果只有一块普中STM32开发板的朋友可以使用信号
	发生器产生信号输入到PA2口。程序只是做了一个简单的采集和显示调节，采集的精度和准确度
	还是比较高的。*/

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
//#include "usart.h"


u16 j = 0;
float temp;
float temp1;
u32 frequen = 0;
u8 arr_freq[8] = "0000000\0";
u16 USART_RX_STA=0;       //接收状态标记	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

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

	NVIC_InitTypeStruct.NVIC_IRQChannel = TIM2_IRQn;  		   //配置中断优先级
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


void RCC_HSE_Config(u32 div,u32 pllm) //自定义系统时间（可以修改时钟）
{
	RCC_DeInit(); //将外设RCC寄存器重设为缺省值
	RCC_HSEConfig(RCC_HSE_ON);//设置外部高速晶振（HSE）
	if(RCC_WaitForHSEStartUp()==SUCCESS) //等待HSE起振
	{
		RCC_HCLKConfig(RCC_SYSCLK_Div1);//设置AHB时钟（HCLK）
		RCC_PCLK1Config(RCC_HCLK_Div2);//设置低速AHB时钟（PCLK1）
		RCC_PCLK2Config(RCC_HCLK_Div1);//设置高速AHB时钟（PCLK2）
		RCC_PLLConfig(div,pllm);//设置PLL时钟源及倍频系数
		RCC_PLLCmd(ENABLE); //使能或者失能PLL
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET);//检查指定的RCC标志位设置与否,PLL就绪
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);//设置系统时钟（SYSCLK）
		while(RCC_GetSYSCLKSource()!=0x08);//返回用作系统时钟的时钟源,0x08：PLL作为系统时钟
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
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;				 //外部时钟的，用来测频率的，
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitTypeStruct);

	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;		 		 //adc输入引脚
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitTypeStruct);

	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;		 		 //外部中断的io配置
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOE, &GPIO_InitTypeStruct);

	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;		 		//外部中断的io配置
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitTypeStruct);

	GPIO_InitTypeStruct.GPIO_Pin = GPIO_Pin_8;					 //定时器1触发ad转换的输出的那个口
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

void USART3_Init(u32 bound)
{
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
 
	
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//TX			   //串口输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //复用推挽输出
	GPIO_Init(GPIOB,&GPIO_InitStructure);  /* 初始化串口输入IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;//RX			 //串口输入
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //模拟输入
	GPIO_Init(GPIOB,&GPIO_InitStructure); /* 初始化GPIO */

   //USART3 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Tx;	//收发模式
	USART_Init(USART3, &USART_InitStructure); //初始化串口3
	
	USART_Cmd(USART3, ENABLE);  //使能串口3
	
	USART_ClearFlag(USART3, USART_FLAG_TC);
		
	//USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
	/*NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//串口3中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、	*/
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
/*void USART3_IRQHandler(void)                	//串口3中断服务程序
{
	u8 r;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
	{
		r =USART_ReceiveData(USART3);//(USART3->DR);	//读取接收到的数据
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(r!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(r==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=r ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		
	} 
	
} */

int main(void)
{	
	u8 vpp_buf[7];
	u8 v_buf[7];
	u16 vol;
	v_buf[2] = 46;
	vpp_buf[2] = 46;
	rcc_init();			   //外设时钟配置	
	delay_init();
	led_init();	
	
	nvic_init();		   // 中断优先级配置
	gpio_init();		   	//外设io口配置
	key_init();
	ADC1_Init();	//adc配置
	 
	time_init();			//定时器配置，测频率用的二个定时器
	time_enable();			//同步开始计数
	ADC_Get_Value();
	exti_init2();
	time_init2();
	TFT_Init();
	TFT_ClearScreen(BLACK);
	set_background();	 	 //初始化背景
	USART3_Init(9600);
	
	vpp = ADC_Get_Vpp();
	while(1)
	{
		if(flag == 0)
		{
			int i = 0;
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
					arr_plot[i] = temp;
					i++;
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
}
