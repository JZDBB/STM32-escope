#include "stm32f10x_it.h"
#include "gui.h"
#include "sys.h"
#include "delay.h"
#include "tim.h"
#include "adc.h"
#include "button.h"
#include "led.h"

#define WIDE (250)
#define HIGH (200)

u8 frequency_flag = 0;
long int shao_miao_shu_du = 0;
u8 num_shao_miao = 5;
u8 mode1 = 0;
u8 mode2 = 0;
u8 num_fu_du =3;
float gao_pin_palus = 0;
u16 vcc_div = 0;
u16 vpp;
int ver = 0;
int hor = 0;
float arr_plot[250];
int flag = 0;
int inter = 0;
int flag_mode = 0;
int gain = 1;
u8 C_dc_ac = 0;
u8 change_gain = 0;

u8 arr_F[13][11] = {"  5us/div\0"," 10us/div\0"," 20us/div\0"," 50us/div\0","100us/div\0","200us/div\0","500us/div\0","  1ms/div\0","  2ms/div\0","  5ms/div\0"," 10ms/div\0"," 20ms/div\0"," 50ms/div\0"};
u8 arr_V[7][11] = {"100mV/div\0","200mV/div\0","500mV/div\0","   1V/div\0","   2V/div\0","   5V/div\0","  10V/div\0"};
u8 arr_gain[8][6] = {"0 0 0\0","0 0 1\0","0 1 0\0","0 1 1\0","1 0 0\0","1 0 1\0","1 1 0\0","1 1 1\0"};
u8 arr_C[2][10] = {" AC\0", " DC\0"};
//u8 arr_f[10] = "  5us/div";
u8 arr_move[2][10] ={" mov_hor\0"," mov_ver\0"};
u8 arr_JDQ[2][3] = {" 0\0"," 1\0"};
float gain_num[8] = {0.05, 0.1, 0.2, 0.4, 8, 10, 20, 25};
float gain_multiple0[8] = {0.036,0.063,0.12,0.226,0.49,0.57,1.13,3};
float gain_multiple1[8] = {0.264,0.54,1.05,2.2,4.2,5.2,9.85,24};
u8 show_gain0[8][8] = {"0.025\0","0.05 \0","0.1  \0","0.2  \0","0.4  \0","0.5  \0","1    \0","2.5  \0"};
u8 show_gain1[8][8] = {"0.25 \0","0.5  \0","1    \0","2    \0","4    \0","5    \0","10   \0","25   \0"};

void set_io1(void)					  										
{
	GPIO_SetBits(GPIOC,GPIO_Pin_3);	
	GPIO_SetBits(GPIOC,GPIO_Pin_4);
	GPIO_SetBits(GPIOC,GPIO_Pin_5);
}
void set_io2(void)					  										
{
	GPIO_SetBits(GPIOC,GPIO_Pin_3);	
	GPIO_SetBits(GPIOC,GPIO_Pin_4);
	GPIO_ResetBits(GPIOC,GPIO_Pin_5);
}
void set_io3(void)					  										
{
	GPIO_SetBits(GPIOC,GPIO_Pin_3);	
	GPIO_ResetBits(GPIOC,GPIO_Pin_4);
	GPIO_SetBits(GPIOC,GPIO_Pin_5);
}
void set_io4(void)					  										
{
	GPIO_SetBits(GPIOC,GPIO_Pin_3);	
	GPIO_ResetBits(GPIOC,GPIO_Pin_4);
	GPIO_ResetBits(GPIOC,GPIO_Pin_5);
}
void set_io5(void)					  										
{
	GPIO_ResetBits(GPIOC,GPIO_Pin_3);	
	GPIO_SetBits(GPIOC,GPIO_Pin_4);
	GPIO_SetBits(GPIOC,GPIO_Pin_5);
}
void set_io6(void)					  										
{
	GPIO_ResetBits(GPIOC,GPIO_Pin_3);	
	GPIO_SetBits(GPIOC,GPIO_Pin_4);
	GPIO_ResetBits(GPIOC,GPIO_Pin_5);
}
void set_io7(void)					  										
{
	GPIO_ResetBits(GPIOC,GPIO_Pin_3);	
	GPIO_ResetBits(GPIOC,GPIO_Pin_4);
	GPIO_SetBits(GPIOC,GPIO_Pin_5);
}
void set_io8(void)					  										
{
	GPIO_ResetBits(GPIOC,GPIO_Pin_3);	
	GPIO_ResetBits(GPIOC,GPIO_Pin_4);
	GPIO_ResetBits(GPIOC,GPIO_Pin_5);
}
void lcd_huadian(u16 a,u16 b,u16 color)
{							    
	GUI_Dot(a,200-b,color);
}

