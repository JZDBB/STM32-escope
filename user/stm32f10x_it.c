#include "stm32f10x_it.h"
#include "gui.h"
#include "sys.h"
#include "delay.h"
#include "tim.h"
#include "adc.h"
#include "button.h"
#include "led.h"
u8 frequency_flag = 0;
long int shao_miao_shu_du = 0;
u8 num_shao_miao = 1;
u8 mode = 0;
u8 num_fu_du =1;
u8 ad_flag = 1;
float gao_pin_palus = 0;
u16 vcc_div = 0;
u16 vpp;


static u8 arr_F[13][11] = {"  5us/div\0"," 10us/div\0"," 20us/div\0"," 50us/div\0","100us/div\0","200us/div\0","500us/div\0","  1ms/div\0","  2ms/div\0","  5ms/div\0"," 10ms/div\0"," 20ms/div\0"," 50ms/div\0"};
static u8 arr_V[6][11] = {"100mV/div\0","200mV/div\0","500mV/div\0","   1V/div\0","   2V/div\0","   5V/div\0"};

//u8 arr_V[10] = "100mV/div";
//u8 arr_f[10] = "  5us/div";
u8 arr_move[8] = " mov_ver";
				
void set_io0(void)					  										
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);	
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
}

void set_io1(void)					  										
{
	GPIO_SetBits(GPIOA,GPIO_Pin_3);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);	
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);     
}

void set_io2(void)					  										
{
	GPIO_SetBits(GPIOA,GPIO_Pin_3);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);	
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
	GPIO_SetBits(GPIOA,GPIO_Pin_6);     
}

void set_io3(void)					  										
{
	GPIO_SetBits(GPIOA,GPIO_Pin_3);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);	

	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	GPIO_SetBits(GPIOA,GPIO_Pin_5);			   //  GPIO_SetBits
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);     	   //GPIO_ResetBits
}

void set_io4(void)					  										
{
	GPIO_SetBits(GPIOA,GPIO_Pin_3);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);	

	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	GPIO_SetBits(GPIOA,GPIO_Pin_5);			   //  GPIO_SetBits
	GPIO_SetBits(GPIOA,GPIO_Pin_6);     	   //GPIO_ResetBits
}

void set_io5(void)					  										
{
	GPIO_SetBits(GPIOA,GPIO_Pin_3);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);	

	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);			   //  GPIO_SetBits
	GPIO_SetBits(GPIOA,GPIO_Pin_6);     	   //GPIO_ResetBits
}

void set_io6(void)					  										
{
	GPIO_SetBits(GPIOA,GPIO_Pin_3);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);	

	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	GPIO_SetBits(GPIOA,GPIO_Pin_5);			   //  GPIO_SetBits
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);     	   //GPIO_ResetBits
}

void set_io7(void)					  										
{
	GPIO_SetBits(GPIOA,GPIO_Pin_3);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);	

	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	GPIO_SetBits(GPIOA,GPIO_Pin_5);			   //  GPIO_SetBits
	GPIO_SetBits(GPIOA,GPIO_Pin_6);     	   //GPIO_ResetBits
}

void set_io8(void)					  										
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);	

	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);			   //  GPIO_SetBits
	GPIO_SetBits(GPIOA,GPIO_Pin_6);     	   //GPIO_ResetBits
}

void set_io9(void)					  										
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);	

	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	GPIO_SetBits(GPIOA,GPIO_Pin_5);			   //  GPIO_SetBits
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);     	   //GPIO_ResetBits
}

void set_io10(void)					  										
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);
	GPIO_SetBits(GPIOA,GPIO_Pin_7);	

	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	GPIO_SetBits(GPIOA,GPIO_Pin_5);			   //  GPIO_SetBits
	GPIO_SetBits(GPIOA,GPIO_Pin_6);     	   //GPIO_ResetBits
}

