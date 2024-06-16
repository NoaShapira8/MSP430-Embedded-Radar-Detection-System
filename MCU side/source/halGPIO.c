 #include  "../header/halGPIO.h"     // private library - HAL layer

const char MENU[] = "\n"
                    "-------------------------MENU-------------------------\n"
                    "1. Objects Detector System\n"
                    "2. Telemeter\n"
                    "3. Light Sources Detector System \n"
                    "4. Light Sources and Objects Detector System \n"
                    "5. LDR 3-digit value [v] onto LCD\n"
                    "6. Script Mode \n"
                    "------------------------------------------------------\r" ;


//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
  GPIOconfig() ; 
  ADC_Config() ;
  //TimerA0_Config_half_sec() ;
  InitTIMER() ;
  //Config_Timer_for_Servo_2(3277,78) ;   // 78 define 0 deg
}

//--------------------------------------------------------------------
//            Polling based Delay function
//---------------------------------------------------------------------
void delay(unsigned int t){  // t[msec]
	volatile unsigned int i;
	
	for(i=t; i>0; i--);
}
//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00) 
	  _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01) 
	  _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02) 
	  _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
	else if(LPM_level == 0x03) 
	  _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04) 
	  _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}

//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}

//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}

//---------------------------------------------------------------------
//            Timer
//---------------------------------------------------------------------
void start_Timer_A1(int state) {
    if (state == 1 || state == 2) {
        TA1CCR0 = 0xF400 ;  //define the period for Trigger (T ~ 60msec)
        TA1CCR1 = 0x0020 ;  //define the DutyCycle of the PWM trigger (T_on ~ 10usec)
        TA1CCTL0 &= ~CCIE ;
        TA1CTL |= MC_1 ;
        TA1CCTL2 |= CCIE ;   // Enable interrupts of capture mode
    }
}


void start_Timer_A0(){
    TA0CTL |= MC_1 ;
    TA0CCTL0 |= CCIE ;
}

void stop_Timer_A0() {
    TA0CTL &= ~MC_1;
    TA0CCTL2 &= ~(CCIE + CCIFG);
}

void stop_Timer_A1() {
    TA1CTL &= ~MC_1;
    TA1CCTL2 &= ~(CCIE + COV + CCIFG);
    //cci_flag = 1;
}

//---------------------------------------------------------------------
//            Servo Motor
//---------------------------------------------------------------------
void start_servo(void) {
    current_angle = 0 ;
    TA0CCR0 = 0x6700 ;  //define the period for Servo (T ~ 25msec)
    TA0CCR1 = 0x0276 ;  //rotate the servo to 0 deg (TA0CCR1_min)
    TA0CCTL0 = 0x00 ;
    TA0CTL |= MC_1 ;
    TA0CCTL0 |= CCIE ;
}
//---------------------------------------------------------------------
//            Int2Str converter
//---------------------------------------------------------------------
void Int2Str(char *str, unsigned int num){

    int strSize = 0;
    long tmp = num, len = 0;

    // Find the size of the intPart by repeatedly dividing by 10
    while(tmp){
        len++;
        tmp /= 10;
    }

    // Print out the numbers in reverse
    int j;
    for(j = len - 1; j >= 0; j--){
        str[j] = (num % 10) + '0';
        num /= 10;
    }
    strSize += len;
    str[strSize] = '\0';
}

//---------------------------------------------------------------------
//            Str2int converter
//---------------------------------------------------------------------
// this function convert string to integers  to use them after arrived as string from the uart

int str2int( char volatile *str){
int i,res = 0;
for (i = 0; str[i] != '\0'; ++i) {
    if (str[i]> '9' || str[i]<'0')
        return -1;
    res = res * 10 + str[i] - '0';
}

return res;
}

