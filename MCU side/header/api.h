#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer

extern void LDR_1_Meas (void) ;
extern void itoa(int n, char s[], int angle) ;
extern void reverse(char s[]) ;
extern void script_mode(int load_mode, int play_mode, int read_script, int ShouldWrite);
extern void count_up(int x);
extern void count_down(int x);
extern void rotate_char(const char x);
extern void set_d_delay(int d);
extern void clear_lcd(void);
extern void servo_deg(int p);
extern void servo_scan(int l, int r);


#endif
