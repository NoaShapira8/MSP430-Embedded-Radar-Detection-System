#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer



void LDR_1_Meas (void) {
  int k=0 ;
  ADC10_on() ;                           // turning on the ADC for meas
  __bis_SR_register(LPM0_bits + GIE);   // wait for ADC
  unsigned int res1 = ADC10MEM ;        // put ADC val to res1
  ADC_off() ;
  ADC_Switch_P1_3() ;
  ADC10_on () ;
  __bis_SR_register(LPM0_bits + GIE);   // wait for ADC
  unsigned int  res2 = ADC10MEM ;       // put ADC val to res1
  LDR_res = (res1 + res2 ) /2  ;
  ADC_off() ;
  ADC10CTL1 &= ~INCH_3 ;

  //sending data to python
  for (k = 0; k < 12; k++) {
      str_LDR[k] = '\0';
  }
  TimerA0_Config_Delay(0x7FFF) ;
  start_Timer_A0() ;

}

//---------------------------------------------------------------------
//            Converting int diff to String diff (enable sending to PC)
//---------------------------------------------------------------------

void itoa(int n, char s[], int angle) {        // change from integer to string
    int i, l, sign ;
    char a[4] = '\0' ;

    if ((sign = n) < 0) {
        n = -n + 0xFFFF ;
    }
    if ((sign = angle) < 0) {
        angle = -angle ;
    }
    i = 0 ;
    do  {
        s[i++] = n % 10 + '0' ;
    }
    while ((n /= 10) > 0) ;
    l = 0 ;
    do {
        a[l++] = angle % 10 + '0' ;
    }
    while ((angle /= 10) > 0) ;

    reverse(s);
    reverse(a);

    s[i++] = ',' ;
    strcat(s, a) ;
    i = l + i ;
    s[i++] = '\r' ;
    s[i++] = '\n' ;
    s[i] = '\0';
}

void reverse(char s[]) {           //reverse the string he gets.
    int i, j ;
    char c ;

    for (i = 0, j = strlen(s) - 1; i < j && strlen(s) > 1; i++, j--) {
        if (s[i] == 0x0a || s[i] == 0x0d) {
            continue ;
        }
        c = s[i] ;
        s[i] = s[j] ;
        s[j] = c ;
    }
}

//---------------------------------------------------------------------
//            Script Mode
//---------------------------------------------------------------------
void script_mode(int load_mode, int play_mode, int read_script, int ShouldWrite){
    if (play_mode == 1){          // read from flash memory
        switch (read_script){
        case 1:
            read_ptr = first_script_ptr;
            break;
        case 2:
            read_ptr = second_script_ptr;
            break;
        case 3:
            read_ptr = third_script_ptr;
            break;
        }
        Read_Flash(read_ptr);
    }
    else if (load_mode == 1){ // write to flash memory
        if (ShouldWrite == 1){
            Write_Flash(cmd, script_num);
            if (script_num == 1) {
                script_num = 2 ;
            }
            if (script_num == 2) {
                script_num = 3 ;
            }
            if (script_num == 2) {
                script_num = 3 ;
            }
        }
    }
}


//---------------------------------------------------------------------
//            Script Mode Functions
//---------------------------------------------------------------------
void count_up(int x){
    //making Counter up until reaching int x with delay d
    while(counterUp <= x) {
        lcd_clear();
        lcd_home();
        counterUp = counterUp + 1 ;
        Int2Str(CountUp_Str , counterUp) ;
        if (counterUp < 10){
            lcd_puts(CountUp_Str) ;
        }

        else if (counterUp < 100){
            lcd_puts(CountUp_Str) ;
        }

        else if (counterUp < 1000){
             lcd_puts(CountUp_Str) ;
        }

        else if (counterUp < 10000){
             lcd_puts(CountUp_Str) ;
        }

        else {
             lcd_puts(CountUp_Str) ;
        }
        Get_d_Delay() ;
    }
}

void count_down(int x){
    //making Counter from x until 0 with delay d
    counterDown = x + 1 ;
    while(counterDown > 0) {
        lcd_clear();
        lcd_home();
        counterDown = counterDown - 1 ;
        Int2Str(CountUp_Str , counterDown) ;
        lcd_puts(CountUp_Str) ;
        Get_d_Delay();
    }
}


void rotate_char(const char x){
    // rotate right char x onto lcd from pixel 0 to pixel index 31(ascii val) with delay d
    //unsigned int z = 0;
    lcd_clear();
    lcd_home();
    while(z < 31) {
        if(first_round == 1){
            //lcd_puts(x) ;
            lcd_data(x) ;
            first_round = 0;
        }
        else{
            lcd_clear();
            lcd_cursor_right();
            lcd_puts(x) ;
        }
        z ++;
        Get_d_Delay();
    }
    z = 0 ;
}

void set_d_delay(int delay){
    d = 10 * delay ;  //units of 10ms
}

void clear_lcd(void){
    lcd_clear();
}

void servo_deg(int p){
    // Point the Ultrasonic sensor to degree p and show the degree and distance (dynamically) onto pc screen
}

void servo_scan(int l, int r){
    //scan area between left l angke to right r angle (once) and show the degree and distance (dynamically) onto pc screen
}



 
  