//---------------------------------------------------------------------
//            Script Functions (write / read Flash)
//---------------------------------------------------------------------
void Write_Flash(char cmd[], int script_num){
    unsigned int i;
        FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation
        FCTL3 = FWKEY;

        //for (i = 0; cmd[i] != '\n' && cmd[i] != '\0'; i++)
        for (i = 0; cmd[i] != '\0'; i++)
        {
            *Flash_ptr++ = cmd[i];                   // Write value to flash
        }
        if (cmd[i] == '\0')
        {
            *Flash_ptr++ = '\0';
            switch (script_num)
            {
            case 1:
                second_script_ptr = Flash_ptr;
                script_num = 2;
                break;
            case 2:
                third_script_ptr = Flash_ptr;
                script_num = 3;
                break;
            case 3:
                script_num = 1;
                break;
            }
        }
        else
        {
            *Flash_ptr++ = '\n';
        }

        FCTL1 = FWKEY;                            // Clear WRT bit
        FCTL3 = FWKEY + LOCK;
        ShouldWrite = 0;
        load_mode = 0 ;
        script_cmd_done = 0 ;
}


void Read_Flash(char *read_ptr){
    int p;
    volatile int dig1, dig2;
    while (*read_ptr != '\0' && *read_ptr != 90) // loop for all script
    {
        p = 0;
        while (*read_ptr != '\n' && *read_ptr != '\0') // loop for line
        {
            if (p == 0)
            {
                read_ptr++;
                opc = (int) (*read_ptr) - 0x30;
                read_ptr++;
                p = 2;
            }
            else if (p == 2)
            {
                if (read_ptr[0] > 57)
                    dig2 = (int) read_ptr[0] - 55;
                else
                    dig2 = (int) read_ptr[0] - 0x30;
                if (read_ptr[1] > 57)
                    dig1 = (int) read_ptr[1] - 55;
                else
                    dig1 = (int) read_ptr[1] - 0x30;
                op1 = dig2 * 16 + dig1;
                read_ptr++;
                read_ptr++;
                p = 4;
            }
            else
            {
                if (read_ptr[0] > 57)
                    dig2 = (int) read_ptr[0] - 55;
                else
                    dig2 = (int) read_ptr[0] - 0x30;
                if (read_ptr[1] > 57)
                    dig1 = (int) read_ptr[1] - 55;
                else
                    dig1 = (int) read_ptr[1] - 0x30;
                op2 = dig2 * 16 + dig1;
                read_ptr++;
                read_ptr++;
            }
        }
        if (*read_ptr != '\0' )
            read_ptr++;

        if(opc == 1)
        {
            count_up(op1);
        }
        else if(opc == 2)
        {
            count_down(op1);
        }
        else if(opc == 3)
        {
            rotate_char(op1);
        }
        else if(opc == 4)
        {
            set_d_delay(op1);
        }
        else if(opc == 5)
        {
            clear_lcd();
        }
        else if(opc == 6)
        {
            servo_deg(op1);
        }
        else if(opc == 7)
        {
            servo_scan(op1, op2);
        }
        else
        {
            _BIS_SR(LPM0_bits + GIE); // LPM0, enable interrupts
        }
    }
    play_mode = 0 ;
    read_script = 0 ;
    script_cmd_done = 0 ;
}
//---------------------------------------------------------------------
//       Get_d_delay
//---------------------------------------------------------------------
void Get_d_Delay() {
    unsigned int Shalem ;
    unsigned int Res ;
    int f ;
    Shalem = (int) d / half_sec_delay ;
    Res = d % half_sec_delay ;

    for(f = 0 ; f < Shalem ; f++) {
        TimerA0_Config_LCD(half_sec_delay * 131) ;     // 500 * 131 = FFFF (Convert ms to CCR0 val)
        __bis_SR_register(LPM0_bits+GIE);
    }
    if(Res != 0) {
        TimerA0_Config_LCD(Res * 131) ;
    __bis_SR_register(LPM0_bits+GIE);
    }
}

//*********************************************************************
//            Timer1_A2 Interrupt Service Routine
//*********************************************************************
#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer1_A1_ISR (void){
    int k = 0 ;
    temp[i] = TA1CCR2 ;    //sample TAR on rising edge
    i += 1 ;
    TA1CCTL2 &= ~ (CCIFG + COV) ;
    if (i == 2) {          //sample TAR on falling edge
        diff = temp[i-1] - temp[i-2] ;  //Echo high level time
        TA1CCTL2 &= ~ CCIE ;
        TA1CCTL2 &= ~ (CCIFG + COV) ;
        test[t] = diff ;    //just test to check samples
        t += 1 ;            // just test
        i = 0 ;
        for (k = 0; k < 12; k++) {
            str_diff[k] = '\0';
        }
        itoa(diff, str_diff, current_angle);
        IE2 |= UCA0TXIE;     // Enable USCI_A0 TX interrupt
        UCA0TXBUF = str_diff[j++];

        if (timer_state == 3) {                  // when state 1 is on
            TimerA0_Config_Delay(0X3333) ;      //Delay of 0.1 sec between rotating servo

            timer_state = 1 ;
            stop_Timer_A1() ;
            start_Timer_A0() ;
            current_angle += 3 ;
        }
        /*else if (timer_state == 4) {       //when state 2 is on (Telemater)
            //stop_Timer_A1() ;
            timer_state = 5 ;
            TimerA0_Config_Delay(0xFFFF) ;   //delay 0.1sec for refresh data in distance screen in PC
            start_Timer_A0() ;
        }*/
    }
}

