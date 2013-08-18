/*-----------------------------------------------------------------------------
* Author: Boris Vinogradov(nis) : no111u3@gmail.com
* File: work.c
* Type: C code file
* Project: STM32F4D
* 2013
*-----------------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "protocol.h"
#include "usart.h"

volatile ProtoMlboxIO * a_proto_mlbox_io[PROTO_COUNT_MLBOX];
/* create new mailbox */
unsigned char proto_create_mlbox_io(ProtoMlboxIO * proto_mlbox_io) {
    a_proto_mlbox_io[0] = proto_mlbox_io;
    return 0;
}
/* send message */
/*unsigned char proto_send_msg(unsigned char mlbox_num) {
    ProtoMsgIO * outbox = a_proto_mlbox_io[mlbox_num]->outbox;
    unsigned char byte_cnt;
    if (outbox->size == 0 && outbox->header != 'C')
        return P_ERROR;
    if (outbox->header == 'Q' || outbox->header == 'A' || outbox->header == 'C') {
        ser1_sendb(outbox->header);
        ser1_sendb(outbox->address);
        ser1_sendb(outbox->size);
        for (byte_cnt = 1; byte_cnt++ <= outbox->size; ) {
            ser1_sendb(outbox->message[byte_cnt]);
        }
        return P_COMPLETE;
    }
    else {
        return P_ERROR;
    }
}*/
/* get message */
unsigned char proto_get_msg(unsigned char mlbox_num) {
    ProtoMsgIO * inbox = a_proto_mlbox_io[mlbox_num]->inbox;
    unsigned char byte_cnt, raw_data;
    raw_data = ser1_getb();
    if (raw_data == 'Q' || raw_data == 'A' || raw_data == 'C') {
        inbox->header = raw_data;
        inbox->address = ser1_getb();
        inbox->size = ser1_getb();
        if (inbox->size == 0 && inbox->header != 'C')
            return P_ERROR;
        for (byte_cnt = 1; byte_cnt++ <= inbox->size; ) {
            inbox->message[byte_cnt] = ser1_getb();
        }
        return P_COMPLETE;
    }
    else {
        return P_ERROR;
    }
}