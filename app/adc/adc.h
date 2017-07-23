#ifndef _adc_H
#define _adc_H

#include "sys.h"
#include "delay.h"

extern u16 a[640];
extern u16 index2;
extern u16 index1;
extern u8 flag_change;
extern u8 flag_50us;
extern int inter_b;
extern float multiple;
extern int mode_hard; 
extern u32 max_data;
extern u32 min_data;

void ADC1_Init(void);
void ADC_Get_Value(void);
u16 ADC_Get_Vpp(void);
u16 get_duty(void);
void clear(void);
void ADC_print(int ver, int hor);
void clear_inter(int inter);
void pause_plot(u16 ver, u16 hor);

#endif