//*********************************************************************
//            TimerA0 Interrupt Service Routine
//*********************************************************************
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR (void){
    if( timer_state == 0) {
        if (servo_index == 35) {
            servo_index = 0 ;
            timer_state = 1;
            TimerA0_Config_Delay(delay_half_sec) ;
            start_Timer_A0() ;
        }
        else {
            servo_index += 1 ;
        }
    }
    else if (timer_state == 1) {
        if ( DC_PWM < 0x0A3D) {
            timer_state = 2 ;
            TimerA0_Config_servo(DC_PWM) ;
            start_Timer_A0() ;
            DC_PWM = DC_PWM + 33 ;
            TA0CTL &= ~TAIFG ;
        }
        else {
            DC_PWM = 0x0A3D ;
            state = 8 ;
            IFG2 &= ~UCA0TXIFG ;
            IE2 &= ~UCA0TXIE ;
            IE2 |= UCA0RXIE;
            flag_1 = 0;
            flag_3 = 0;
        }
    }
    else if (timer_state == 2){
        if (servo_index == 10) {
            if ( state == 1 ) {
                timer_state = 3 ;
                start_Timer_A1(state) ;
                servo_index = 0 ;
                stop_Timer_A0();
            }
            else if ( state == 3) {
                stop_Timer_A0();
                servo_index = 0 ;
                timer_state = 7 ;
                LDR_1_Meas() ;
                }
            }
        else {
            servo_index += 1 ;
        }
    }
    else if (timer_state == 4){      //rotate for specific deg - Telemater, one time
        if (servo_index == 35) {
            servo_index = 0 ;
            stop_Timer_A0();
            cci_flag = 1 ;
            start_Timer_A1(state) ;

        }
        else {
            servo_index += 1 ;
        }
    }
    /*
    else if (timer_state == 5){
        stop_Timer_A0();
        start_Timer_A1(state) ;
        timer_state = 4 ;
    }*/

    else if (timer_state == 7 ){
        stop_Timer_A0();
        itoa(LDR_res, str_LDR, current_angle);
        IE2 |= UCA0TXIE;     // Enable USCI_A0 TX interrupt
        UCA0TXBUF = str_LDR[j++];
        TimerA0_Config_Delay(0x2000) ;
        timer_state = 1 ;
        start_Timer_A0() ;
        current_angle += 3 ;
    }

   if(state == 5) {         //Delay for script mode functions
       __bic_SR_register_on_exit(LPM0_bits + GIE);      // Exit LPM0
   }

   if(state == 6) {         //Delay for LDR Calib mode functions
       stop_Timer_A0();
       itoa(LDR_res, str_LDR, LDR_dist);
       IE2 |= UCA0TXIE;     // Enable USCI_A0 TX interrupt
       UCA0TXBUF = str_LDR[j++];
       LDR_dist += 5 ;

   }
}

//*********************************************************************
//            ADC10 Interrupt Service Rotine
//*********************************************************************
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
  LPM0_EXIT ;
}

