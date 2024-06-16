#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer
#include     <string.h>
#include    <stdio.h>

int state ,T_on_delay, PWM_status, x;
unsigned int RX_index = 0 , Menu_index=0 ,angle ,more_data_flag=0 ; 
enum SYSmode lpm_mode;
char T_on[] ;
int LDR_res,samp, first_round=1, sand_samp_flag = 0 ;
int flag_1 =0, flag_3=0, flag_4=0, flag_6=0;
int timer_state = 1 ;
unsigned int delay_half_sec = 0xFFFF ;

//variable for UltraSonic Sensor
volatile int temp[2], diff ;
unsigned int i = 0 ;
char str_diff[13];
int cap_flag , j=0 , cci_flag;
int test[60] , t=0;

//variable for Servo motor
int current_angle = 0 ,servo_index = 0 , DC_PWM = 0x0276;
int t_on_tel ;

//variable for LDR Functions
int LDR_dist =0 , send_flag = 0 ;
char str_LDR[13];



//variable for Script
int play_mode = 0 ,load_mode =  0 ;
int ShouldWrite = 0, script_cmd_done = 0;   // '0' = Disable write to flash,  '1' = Enable Writing to flash
char cmd[100];
int read_script = 0;   // Choosing which script to read between [1-3]
unsigned int r_index = 0;  //index for writing chars to cmd array
int script_num = 1;   //choosing which script is Writing to Flash
int opc, op1, op2;

char *Flash_ptr = (char*)0x0FC00;              // Initialize Flash pointer
char *first_script_ptr = (char*)0x0FC00;;      //pointer to the first script
char *second_script_ptr = (char*)0x0FC00;;     //pointer to the first script
char *third_script_ptr = (char*)0x0FC00;;      //pointer to the first script
char *read_ptr;

//variable for LCD Functions
int d = half_sec_delay, z=0 ;  //The default delay d value
unsigned int counterUp = 0, counterDown = 0;
const char CountUp_Str[5];

void main(void){
  
  state = 8;
  IE2 &= ~UCA0TXIE;     // Disable USCI_A0 TX interrupt
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();          // Config all and go to angle 0 
  lcd_init() ;
  lcd_clear() ;
  
   while(1){
    switch(state){
      
    case 0:
      //__bis_SR_register(LPM0_bits + GIE);
      break;

    case 1:
        if (flag_1 == 0) {
            TimerA0_Config_0_deg(DC_PWM) ;
            timer_state = 0 ;
            start_Timer_A0() ;
            flag_1 = 1;
        }
        else {
            __bis_SR_register(LPM0_bits + GIE) ;
        }
      break;
      
    case 2:
      //__bis_SR_register(LPM0_bits + GIE);       //Waiting for angle
      break;
      
    case 3:
        if (flag_3 == 0) {
            TimerA0_Config_0_deg(DC_PWM) ;
            timer_state = 0 ;
            start_Timer_A0() ;
            flag_3 = 1;
        }
        else {
            __bis_SR_register(LPM0_bits + GIE) ;
        }

      break;
      
    case 4:
        if (flag_4 == 0){
            //Config_Timer_for_Servo_2(3277,78) ;
            Timer_A0_on() ;
            flag_4 = 1;
        }
      break;
      
    case 5:
        if(load_mode == 1 || play_mode == 1 ){
            if( ShouldWrite == 1 || read_script == 1 || read_script == 2 || read_script == 3) {
                __no_operation();
                script_mode(load_mode, play_mode, read_script, ShouldWrite);
                IE2 |= UCA0RXIE ;
                __bis_SR_register(LPM0_bits + GIE);   // Enter LPM0
            }
        }
      break;
      
    case 6:
        __bis_SR_register(LPM0_bits + GIE);
        LDR_1_Meas() ;
        flag_6 = 1 ;
      break;
      
    case 7:
      break;

    case 8 :                                // Waiting for new state   
      GPIOconfig();
      IE2 &= ~UCA0TXIE;
      IFG2 &= ~UCA0TXIFG;
      IE2 |= UCA0RXIE;                      // Enable USCI_A0 RX interrupt
      //if (flag_6 == 1) {
        //  IFG2 |= UCA0RXIFG ;
      //}
      more_data_flag =0 ;                   // Nothing else to receive 
      __bis_SR_register(LPM0_bits + GIE);   // Enter LPM0
      __no_operation();
      break ; 

    default:
      state = 0 ;
    }
    //_BIS_SR(LPM0_bits + GIE); // LPM0, enable interrupts
  }
}





