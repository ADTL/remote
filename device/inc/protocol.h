/*-----------------------------------------------------------------------------
* Author: Boris Vinogradov(nis) : no111u3@gmail.com
* File: protocol.h
* Type: C header file
* Project: f4examples/05-timers/02-ccr-in-comp-mode
* 2012
*-----------------------------------------------------------------------------*/
#define PROTO_COUNT_MLBOX 1

enum ProtoIOStates { P_NULL, P_COMPLETE, P_ERROR };

typedef struct {
    unsigned char header;
    unsigned char address;
    unsigned char size;
    unsigned char * message;
} ProtoMsgIO;

typedef struct {
    ProtoMsgIO * inbox;
    ProtoMsgIO * outbox;
} ProtoMlboxIO;

extern volatile ProtoMlboxIO * a_proto_mlbox_io[PROTO_COUNT_MLBOX];
/* create new mailbox */
unsigned char proto_create_mlbox_io(ProtoMlboxIO * proto_mlbox_io);
/* send message */
// unsigned char proto_send_msg(unsigned char mlbox_num);
/* get message */
//unsigned char proto_get_msg(unsigned char mlbox_num);