void lcd_huaxian(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
	GUI_Line(x1,200-y1,x2,200-y2,color);	
}

void hua_wang(void)
{
	u16 index_y = 0;
	u16 index_hang = 0;	

	POINT_COLOR = BLUE;		
	for(index_hang = 0;index_hang< 250;index_hang = index_hang + 25)
	{
		for(index_y = 0;index_y<200;index_y = index_y +5)
		{
			lcd_huadian(index_hang,index_y,POINT_COLOR);	
		}
	}	
	for(index_hang = 0;index_hang<200;index_hang = index_hang + 25)
	{
		for(index_y = 0;index_y<250;index_y = index_y +5)
		{
			lcd_huadian(index_y,index_hang,POINT_COLOR);	
		}
	}
	POINT_COLOR = BLUE;
	LCD_DrawRectangle(0,0,250,200,POINT_COLOR);
	GUI_Line(0,100,250,100,POINT_COLOR);
	GUI_Line(125,0,125,200,POINT_COLOR);
	POINT_COLOR=RED;
}
void set_background(void)
{
	POINT_COLOR = BLUE;
  TFT_ClearScreen(BLACK);
	
	LCD_DrawRectangle(0,0,250,200,POINT_COLOR);
	GUI_Line(0,100,250,100,POINT_COLOR);
	GUI_Line(125,0,125,200,POINT_COLOR);

	POINT_COLOR=RED;
	LCD_DrawRectangle(252,0,346,200,RED);
	GUI_Show12ASCII(262,5,"V(V):",POINT_COLOR,BLACK);
	//GUI_Show12ASCII(262,210,"Vmax:",POINT_COLOR,YELLOW);
	//GUI_Show12ASCII(262,210,"Vmin:",POINT_COLOR,YELLOW);
	GUI_Show12ASCII(262,45,"Vpp(V):",POINT_COLOR,BLACK);
	GUI_Show12ASCII(262,85,"Freq(Hz):",POINT_COLOR,BLACK);
	GUI_Show12ASCII(262,125,"duty(%):",POINT_COLOR,BLACK);
	
	POINT_COLOR=BLUE;
	LCD_DrawRectangle(348,0,399,200,WHITE);
	LCD_DrawRectangle(351,2,396,20,POINT_COLOR);
	LCD_DrawRectangle(351,30,396,66,POINT_COLOR);
	LCD_DrawRectangle(351,76,396,112,POINT_COLOR);
	GUI_Show12ASCII(354,4,arr_C[0],POINT_COLOR,BLACK);
	//GUI_Show12ASCII(262,210,"Vmax:",POINT_COLOR,YELLOW);
	//GUI_Show12ASCII(262,210,"Vmin:",POINT_COLOR,YELLOW);
	GUI_Show12ASCII(354,32," SJ ",POINT_COLOR,BLACK);
	GUI_Show12ASCII(354,78,"C B A",POINT_COLOR,BLACK);
	GUI_Show12ASCII(354,94,arr_gain[gain-1],BLUE,BLACK);
	GUI_Show12ASCII(354,48,arr_JDQ[change_gain],BLUE,BLACK);
	GUI_Show12ASCII(354,4,arr_C[C_dc_ac],BLUE,BLACK);

	POINT_COLOR=BLUE;
	LCD_DrawRectangle(0,202,398,236,GREEN);
	GUI_Box(2,205,76,230,YELLOW);
	GUI_Box(92,205,166,230,YELLOW);
	GUI_Box(182,205,256,230,YELLOW);
	GUI_Box(272,205,346,230,YELLOW);
	LCD_DrawRectangle(2,205,76,230,RED);
	GUI_Show12ASCII(4,210,arr_V[2],POINT_COLOR,YELLOW);	
	GUI_Show12ASCII(94,210,arr_F[4],POINT_COLOR,YELLOW);
	GUI_Show12ASCII(184,210,arr_move[0],POINT_COLOR,YELLOW);
	GUI_Show12ASCII(274,210,"load data",POINT_COLOR,YELLOW);
}

