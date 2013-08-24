#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "buttons.h"

volatile ButtonsState button_state;
/* Buttons events */
void buttons_event(void) {
    uint8_t button;
    for (button = 0; button <= B_MOD; button++) {
	if (!(BUT_PORT->IDR & (1 << (button + 2)))) {
	    button_state.counter[button]++;
	}
	else {
	    if (button_state.counter[button] > B_SHORT_CLICK)
		button_state.state[button] = B_CLICK;
	    button_state.counter[button] = 0;
	}
    }
}
/* Buttons setup */
void buttons_setup(void) {
    /* Configure ports */
    GPIO_InitTypeDef GPIO_InitStructure;
    /* Enable GPIOE clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    /* Setup button pins to input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |
				  GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(BUT_PORT, &GPIO_InitStructure);
    /* Configure interrupt */
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Enable the TIM1 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    /* Configure timer */
    TIM_TimeBaseInitTypeDef base_timer;
    /* TIM6 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    TIM_TimeBaseStructInit(&base_timer);
    base_timer.TIM_Prescaler = 16000 - 1;
    base_timer.TIM_Period = 10 - 1;
    TIM_TimeBaseInit(TIM6, &base_timer);
    
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM6, ENABLE);
}
/* Button event timer */
void tim6_dac_handler(void) {
    if (TIM_GetITStatus(TIM6, TIM_IT_Update)) {
	buttons_event();
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
}
