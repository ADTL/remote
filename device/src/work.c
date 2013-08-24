/*-----------------------------------------------------------------------------
* Author: Boris Vinogradov(nis) : no111u3@gmail.com
* File: work.c
* Type: C code file
* Project: STM32F4D
* 2013
*-----------------------------------------------------------------------------*/
/* includes */
#include "stm32f4xx.h"
#include "usart.h"
#include "status.h"
#include "buttons.h"
#include "lcd.h"
#include "protocol.h"
#include "proto.h"
#include "xprintf.h"
#include "test-img.h"
#include "stm32f4xx_gpio.h"
/* globals */
volatile int status = 0;
/* check status */
void check_status(void) {
    if (GPIOA->IDR & GPIO_Pin_1) {
        GPIOD->ODR &= ~GPIO_Pin_15;
        status = 0;
        LCD_String("BT", 22, 6, 1, BLACK, GLASSY);
    }
    else {
        GPIOD->ODR |= GPIO_Pin_15;
        status = 1;
        LCD_String("BT", 22, 6, 1, BLUE, GLASSY);
    }
}
/* sender */
void sender(char msg) {
    volatile ProtoIOMBox * mbox;
    if (status) {
        mbox = proto_srv_dat.mailboxes[PROTO_MBOX_ADR_COMMON];
        mbox->outbox->header = 'C'; /* Command */
        mbox->outbox->size = 0x01; /* 0 for ping request */
        mbox->outbox->message[0] = msg; /* short message */
        mbox->outbox_s = PROTO_IO_MBOX_READY; /* Box ready */
        proto_send_msg(PROTO_MBOX_ADR_COMMON);
        /* wait to send message */
        while (mbox->outbox_s <= PROTO_IO_MBOX_SEND);
    }
}
/* main work function */
void work(void) {
    ProtoMsgIO inbox, outbox;
    ProtoMlboxIO mailbox;
    unsigned short i, j;
    unsigned char mailbox_num = 0;
    volatile ProtoIOMBox * mbox;
    unsigned char in_message[3];
    /* setup status */
    status_setup();
    /* setup serial console */
    usart1_setup();
    /* setup proto */
    mailbox.inbox = &inbox;
    mailbox.outbox = &outbox;
    mailbox_num = proto_create_mlbox_io(&mailbox);
    proto_setup();
    mbox = proto_srv_dat.mailboxes[mailbox_num];
    /* setup button */
    buttons_setup();
    /* lcd setup */
    LCD_Setup();
    LCD_Clear(BLACK);
    
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {
            LCD_Pixel(j + 10, i + 10, test.data[i * 10 + j]);
        }
    }
    LCD_Window(0, 0, 9, 9);
    LCD_RS_LOW;
    LCD_SELECT;
    for (i = 0; i < 100; i++) {
        LCD_Send(test.data[i] >> 8);
        LCD_Send(test.data[i] & 0x00ff);
    }
    LCD_Cursor(0, 0);
    LCD_DESEL;
    
    /* check status */
    check_status();
    /* send ping */
    inbox.message = in_message;
    mbox->outbox->header = 'C'; /* Command */
    mbox->outbox->size = 0x00; /* 0 for ping request */
    mbox->outbox_s = PROTO_IO_MBOX_READY; /* Box ready */
    /* wait connection estabilished */
    while (status == 0);
    /* send ping message */
    proto_send_msg(mailbox_num);
    /* wait to send message */
    while (mbox->outbox_s <= PROTO_IO_MBOX_SEND);
    if (mbox->outbox_s == PROTO_IO_MBOX_COMPLETE)
        LCD_String("Con", 36, 6, 1, WHITE, GLASSY);
    else
        LCD_String("Un", 36, 6, 1, RED, GLASSY);
    if (proto_get_msg(mailbox_num) == P_COMPLETE)
        LCD_String("OK", 36 + 3 * 7, 6, 1, GREEN, GLASSY);
    else
        LCD_String("ERR", 36 + 3 * 7, 6, 1, RED, GLASSY);
    /* infinity loop */
    while (1) {
        if (button_state.state[B_LGHT] == B_CLICK) {
            sender('+');
            button_state.state[B_LGHT] = B_RELEASE;
        }
        if (button_state.state[B_MOD] == B_CLICK) {
            sender('m');
            button_state.state[B_MOD] = B_RELEASE;
        }
        if (button_state.state[B_SET] == B_CLICK) {
            sender('-');
            button_state.state[B_SET] = B_RELEASE;
        }
        if (button_state.state[B_UP] == B_CLICK) {
            sender('<');
            button_state.state[B_UP] = B_RELEASE;
        }
        if (button_state.state[B_SU] == B_CLICK) {
            sender('p');
            button_state.state[B_SU] = B_RELEASE;
        }
        if (button_state.state[B_DWN] == B_CLICK) {
            sender('>');
            button_state.state[B_DWN] = B_RELEASE;
        }
    }
}