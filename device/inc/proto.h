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
#define PROTO_IO_MBOX_NOTHING 0
#define PROTO_IO_MBOX_READY 1
#define PROTO_IO_MBOX_SEND 2
#define PROTO_IO_MBOX_COMPLETE 3
#define PROTO_IO_MBOX_ERROR 4
/* status state machine */
#define PROTO_SRV_STAT_READY 0
#define PROTO_SRV_STAT_HEADER 1
#define PROTO_SRV_STAT_ADDRESS 2
#define PROTO_SRV_STAT_SIZE 3
#define PROTO_SRV_STAT_MESSAGE 4
#define PROTO_SRV_STAT_COMPLETE 5
#define PROTO_SRV_STAT_ERROR 6
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
/* proto send state machine */
void proto_send_sm(void);
/* proto usart1 Tx handler */
void usart1_handler(void);