void set_io11(void)					  										
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);	

	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);			   //  GPIO_SetBits
	GPIO_SetBits(GPIOA,GPIO_Pin_6);     	   //GPIO_ResetBits
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
	for(index_hang = 0;index_hang< 300;index_hang = index_hang + 30)
	{
		for(index_y = 0;index_y<200;index_y = index_y +5)
		{
			lcd_huadian(index_hang,index_y,POINT_COLOR);	
		}
	}	
	for(index_hang = 0;index_hang<200;index_hang = index_hang + 25)
	{
		for(index_y = 0;index_y<300;index_y = index_y +5)
		{
			lcd_huadian(index_y,index_hang,POINT_COLOR);	
		}
	}
	POINT_COLOR = BLUE;
	LCD_DrawRectangle(0,0,300,200,POINT_COLOR);
	GUI_Line(0,100,300,100,POINT_COLOR);
	GUI_Line(150,0,150,200,POINT_COLOR);
	POINT_COLOR=RED;
}

void set_background(void)
{
	POINT_COLOR = BLUE;
  TFT_ClearScreen(BLACK);
	
	LCD_DrawRectangle(0,0,300,200,POINT_COLOR);
	GUI_Line(0,100,300,100,POINT_COLOR);
	GUI_Line(150,0,150,200,POINT_COLOR);

	POINT_COLOR=RED;
	LCD_DrawRectangle(302,0,398,200,WHITE);
	GUI_Show12ASCII(312,5,"V(V):",POINT_COLOR,BLACK);
	//GUI_Show12ASCII(274,210,"Vmax:",POINT_COLOR,YELLOW);
	//GUI_Show12ASCII(274,210,"Vmin:",POINT_COLOR,YELLOW);
	GUI_Show12ASCII(312,45,"Vpp(V):",POINT_COLOR,BLACK);
	GUI_Show12ASCII(312,95,"Freq(Hz):",POINT_COLOR,BLACK);
	GUI_Show12ASCII(312,145,"duty(%):",POINT_COLOR,BLACK);


	POINT_COLOR=BLUE;
	LCD_DrawRectangle(0,202,398,236,GREEN);
	GUI_Box(2,205,76,230,YELLOW);
	GUI_Box(92,205,166,230,YELLOW);
	GUI_Box(182,205,256,230,YELLOW);
	GUI_Box(272,205,346,230,YELLOW);
	LCD_DrawRectangle(2,205,76,230,RED);
	GUI_Show12ASCII(4,210,arr_V[0],POINT_COLOR,YELLOW);	
	GUI_Show12ASCII(94,210,arr_F[0],POINT_COLOR,YELLOW);
	GUI_Show12ASCII(184,210,arr_move,POINT_COLOR,YELLOW);
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
}

void EXTI0_IRQHandler(void)
{
	u16 yan_se1;
	delay_ms(10);
	yan_se1 = POINT_COLOR;
	POINT_COLOR=RED;
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
	{
		mode++;
		led0=0;
		if(mode == 4)mode = 0;
		if(mode == 0)
		{
			//GUI_Show12ASCII(260,224,"f",POINT_COLOR,WHITE);
			LCD_DrawRectangle(2,205,76,230,RED);
			LCD_DrawRectangle(92,205,166,230,YELLOW);
			LCD_DrawRectangle(182,205,256,230,YELLOW);
			LCD_DrawRectangle(272,205,346,230,YELLOW);
		}
		else if(mode == 1)//GUI_Show12ASCII(260,224,"v",POINT_COLOR,WHITE);
		{
			LCD_DrawRectangle(2,205,76,230,YELLOW);
			LCD_DrawRectangle(92,205,166,230,RED);
			LCD_DrawRectangle(182,205,256,230,YELLOW);
			LCD_DrawRectangle(272,205,346,230,YELLOW);
		}
		else if(mode ==2)
		{
			LCD_DrawRectangle(2,205,76,230,YELLOW);
			LCD_DrawRectangle(92,205,166,230,YELLOW);
			LCD_DrawRectangle(182,205,256,230,RED);
			LCD_DrawRectangle(272,205,346,230,YELLOW);
		}
		else if(mode ==3)
		{
			LCD_DrawRectangle(2,205,76,230,YELLOW);
			LCD_DrawRectangle(92,205,166,230,YELLOW);
			LCD_DrawRectangle(182,205,256,230,YELLOW);
			LCD_DrawRectangle(272,205,346,230,RED);
		}
	}
	
	EXTI_ClearITPendingBit(EXTI_Line0);
	POINT_COLOR = yan_se1;
}	   

