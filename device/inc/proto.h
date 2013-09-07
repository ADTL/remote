/*-----------------------------------------------------------------------------
* Author: Boris Vinogradov(nis) : no111u3@gmail.com
* File: proto.h
* Type: C header file
* Project: f4examples/05-timers/02-ccr-in-comp-mode
* 2012
*-----------------------------------------------------------------------------*/
/* mailboxes */
#define PROTO_SERVICE_MBOX_NUM 3
#define PROTO_MBOX_NUM PROTO_SERVICE_MBOX_NUM + 0
/* header of message */
#define PROTO_IO_MSG_HEADER_COMMAND 'C'
#define PROTO_IO_MSG_HEADER_QUEST 'Q'
#define PROTO_IO_MSG_HEADER_ANSVER 'A'
/* status message */
enum PROTO_IO_MBOX {
    PROTO_IO_MBOX_NOTHING,
    PROTO_IO_MBOX_READY,
    PROTO_IO_MBOX_SEND,
    PROTO_IO_MBOX_COMPLETE,
    PROTO_IO_MBOX_ERROR
};
/* status state machine */
enum PROTO_SRV_STAT {
    PROTO_SRV_STAT_READY,
    PROTO_SRV_STAT_HEADER,
    PROTO_SRV_STAT_ADDRESS,
    PROTO_SRV_STAT_SIZE,
    PROTO_SRV_STAT_MESSAGE,
    PROTO_SRV_STAT_COMPLETE,
    PROTO_SRV_STAT_ERROR,
    PROTO_SRV_STAT_ERROR_ADDR
};
/* addresses for system needed */
#define PROTO_MBOX_ADR_SERVICE 0
#define PROTO_MBOX_ADR_SYSTEM 1
#define PROTO_MBOX_ADR_COMMON 2
/* message */
typedef struct {
    unsigned char header;
    unsigned char size;
    char * message;
} ProtoIOMsg;
/* mail box */
typedef struct {
    ProtoIOMsg * inbox;
    ProtoIOMsg * outbox;
    unsigned char inbox_s;
    unsigned char outbox_s;
} ProtoIOMBox;
/* internal(service) state machine regs */
typedef struct {
    unsigned char header;
    unsigned char address;
    unsigned char counter;
    unsigned char size;
    unsigned char status;
} ProtoSrvStatus;
/* internal(service) state machine stack */
typedef struct {
    ProtoIOMBox * mailboxes[PROTO_MBOX_NUM];
    ProtoSrvStatus * s_inbox;
    ProtoSrvStatus * s_outbox;
} ProtoSrvDat;
/* main state struct */
extern volatile ProtoSrvDat proto_srv_dat;
/* proto setup */
void proto_setup(void);
/* proto send message */
void proto_send_msg(unsigned char mbox_num);
/* proto get message */
void proto_get_msg(unsigned char mbox_num);
/* proto send state machine */
void proto_send_sm(void);
/* proto get state machine */
void proto_get_sm(void);
/* proto usart1 Tx handler */
void usart1_handler(void);