//*********************************************************************
//         USCI A0/B0 Receive Interrupt Service Rotine
//*********************************************************************
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){
    if ( state == 8 ) {
           state = UCA0RXBUF - 48;
        if (state == 5){
            play_mode = 0 ;         // Play
            load_mode =  0 ;
            ShouldWrite = 0 ;
            script_cmd_done = 0 ;
            IE2 &= ~UCA0TXIE;
            IFG2 &= ~UCA0TXIFG;
       }
        if ( state == 2 ||state ==6 || state == 5) { // LDR Calib or Telemeter
              more_data_flag =0 ;    // go to sleep
       }
        LPM0_EXIT; // Quit sleep mode
    }

    else if (state != 8){        // If Im here it's because I need more data from user
        more_data_flag = 1 ;
    }

    if (state == 2){               //Define angle for Telemater
        if (more_data_flag == 1){
            T_on[RX_index++] = UCA0RXBUF;
            if (T_on[RX_index - 1] == '\0'){
                RX_index = 0;
                timer_state = 4 ;
                t_on_tel = str2int(T_on);
                TimerA0_Config_0_deg(t_on_tel) ;
                t_on_tel = 0 ;
                start_Timer_A0() ;
            }
        }
        if (UCA0RXBUF == 0x72) {
            IE2 |= UCA0TXIE;
            TA1CCTL2 &= ~ COV;
            TA1CCTL2 |= CCIE;
            TA0CCTL0 |= CCIE;
        }
    }
    /*
     else if (state == 6 && UCA0RXBUF == 0x53){               //sampling for LDR
        //IE2 |= UCA0TXIE;                      // Enable USCI_A0 TX interrupt
         samp_flag = 1 ;
        //samp = UCA0RXBUF - 41;
        //__bic_SR_register_on_exit(LPM0_bits + GIE);      // Exit LPM0 on return to main
    }*/



    else if (state == 5 && more_data_flag == 1 && !(script_cmd_done)) {                                  //Script Mode management
        script_cmd_done = 1 ;
        if (UCA0RXBUF == 0x4C || UCA0RXBUF == 0x57) {         //checking if Load or Play
        if (UCA0RXBUF == 0x4C) {
          load_mode = 1;      // write
          play_mode = 0 ;
        }
        else{
            play_mode = 1 ;         // Play
            load_mode =  0 ;
        }
      }
    }
    else if (play_mode == 1 && UCA0RXBUF != 88){               // read, then change script to execute - first time of entrance
        if (UCA0RXBUF <= 0x33 && UCA0RXBUF >= 0x31){
          read_script = UCA0RXBUF - 48;                    // Script1/Script2/Script3
          __bic_SR_register_on_exit(LPM0_bits + GIE);      // Exit LPM0 on return to main
        }
      }
      
      else if(UCA0RXBUF == 88) {                           // if event 'Exit Script Mode' pressed
        state = 8;
        __bic_SR_register_on_exit(LPM0_bits + GIE);        // Exit LPM0 on return to main
      }

      else if(script_cmd_done == 1) {                                               //in this phase we read the data to cmd in order to save it in flash memory
        cmd[r_index++] = UCA0RXBUF;
        //if (cmd[r_index - 1] == '\n' || cmd[r_index - 1] == '\0')
        if (cmd[r_index - 1] == '\0')
        {
          r_index = 0;
          ShouldWrite = 1;
          __bic_SR_register_on_exit(LPM0_bits + GIE);      // Exit LPM0 on return to main
        }
      }
}

//*********************************************************************
//         USCI A0/B0 Transmit Interrupt Service Rotine
//*********************************************************************
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void) {
    if (state ==1 || state ==2 ) {
        if (str_diff[j] != '\0') {
            UCA0TXBUF = str_diff[j++] ;
        }

        else {
            if(timer_state == 4) {        //when state 2 on
                start_Timer_A1(state);
            }
            IFG2 &= ~UCA0TXIFG;
            IE2 &= ~UCA0TXIE;
            j = 0 ;
        }
    }
    else if (state == 6 || state ==3 ) {              //when state 6 on
        if (str_LDR[j] != '\0') {
            UCA0TXBUF = str_LDR[j++] ;
        }

        else {
            IFG2 &= ~UCA0TXIFG;
            IE2 &= ~UCA0TXIE;
            j = 0 ;
        }
    }
}

