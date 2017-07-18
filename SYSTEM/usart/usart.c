#include "usart.h"	  

#ifdef USART3_PRINTF
int fputc(int ch,FILE *p)  //函数默认的，在使用printf函数时自动调用
{
	USART_SendData(USART3,(u8)ch);	
	while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
	return ch;
}
#endif