void key_init(void)
{
	EXTI_InitTypeDef   EXTI_InitTypeStruct;

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);	 //K_UP
	EXTI_InitTypeStruct.EXTI_Line = EXTI_Line0;
	EXTI_InitTypeStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitTypeStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitTypeStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitTypeStruct);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3);	   //K_DOWN
	EXTI_InitTypeStruct.EXTI_Line = EXTI_Line3;
	EXTI_InitTypeStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitTypeStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitTypeStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitTypeStruct);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);	  //K_RIGHT
	EXTI_InitTypeStruct.EXTI_Line = EXTI_Line4;
	EXTI_InitTypeStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitTypeStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitTypeStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitTypeStruct);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);	  //K_LEFT
	EXTI_InitTypeStruct.EXTI_Line = EXTI_Line2;
	EXTI_InitTypeStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitTypeStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitTypeStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitTypeStruct);
}

void load_data()
{
	int i;
	for(i = 0;i<640;i++)
	{
		USART_SendData(USART3, (u8)(a[i]>>8));
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
		USART_SendData(USART3, (u8)(a[i]<<8>>8));
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
	}
}

void EXTI0_IRQHandler(void)
{
	delay_ms(10);
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
	{
		mode1++;
		led0=0;
		flag_mode = 0;
		if(mode1 == 5)mode1 = 0;
		if(mode1 == 0)
		{
			LCD_DrawRectangle(2,205,76,230,RED);
			LCD_DrawRectangle(92,205,166,230,YELLOW);
			LCD_DrawRectangle(182,205,256,230,YELLOW);
			LCD_DrawRectangle(272,205,346,230,YELLOW);
		}
		else if(mode1 == 1)
		{
			LCD_DrawRectangle(2,205,76,230,YELLOW);
			LCD_DrawRectangle(92,205,166,230,RED);
			LCD_DrawRectangle(182,205,256,230,YELLOW);
			LCD_DrawRectangle(272,205,346,230,YELLOW);
		}
		else if(mode1 ==2)
		{
			LCD_DrawRectangle(2,205,76,230,YELLOW);
			LCD_DrawRectangle(92,205,166,230,YELLOW);
			LCD_DrawRectangle(182,205,256,230,RED);
			LCD_DrawRectangle(272,205,346,230,YELLOW);
			DMA_Cmd(DMA1_Channel1,DISABLE);
			TIM_Cmd(TIM1,DISABLE);
			flag = 1;
			//pause_plot(ver, hor);
			ADC_print(ver, hor);		
		}
		else if(mode1 ==3)
		{
			LCD_DrawRectangle(2,205,76,230,YELLOW);
			LCD_DrawRectangle(92,205,166,230,YELLOW);
			LCD_DrawRectangle(182,205,256,230,RED);
			LCD_DrawRectangle(272,205,346,230,YELLOW);
			GUI_Show12ASCII(184,210,arr_move[1],BLUE,YELLOW);
			DMA_Cmd(DMA1_Channel1,DISABLE);
			TIM_Cmd(TIM1,DISABLE);
			flag = 1;
			//pause_plot(ver, hor);
			ADC_print(ver, hor);
		}
		else if(mode1 ==4)
		{
			LCD_DrawRectangle(2,205,76,230,YELLOW);
			LCD_DrawRectangle(92,205,166,230,YELLOW);
			LCD_DrawRectangle(182,205,256,230,YELLOW);
			LCD_DrawRectangle(272,205,346,230,RED);
			GUI_Show12ASCII(184,210,arr_move[0],BLUE,YELLOW);
			DMA_Cmd(DMA1_Channel1,DISABLE);
			TIM_Cmd(TIM1,DISABLE);
			flag = 0;
			ver = 0;
			hor = 0;
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}	   

void EXTI2_IRQHandler()	
{
	delay_ms(100);
	if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)==0)
	{
   	mode2++;
		flag_mode = 1;
		if(mode2 ==3)mode2 = 0;
		if(mode2 ==0)
		{
			LCD_DrawRectangle(351,2,396,20,WHITE);
			LCD_DrawRectangle(351,30,396,66,BLUE);
			LCD_DrawRectangle(351,76,396,112,BLUE);
		}
		else if(mode2 ==1)
		{
			LCD_DrawRectangle(351,2,396,20,BLUE);
			LCD_DrawRectangle(351,30,396,66,WHITE);
			LCD_DrawRectangle(351,76,396,112,BLUE);
		}
		else if(mode2 ==2)
		{
			LCD_DrawRectangle(351,2,396,20,BLUE);
			LCD_DrawRectangle(351,30,396,66,BLUE);
			LCD_DrawRectangle(351,76,396,112,WHITE);
		}
	}		
	EXTI_ClearITPendingBit(EXTI_Line2);
}