void EXTI3_IRQHandler(void)
{
	delay_ms(10);
	led1=0;
	if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)==0)
	{
		if(mode == 0)
		{
			num_fu_du++;
			if(num_fu_du==7)num_fu_du=1;
		}
		else if(mode==1)
		{	
			num_shao_miao++;
			if(num_shao_miao == 14)num_shao_miao = 1;
			
		}
		else if(mode==2)
		{
			
		}
		else if(mode==3)
		{
			
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line3);
}

void EXTI4_IRQHandler(void)
{
	delay_ms(10);
	led2=0;
	if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)==0)
	{
		if(mode == 0)
		{
			num_fu_du--;
			if(num_fu_du==0)num_fu_du=6;
		}
		else if(mode==1)
		{	
			num_shao_miao--;
			if(num_shao_miao == 0)num_shao_miao = 13;
			
		}
		else if(mode==2)
		{
			
		}
		else if(mode==3)
		{
			
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line4);
}

void TIM2_IRQHandler(void)
{
	u16 temple;
	u16 yan_se;
	if(TIM_GetITStatus(TIM2, TIM_IT_Update))
	{
		TIM_Cmd(TIM3,DISABLE);
		TIM_Cmd(TIM2,DISABLE);

   	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		temple = TIM_GetCounter(TIM3);
		frequency = 65536*count+ temple;
		frequency = frequency - frequency*(130.10/1000000);

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
			case 1:shao_miao_shu_du = 5;break;
			case 2:shao_miao_shu_du = 10;break;	
			case 3:shao_miao_shu_du = 20;break;	
			case 4:shao_miao_shu_du = 50;break;		  //分界点
			case 5:shao_miao_shu_du = 100;break;
			case 6:shao_miao_shu_du = 200;break;
			case 7:shao_miao_shu_du = 500;break;
			case 8:shao_miao_shu_du = 1000;break;
			case 9:shao_miao_shu_du = 2000;break;
			case 10:shao_miao_shu_du = 5000;break;
			case 11:shao_miao_shu_du = 10000;break;
			case 12:shao_miao_shu_du = 20000;break;
			case 13:shao_miao_shu_du = 50000;break;
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
			
			case 1:vcc_div=100;set_io11();break;
			case 2:vcc_div=200;set_io10();break;
			case 3:vcc_div=500;set_io7();break;
			case 4:vcc_div=1000;set_io1();break;
			case 5:vcc_div=2000;set_io1();break;
			case 6:vcc_div=5000;set_io1();break;
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
		
		
		yan_se = POINT_COLOR;
		POINT_COLOR=BLUE;
		if(frequency>20000)
		{
			frequency_flag = 1;	
		}
		else
		{
			frequency_flag = 0;			
		}
		
		GUI_Show12ASCII(94,210,arr_F[num_shao_miao-1],POINT_COLOR,YELLOW);
		GUI_Show12ASCII(4,210,arr_V[num_fu_du-1],POINT_COLOR,YELLOW);

		POINT_COLOR=yan_se;

		count = 0;
		TIM_SetCounter(TIM2,0);
		TIM_SetCounter(TIM3,0);

		TIM_Cmd(TIM2,ENABLE);
     	TIM_Cmd(TIM3,ENABLE);
		led0=!led0;	  
	}
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update))
	{
   		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		count++;
		
	}
}
