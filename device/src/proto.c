/*-----------------------------------------------------------------------------
* Author: Boris Vinogradov(nis) : no111u3@gmail.com
* File: work.c
* Type: C code file
* Project: STM32F4D
* 2013
*-----------------------------------------------------------------------------*/ 
#include "proto.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "usart.h"
/* main state struct */
volatile ProtoSrvDat proto_srv_dat;
/* service stack vars */
char proto_service_in_b[16],
    proto_service_out_b[16],
    proto_system_in_b[16],
    proto_system_out_b[16],
    proto_common_in_b[64],
    proto_common_out_b[64];
ProtoIOMsg proto_service_in_m,
    proto_service_out_m,
    proto_system_in_m,
    proto_system_out_m,
    proto_common_in_m,
    proto_common_out_m;
ProtoIOMBox proto_service_mb,
    proto_system_mb,
    proto_common_mb;
ProtoSrvStatus proto_s_inbox,
    proto_s_outbox;
/* proto setup */
void proto_setup(void) {
    /* create basics mail boxes */
    proto_service_in_m.message = proto_service_in_b;
    proto_service_out_m.message = proto_service_out_b;
    proto_system_in_m.message = proto_system_in_b;
    proto_system_out_m.message = proto_system_out_b;
    proto_common_in_m.message = proto_common_in_b;
    proto_common_out_m.message = proto_common_out_b;
    
    proto_service_mb.inbox = &proto_service_in_m;
    proto_service_mb.outbox = &proto_service_out_m;
    proto_system_mb.inbox = &proto_system_in_m;
    proto_system_mb.outbox = &proto_system_out_m;
    proto_common_mb.inbox = &proto_common_in_m;
    proto_common_mb.outbox = &proto_common_out_m;
    /* append basics mail boxes */
    proto_srv_dat.mailboxes[PROTO_MBOX_ADR_SERVICE] = &proto_service_mb;
    proto_srv_dat.mailboxes[PROTO_MBOX_ADR_SYSTEM] = &proto_system_mb;
    proto_srv_dat.mailboxes[PROTO_MBOX_ADR_COMMON] = &proto_common_mb;
    proto_srv_dat.s_inbox = &proto_s_inbox;
    proto_srv_dat.s_outbox = &proto_s_outbox;
}
/* proto usart1 Tx handler */
void usart1_handler(void) {
    if (USART_GetITStatus(USART1, USART_IT_TC) != RESET) {
        proto_send_sm();
        USART_ClearITPendingBit(USART1, USART_IT_TC);
    }
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        proto_get_sm();
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
/* proto send message */
void proto_send_msg(unsigned char mbox_num) {
    proto_srv_dat.s_outbox->address = mbox_num;
    proto_srv_dat.s_outbox->status = PROTO_SRV_STAT_READY;
    proto_send_sm();
}
/* proto get message */
void proto_get_msg(unsigned char mbox_num) {
    proto_srv_dat.s_inbox->address = mbox_num;
    proto_srv_dat.s_inbox->status = PROTO_SRV_STAT_READY;
    proto_get_sm();
}
/* proto send state machine */
void proto_send_sm(void) {
    ProtoSrvStatus * outbox = proto_srv_dat.s_outbox;
    ProtoIOMBox * mbox = proto_srv_dat.mailboxes[outbox->address];
    
    switch(outbox->status) {
        case PROTO_SRV_STAT_READY:
            outbox->header = mbox->outbox->header;
            if (mbox->outbox_s != PROTO_IO_MBOX_READY) {
                outbox->status = PROTO_SRV_STAT_ERROR;
                proto_send_sm();
                break;
            }
            if (outbox->header != 'Q' && outbox->header != 'A' && outbox->header != 'C') {
                mbox->outbox_s = PROTO_IO_MBOX_ERROR;
                proto_send_sm();
                break;
            }
            mbox->outbox_s = PROTO_IO_MBOX_SEND;
            GPIOD->ODR |= GPIO_Pin_14;
            outbox->status = PROTO_SRV_STAT_HEADER;
        case PROTO_SRV_STAT_HEADER:
            ser1_sendb(outbox->header);
            outbox->status = PROTO_SRV_STAT_ADDRESS;
            break;
        case PROTO_SRV_STAT_ADDRESS:
            ser1_sendb(outbox->address);
            outbox->status = PROTO_SRV_STAT_SIZE;
            break;
        case PROTO_SRV_STAT_SIZE:
            outbox->size = mbox->outbox->size;
            ser1_sendb(outbox->size);
            if (outbox->size > 0) {
                outbox->counter = 0;
                outbox->status = PROTO_SRV_STAT_MESSAGE;
            }
            else
                outbox->status = PROTO_SRV_STAT_COMPLETE;
            break;
        case PROTO_SRV_STAT_MESSAGE:
            ser1_sendb(mbox->outbox->message[outbox->counter++]);
            if (outbox->counter >= outbox->size) {
                outbox->status = PROTO_SRV_STAT_COMPLETE;
            }
            break;
        case PROTO_SRV_STAT_COMPLETE:
            outbox->status = PROTO_SRV_STAT_READY;
            mbox->outbox_s = PROTO_IO_MBOX_COMPLETE;
            GPIOD->ODR &= ~GPIO_Pin_14;
            break;
        case PROTO_SRV_STAT_ERROR:
            mbox->outbox_s = PROTO_IO_MBOX_ERROR;
            outbox->header = 'N';
            outbox->status = PROTO_SRV_STAT_READY;
            GPIOD->ODR &= ~GPIO_Pin_14;
            break;
        default:
            break;
    }
}
/* proto get state machine */
void proto_get_sm(void) {
    ProtoSrvStatus * inbox = proto_srv_dat.s_inbox;
    ProtoIOMBox * mbox = proto_srv_dat.mailboxes[inbox->address];
    switch(inbox->status) {
        case PROTO_SRV_STAT_READY:
            mbox->inbox_s = PROTO_IO_MBOX_SEND;
            GPIOD->ODR |= GPIO_Pin_12;
            inbox->status = PROTO_SRV_STAT_HEADER;
        case PROTO_SRV_STAT_HEADER:
            inbox->header = ser1_getb();
            if (inbox->header != 'Q' && inbox->header != 'A' && inbox->header != 'C') {
                inbox->status = PROTO_SRV_STAT_ERROR_ADDR;
                proto_get_sm();
            }
            else
                inbox->status = PROTO_SRV_STAT_ADDRESS;
            break;
        case PROTO_SRV_STAT_ADDRESS:
            inbox->address = ser1_getb();
            if (inbox->address >= PROTO_MBOX_NUM) {
                inbox->status = PROTO_SRV_STAT_ERROR_ADDR;
                proto_get_sm();
            }
            else
                inbox->status = PROTO_SRV_STAT_SIZE;
            break;
        case PROTO_SRV_STAT_SIZE:
            inbox->size = ser1_getb();
            if (inbox->size > 0) {
                if (inbox->size > mbox->inbox->size) {
                    inbox->status = PROTO_SRV_STAT_ERROR;
                    proto_get_sm();
                }
                inbox->counter = 0;
                inbox->status = PROTO_SRV_STAT_MESSAGE;
            }
            else {
                inbox->status = PROTO_SRV_STAT_COMPLETE;
                proto_get_sm();
            }
            break;
        case PROTO_SRV_STAT_MESSAGE:
            mbox->inbox->message[inbox->counter++] = ser1_getb();
            if (inbox->counter >= inbox->size) {
                inbox->status = PROTO_SRV_STAT_COMPLETE;
                proto_get_sm();
            }
            break;
        case PROTO_SRV_STAT_COMPLETE:
            inbox->status = PROTO_SRV_STAT_READY;
            mbox->inbox->header = inbox->header;
            mbox->inbox->size = inbox->size;
            mbox->inbox_s = PROTO_IO_MBOX_COMPLETE;
            GPIOD->ODR &= ~GPIO_Pin_12;
            break;
        case PROTO_SRV_STAT_ERROR:
            mbox->inbox_s = PROTO_IO_MBOX_ERROR;
        case PROTO_SRV_STAT_ERROR_ADDR:
            inbox->header = 'N';
            inbox->status = PROTO_SRV_STAT_READY;
            GPIOD->ODR &= ~GPIO_Pin_12;
            break;
        default:
            break;
    }
}