void EXTI3_IRQHandler(void)
{
	delay_ms(10);
	led1=0;
	if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)==0)
	{
		if(mode1 == 0 & flag_mode==0)
		{
			num_fu_du++;
			if(num_fu_du==8)num_fu_du=1;
		}
		else if(mode1==1&flag_mode==0)
		{	
			num_shao_miao++;
			if(num_shao_miao == 14)num_shao_miao = 1;
		}
		else if(mode1==2& flag_mode==0)
		{
			hor = hor+10;
			ADC_print(ver,hor);
		}
		else if(mode1==3& flag_mode==0)
		{
			ver = ver+10;
			ADC_print(ver,hor);
		}
		else if(mode1==4& flag_mode==0)
		{
			load_data();
		}
		else if(mode2 ==0& flag_mode==1)
		{
			if(C_dc_ac ==0)
			{
				C_dc_ac =1;
			}
			else
			{
				C_dc_ac =0;
			}
		}
		else if(mode2==1& flag_mode==1)
		{
			if(change_gain ==0)
			{
				change_gain =1;
			}
			else
			{
				change_gain =0;
			}
		}
		else if(mode2 ==2& flag_mode==1)
		{
			gain++;
			if(gain==9)gain = 1;
		}
		GUI_Show12ASCII(94,210,arr_F[num_shao_miao-1],BLUE,YELLOW);
		GUI_Show12ASCII(4,210,arr_V[num_fu_du-1],BLUE,YELLOW);
		GUI_Show12ASCII(354,94,arr_gain[gain-1],BLUE,BLACK);
		GUI_Show12ASCII(354,48,arr_JDQ[change_gain],BLUE,BLACK);
		GUI_Show12ASCII(354,4,arr_C[C_dc_ac],BLUE,BLACK);
	}
	EXTI_ClearITPendingBit(EXTI_Line3);
}

void EXTI4_IRQHandler(void)
{
	delay_ms(10);
	led2=0;
	if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)==0)
	{
		if(mode1 == 0& flag_mode==0)
		{
			num_fu_du--;
			if(num_fu_du==0)num_fu_du=7;
		}
		else if(mode1==1& flag_mode==0)
		{	
			num_shao_miao--;
			if(num_shao_miao == 0)num_shao_miao = 13;
		}
		else if(mode1==2& flag_mode==0)
		{
			hor = hor-10;
			ADC_print(ver,hor);
		}
		else if(mode1==3& flag_mode==0)
		{
			ver = ver-10;
			ADC_print(ver,hor);
		}
		else if(mode1==4& flag_mode==0)
		{
			
		}
		else if(mode2 ==0& flag_mode==1)
		{
			
		}
		else if(mode2==1& flag_mode==1)
		{
			
		}
		else if(mode2 ==2& flag_mode==1)
		{
			gain--;
			if(gain==0)gain = 8;
		}
		GUI_Show12ASCII(94,210,arr_F[num_shao_miao-1],BLUE,YELLOW);
		GUI_Show12ASCII(4,210,arr_V[num_fu_du-1],BLUE,YELLOW);
		GUI_Show12ASCII(354,94,arr_gain[gain-1],BLUE,BLACK);
	}
	EXTI_ClearITPendingBit(EXTI_Line4);
}