//*********************************************************************
//            Port2 Interrupt Service Rotine
//*********************************************************************
#pragma vector=PORT2_VECTOR
  __interrupt void PBs_handler(void){

    delay(debounceVal);
//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------
    if(PBsArrIntPend & PB0){
      PBsArrIntPend &= ~PB0;
      if (LDR_dist > 50 ) {
          state = 8 ;
      }
    }


//---------------------------------------------------------------------
//            Exit from a given LPM
//---------------------------------------------------------------------
        switch(lpm_mode){
        case mode0:
         LPM0_EXIT; // must be called from ISR only
         break;

        case mode1:
         LPM1_EXIT; // must be called from ISR only
         break;

        case mode2:
         LPM2_EXIT; // must be called from ISR only
         break;

                case mode3:
         LPM3_EXIT; // must be called from ISR only
         break;

                case mode4:
         LPM4_EXIT; // must be called from ISR only
         break;
    }

}


//---------------------------------------------------------------------
//            LCD
//---------------------------------------------------------------------

  //******************************************************************
  // send a command to the LCD
  //******************************************************************
  void lcd_cmd(unsigned char c){

      LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

      if (LCD_MODE == FOURBIT_MODE)
      {
          LCD_Data_Write &= ~OUTPUT_DATA;// clear bits before new write
          LCD_Data_Write |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
          lcd_strobe();
          LCD_Data_Write &= ~OUTPUT_DATA;
          LCD_Data_Write |= (c & (0x0F)) << LCD_DATA_OFFSET;
          lcd_strobe();
      }
      else
      {
          LCD_Data_Write = c;
          lcd_strobe();
      }
  }


  //******************************************************************
  // send data to the LCD
  //******************************************************************
  void lcd_data(unsigned char c){

      LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

      LCD_Data_Write &= ~OUTPUT_DATA;
      LCD_RS(1);
      if (LCD_MODE == FOURBIT_MODE)
      {
              LCD_Data_Write &= ~OUTPUT_DATA;
              LCD_Data_Write |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
              lcd_strobe();
              LCD_Data_Write &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
              LCD_Data_Write &= ~OUTPUT_DATA;
              LCD_Data_Write |= (c & 0x0F) << LCD_DATA_OFFSET;
              lcd_strobe();
      }
      else
      {
              LCD_Data_Write = c;
              lcd_strobe();
      }

      LCD_RS(0);
  }

  //******************************************************************
  // write a string of chars to the LCD
  //******************************************************************
  void lcd_puts(const char * s){

      while(*s)
          lcd_data(*s++);
  }


  //******************************************************************
  // initialize the LCD
  //******************************************************************
  void lcd_init(){

      char init_value;

      if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
      else init_value = 0x3F;

      LCD_RS_DIR(OUTPUT_PIN);
      LCD_EN_DIR(OUTPUT_PIN);
      LCD_RW_DIR(OUTPUT_PIN);
      LCD_Data_Dir |= OUTPUT_DATA;
      LCD_RS(0);
      LCD_EN(0);
      LCD_RW(0);

      DelayMs(15);
      LCD_Data_Write &= ~OUTPUT_DATA;
      LCD_Data_Write |= init_value;
      lcd_strobe();
      DelayMs(5);
      LCD_Data_Write &= ~OUTPUT_DATA;
      LCD_Data_Write |= init_value;
      lcd_strobe();
      DelayUs(200);
      LCD_Data_Write &= ~OUTPUT_DATA;
      LCD_Data_Write |= init_value;
      lcd_strobe();

      if (LCD_MODE == FOURBIT_MODE){
          LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
          LCD_Data_Write &= ~OUTPUT_DATA;
          LCD_Data_Write |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
          lcd_strobe();
          lcd_cmd(0x28); // Function Set
      }
      else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots

      lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
      lcd_cmd(0x1); //Display Clear
      lcd_cmd(0x6); //Entry Mode
      lcd_cmd(0x80); //Initialize DDRAM address to zero
  }

  //******************************************************************
  // lcd strobe functions
  //******************************************************************
  void lcd_strobe(){
    LCD_EN(1);
    asm("nOp");
    //asm("NOP");
    LCD_EN(0);
  }

  //******************************************************************
  // Delay usec functions
  //******************************************************************
  void DelayUs(unsigned int cnt){

      unsigned char j;
      for(j=cnt ; j>0 ; j--) asm("n0p"); // the command asm("nop") takes raphly 1usec

  }
  //******************************************************************
  // Delay msec functions
  //******************************************************************
  void DelayMs(unsigned int cnt){

      unsigned char j;
      for(j=cnt ; j>0 ; j--) DelayUs(1000); // the command asm("nop") takes raphly 1usec

  }


