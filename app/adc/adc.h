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
extern u8 multiple;

void ADC1_Init(void);
void ADC_Get_Value(void);
u16 ADC_Get_Vpp(void);
void clear(void);
void ADC_print(int ver, int hor);
void clear_inter(int inter);
void pause_plot(u16 ver, u16 hor);

#endif