void TIM2_IRQHandler(void)
{
	//u16 temple;
	//u16 yan_se;
	if(TIM_GetITStatus(TIM2, TIM_IT_Update))
	{
		TIM_Cmd(TIM2,DISABLE);

   	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		//frequency = 65536*count+ temple;
		//frequency = frequency - frequency*(130.10/1000000);
		
		switch(num_shao_miao)
		{
			/*case 1:shao_miao_shu_du = 347;gao_pin_palus = 1;break;
			
			case 2:shao_miao_shu_du = 694;gao_pin_palus = 2;break;
			case 3:shao_miao_shu_du = 1736;gao_pin_palus = 5;break;
			case 4:shao_miao_shu_du = 3472;gao_pin_palus = 10;break;
			case 5:shao_miao_shu_du = 6944;gao_pin_palus = 20;break;
			case 6:shao_miao_shu_du = 17361;gao_pin_palus = 50;break;
			case 7:shao_miao_shu_du = 34722;gao_pin_palus = 100;break;	  
			case 8:shao_miao_shu_du = 50;break;		  //分界点
			case 9:shao_miao_shu_du = 100;break;
			case 10:shao_miao_shu_du = 200;break;
			case 11:shao_miao_shu_du = 500;break;
			case 12:shao_miao_shu_du = 1000;break;
			case 13:shao_miao_shu_du = 2000;break;
			case 14:shao_miao_shu_du = 5000;break;
			case 15:shao_miao_shu_du = 10000;break;
			case 16:shao_miao_shu_du = 20000;break;
			case 17:shao_miao_shu_du = 50000;break;
			case 18:shao_miao_shu_du = 100000;break;
			case 19:shao_miao_shu_du = 200000;break;
			case 20:shao_miao_shu_du = 500000;break;
			case 21:shao_miao_shu_du = 1000000;break;*/
			case 1:shao_miao_shu_du = 5;inter = 5;flag_50us=1;break;
			case 2:shao_miao_shu_du = 10;inter = 3;flag_50us=1;break;	
			case 3:shao_miao_shu_du = 20;inter = 2;flag_50us=1;break;
			case 4:shao_miao_shu_du = 50;inter = 0;flag_50us=0;break;		  //分界点
			case 5:shao_miao_shu_du = 100;inter = 0;flag_50us=0;break;
			case 6:shao_miao_shu_du = 200;inter = 0;flag_50us=0;break;
			case 7:shao_miao_shu_du = 500;inter = 0;flag_50us=0;break;
			case 8:shao_miao_shu_du = 1000;inter = 0;flag_50us=0;break;
			case 9:shao_miao_shu_du = 2000;inter = 0;flag_50us=0;break;
			case 10:shao_miao_shu_du = 5000;inter = 0;flag_50us=0;break;
			case 11:shao_miao_shu_du = 10000;inter = 0;flag_50us=0;break;
			case 12:shao_miao_shu_du = 20000;inter = 0;flag_50us=0;break;
			case 13:shao_miao_shu_du = 50000;inter = 0;flag_50us=0;break;
			default :break;
		}
		switch(num_fu_du)
		{
/*			case 1:vcc_div=1000;set_io1();break;
			case 2:vcc_div=950;set_io2();break;
			case 3:vcc_div=900;set_io3();break;
			case 4:vcc_div=800;set_io4();break;
			case 5:vcc_div=700;set_io5();break;
			case 6:vcc_div=600;set_io6();break;
			case 7:vcc_div=500;set_io7();break;
			case 8:vcc_div=400;set_io8();break;
			case 9:vcc_div=300;set_io9();break;
			case 10:vcc_div=200;set_io10();break;
			case 11:vcc_div=100;set_io11();break;*/
			
			case 1:vcc_div=100;break;
			case 2:vcc_div=200;break;
			case 3:vcc_div=500;break;
			case 4:vcc_div=1000;break;
			case 5:vcc_div=2000;break;
			case 6:vcc_div=5000;break;
			case 7:vcc_div=10000;break;
			default :break;
		}

		/*shao_miao_shu_du_buf[0]=shao_miao_shu_du/1000000+0x30;
		shao_miao_shu_du_buf[1]=shao_miao_shu_du%1000000/100000+0x30;
		shao_miao_shu_du_buf[2]=shao_miao_shu_du%1000000%100000/10000+0x30;
		shao_miao_shu_du_buf[3]=shao_miao_shu_du%1000000%100000%10000/1000+0x30;
		shao_miao_shu_du_buf[4]=shao_miao_shu_du%1000000%100000%10000%1000/100+0x30;
		shao_miao_shu_du_buf[5]=shao_miao_shu_du%1000000%100000%10000%1000%100/10+0x30;
		shao_miao_shu_du_buf[6]=shao_miao_shu_du%1000000%100000%10000%1000%100%10+0x30;
		shao_miao_shu_du_buf[7]='\0';*/
		
/*		if(num_shao_miao>7)
		{
			shao_miao_shu_du_buf[3] = 109;
			shao_miao_shu_du_buf[0]=32;
			shao_miao_shu_du_buf[1] = 32;
			shao_miao_shu_du_buf[2] = 32;
			if(shao_miao_shu_du<10)
			{
				shao_miao_shu_du_buf[2] = shao_miao_shu_du+48;
			}
			else if(shao_miao_shu_du<100)
			{
				shao_miao_shu_du_buf[1] = shao_miao_shu_du/10+48;
				shao_miao_shu_du_buf[2] = 48;
			}
			else
			{
				shao_miao_shu_du_buf[0] = shao_miao_shu_du/100+48;
				shao_miao_shu_du_buf[1] = (shao_miao_shu_du/10)%10+48;
				shao_miao_shu_du_buf[2] = shao_miao_shu_du%10+48;
			}
		}
		else
		{
			shao_miao_shu_du_buf[3] = 117;
			shao_miao_shu_du_buf[0]=32;
			shao_miao_shu_du_buf[1] = 32;
			shao_miao_shu_du_buf[2] = 32;
			if(shao_miao_shu_du<10)
			{
				shao_miao_shu_du_buf[2] = shao_miao_shu_du+48;
			}
			else if(shao_miao_shu_du<100)
			{
				shao_miao_shu_du_buf[1] = shao_miao_shu_du/10+48;
				shao_miao_shu_du_buf[2] = 48;
			}
			else
			{
				shao_miao_shu_du_buf[0] = shao_miao_shu_du/100+48;
				shao_miao_shu_du_buf[1] = (shao_miao_shu_du/10)%10+48;
				shao_miao_shu_du_buf[2] = shao_miao_shu_du%10+48;
			}
		}
		shao_miao_shu_du_buf[4] = '\0';

		vcc_div_buf[0]=vcc_div/1000000+0x30;
		vcc_div_buf[1]=vcc_div%1000000/100000+0x30;
		vcc_div_buf[2]=vcc_div%1000000%100000/10000+0x30;
		vcc_div_buf[3]=vcc_div%1000000%100000%10000/1000+0x30;
		vcc_div_buf[4]=vcc_div%1000000%100000%10000%1000/100+0x30;
		vcc_div_buf[5]=vcc_div%1000000%100000%10000%1000%100/10+0x30;
		vcc_div_buf[6]=vcc_div%1000000%100000%10000%1000%100%10+0x30;
		vcc_div_buf[7]='\0';*/
		
		
		/*yan_se = POINT_COLOR;
		POINT_COLOR=BLUE;
		if(frequency>20000)
		{
			frequency_flag = 1;	
		}
		else
		{
			frequency_flag = 0;			
		}

		POINT_COLOR=yan_se;*/
		
		switch(C_dc_ac)
		{
			case 0:GPIO_SetBits(GPIOC,GPIO_Pin_7);break;
			case 1:GPIO_ResetBits(GPIOC,GPIO_Pin_7);break;
			default: break;
		}
		
		switch(change_gain)
		{
			case 0:
				GPIO_SetBits(GPIOC,GPIO_Pin_6);
				switch(gain)
				{	
					case 1:set_io1();break;
					case 2:set_io2();break;
					case 3:set_io3();break;
					case 4:set_io4();break;
					case 5:set_io5();break;
					case 6:set_io6();break;
					case 7:set_io7();break;
					case 8:set_io8();break;
					default :break;
				}
				multiple =1/(gain_multiple0[gain-1]);
				GUI_Show12ASCII(354,150,show_gain0[gain-1],BLUE,BLACK);
			break;
			case 1:
				GPIO_ResetBits(GPIOC,GPIO_Pin_6);
				switch(gain)
				{	
					case 1:set_io1();break;
					case 2:set_io2();break;
					case 3:set_io3();break;
					case 4:set_io4();break;
					case 5:set_io5();break;
					case 6:set_io6();break;
					case 7:set_io7();break;
					case 8:set_io8();break;
					default :break;
				}
				multiple =1/(gain_multiple1[gain-1]);
				GUI_Show12ASCII(354,150,show_gain1[gain-1],BLUE,BLACK);
			break;
			default: break;
		}	
		TIM_SetCounter(TIM2,0);
		TIM_Cmd(TIM2,ENABLE);
	}
}

