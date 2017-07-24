
#ifndef __STM32F10x_IT_H
#define __STM32F10x_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"


extern u8 frequency_flag;
extern long int shao_miao_shu_du;
extern u8 num_shao_miao;
extern u8 mode1;
extern u8 mode2;
extern u8 num_fu_du;
extern u8 ad_flag;
extern u16 vpp;
extern float gao_pin_palus;
extern u16 vcc_div;
extern u16 interval;
extern int flag;
extern int flag_mode;
extern float arr_plot[250];
extern int inter;
extern int gain;
extern u8 C_dc_ac;
extern u8 change_gain;
	 
extern u8 arr_F[13][11];
extern u8 arr_V[7][11];
extern u8 arr_gain[8][6];
extern u8 arr_JDQ[2][3];
extern float gain_multiple0[8];
extern float gain_multiple1[8];
extern u8 show_gain0[8][8];
extern u8 show_gain1[8][8];
//extern u8 arr_move[2][10];


void lcd_huadian(u16 a,u16 b,u16 color);
void lcd_huaxian(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);
void hua_wang(void);
void set_background(void);
void key_init(void);
void set_io0(void);
void set_io1(void);
void set_io2(void);
void set_io3(void);
void set_io4(void);
void set_io5(void);
void set_io6(void);
void set_io7(void);
void set_io8(void);
void set_io9(void);
void set_io10(void);
void set_io11(void);
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);


#ifdef __cplusplus
}
#endif

#endif /* __STM32F10x_IT_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
