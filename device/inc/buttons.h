/*-----------------------------------------------------------------------------
* Author: Boris Vinogradov(nis) : no111u3@gmail.com
* File: buttons.h
* Type: C header file
* Project: f4examples/05-timers/02-ccr-in-comp-mode
* 2012
*-----------------------------------------------------------------------------*/
#define B_CLICK (unsigned char)'C' /* state if button pressed */
#define B_RELEASE (unsigned char)'R' /* state if button released */
#define B_SHORT_CLICK 1 /* couter value for short click */

#define BUT_PORT GPIOA

enum ButtonTypes { B_LGHT, B_SET, B_UP, B_DWN, B_SU, B_MOD };

typedef struct {
    unsigned char counter[6];
    unsigned char state[6];
} ButtonsState;

/* extern buttons structure */
extern volatile ButtonsState button_state;
/* Buttons setup */
void buttons_setup(void);