/*-----------------------------------------------------------------------------
* Author: Boris Vinogradov(nis) : no111u3@gmail.com
* File: work.c
* Type: C code file
* Project: STM32F4D
* 2013
*-----------------------------------------------------------------------------*/ 
#include "misc.h"
#include "proto.h"
#include "stm32f4xx.h"
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
/* proto setup */
void proto_setup(void) {
    NVIC_InitTypeDef NVIC_InitStructure;
    /* create basics mail boxes */
    proto_service_in_m.message = &proto_service_in_b;
    proto_service_out_m.message = &proto_service_out_b;
    proto_system_in_m.message = &proto_system_in_b;
    proto_system_out_m.message = &proto_system_out_b;
    proto_common_in_m.message = &proto_common_in_b;
    proto_common_out_m.message = &proto_common_out_b;
    
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
    /* enable usart interrupts */
    /*NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);*/
}
/* proto usart1 Tx handler */
void usart1_handler(void) {
}
/* proto send message */
void proto_send_msg(unsigned char mbox_num) {
    ProtoIOMBox * mbox = proto_srv_dat.mailboxes[mbox_num];
    unsigned char byte_cnt;
    if (mbox->outbox_s == PROTO_IO_MBOX_READY) {
        if (mbox->outbox->size == 0 && mbox->outbox->header != 'C')
            mbox->outbox_s = PROTO_IO_MBOX_ERROR;
        else if (mbox->outbox->header != 'Q' && mbox->outbox->header != 'A' && mbox->outbox->header != 'C')
            mbox->outbox_s = PROTO_IO_MBOX_ERROR;
        else {
            mbox->outbox_s = PROTO_IO_MBOX_SEND;
            proto_srv_dat.outbox.header = mbox->outbox->header;
            proto_srv_dat.outbox.address = mbox_num;
            proto_srv_dat.outbox.status = PROTO_SRV_STAT_READY;
            proto_send_sm();
            proto_send_sm();
            proto_send_sm();
            for (byte_cnt = 0; byte_cnt++ < mbox->outbox->size; ) {
                ser1_sendb(mbox->outbox->message[byte_cnt]);
            }
            mbox->outbox_s = PROTO_IO_MBOX_COMPLETE;
        }
    }
    else {
        mbox->outbox_s = PROTO_IO_MBOX_ERROR;
    }
}
/* proto send state machine */
void proto_send_sm(void) {
    switch(proto_srv_dat.outbox.status) {
        case PROTO_SRV_STAT_READY:
            ser1_sendb(proto_srv_dat.outbox.header);
            proto_srv_dat.outbox.status = PROTO_SRV_STAT_HEADER;
            break;
        case PROTO_SRV_STAT_HEADER:
            ser1_sendb(proto_srv_dat.outbox.address);
            proto_srv_dat.outbox.status = PROTO_SRV_STAT_ADDRESS;
            break;
        case PROTO_SRV_STAT_ADDRESS:
            ser1_sendb(proto_srv_dat.mailboxes[proto_srv_dat.outbox.address]->outbox->size);
            break;
        default:
            break;
    }
}
