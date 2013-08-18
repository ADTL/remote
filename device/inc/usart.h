/*-----------------------------------------------------------------------------
* Author: Boris Vinogradov(nis) : no111u3@gmail.com
* File: usart.h
* Type: C header file
* Project: STM32F4D
* 2013
*-----------------------------------------------------------------------------*/  
/*USARTx send byte */
void usart_sendb(USART_TypeDef * USART, char byte);
/*USARTx get byte */
char usart_getb(USART_TypeDef * USART);
/*USART1 send byte */
void ser1_sendb(unsigned char byte);
/*USART1 get byte */
unsigned char ser1_getb(void);
/*USART1 setup */
void usart1_setup(void);