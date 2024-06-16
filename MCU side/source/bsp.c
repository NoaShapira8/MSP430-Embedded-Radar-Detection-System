#include  "../header/bsp.h"    // private library - BSP layer


void GPIOconfig(void) { 
// UART Config
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
  if (CALBC1_1MHZ==0xFF)                  // If calibration constant erased
  {
    while(1);                               // do not load, trap CPU!!
  }
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
  DCOCTL = CALDCO_1MHZ;
  
  //-------------------------------------------------------------
  //            LCD setup
  //-------------------------------------------------------------
  LCD_Data_Sel &= ~0xF0 ;                          // makes P1.4 - P1.7 IO mode
  LCD_Data_Dir |= 0xF0 ;                           // makes P1.4 - P1.7 Output mode
  LCD_Data_Write &= ~0xF0 ;                         // CLR P1.4 - P1.7
  
  LCD_CTL_Sel &= ~(BIT1 + BIT5 + BIT7) ;             // makes P2.1 , P2.5 , P2.7 IO mode
  LCD_CTL_Dir |= (BIT1 + BIT5 + BIT7) ;              // makes P2.1 , P2.5 , P2.7 Output mode
  LCd_CTL_Write &= ~(BIT1 + BIT5 + BIT7) ;           // CLR P2.1 , P2.5 , P2.7
  
  
  //-------------------------------------------------------------
  //            UART setup
  //-------------------------------------------------------------
  UART_Port_Sel |= BIT1 + BIT2 ;      // P1.1 = RXD, P1.2=TXD
  UART_Port_Sel2 |= BIT1 + BIT2 ;     // P1.1 = RXD, P1.2=TXD
  UART_Port_Out &= ~(BIT1 + BIT2) ;
  
  UCA0CTL1 |= UCSSEL_2;                    // CLK = SMCLK
  UCA0BR0 = 104;                           // config the low bits of baud rate
  UCA0BR1 = 0x00;                          // config the high bits of baud rate
  UCA0MCTL = UCBRS0;
  UCA0CTL1 &= ~UCSWRST;                 // **Initialize USCI state machine**
  
  
  //-------------------------------------------------------------
  //            LDR setup
  //-------------------------------------------------------------
  LDR_Port_Sel &= ~0x09 ;             // makes P1.1 & P1.3 IO mode
  LDR_Port_Dir &= ~0x09 ;            // makes P1.1 & P1.3 Input mode
  
  
  //-------------------------------------------------------------
  //            PushButtons setup
  //-------------------------------------------------------------
  PBsArrPortSel &= ~0x01;
  PBsArrPortDir &= ~0x01;
  PBsArrIntEdgeSel |= 0x01;  	     // pull-up mode
  PBsArrIntEn |= 0x01;
  PBsArrIntPend &= ~0xFF;            // clear pending interrupts
  
  _BIS_SR(GIE);                     // enable interrupts globally
  
  
  //-------------------------------------------------------------
  //            UltraSonic setup
  //-------------------------------------------------------------
   Sonic_Port_Sel |= (BIT2 + BIT4) ;   // makes P2.2 , P2.4 Timer mode
   Sonic_Port_Dir |= BIT2 ;    // makes P2.2 Output mode (trigger)
   //Sonic_Port_Dir &= ~0x10 ;   // makes P2.4 Input mode (Echo)
   //Sonic_Port_Out &= ~ 0x04 ;  // Clear P2.2
  
  
  //-------------------------------------------------------------
  //            PWM(Servo) setup
  //-------------------------------------------------------------
   Servo_Port_Sel |= BIT6 ;   // makes P2.6 IO mode
   Servo_Port_Dir |= BIT6 ;   // makes P2.6 Output mode
   Servo_Port_Out &= ~ 0x02 ;    // Clear P2.6
   
   //-------------------------------------------------------------
   //            Flash setup
   //-------------------------------------------------------------
   FCTL2 = FWKEY + FSSEL0 + FN1;
   FCTL1 = FWKEY + ERASE;                    // Set Erase bit
   FCTL3 = FWKEY;                            // Clear Lock bit
   //*Flash_ptr = 0;                        // Dummy write to erase Flash segment
}


void InitTIMER(void) {
 // init TimerA1 for Ultra Sonic sensor
    TA1CTL = TACLR ;
    TA1CTL &= ~ TAIFG ;
    TA1CCTL0 &= ~ CCIFG ;
    TA1CCTL2 &= ~ CCIFG ;
    TA1CCTL1 = OUTMOD_7 ;      // output PWM signal for triger (10us)
    TA1CTL = TASSEL_2 + MC_0;  //smclock, stop mode
    TA1CCTL2 = 0x00 ;          //clear
    TA1CCTL2 |= CM_3 + SCS + CAP ;  //capture on both rising and falling edges, sync cap, capure mode - for echo

}

void TimerA0_Config_Delay(unsigned int Delay) {
    TA0CTL = TACLR ;    //Clr
    TA0CCR0 = Delay ;  //half sec delay
    TACTL = TASSEL_2 + MC_0 + ID_3 ;  //  select: 2 - SMCLK ; control: 1 - Up ; divider: 3 - /8
    TA0CCTL0 = CCIE ;
}

void TimerA0_Config_servo(unsigned int DutyCycle) {         // rotate servo 0-180 deg
    TA0CTL = TACLR ;
    TA0CTL &= ~ TAIFG ;
    TA0CTL |= TASSEL_2 + MC_0 ;   //smclock, stop mode
    TA0CCR0 = 0x6666;
    TA0CCR1 = DutyCycle ; // max is 0A32, min is 0276 (jumps of 55 for rotating of 5 deg)
    TA0CCTL1 = 0x00 ;
    TA0CCTL1 |= OUTMOD_7 ;
    TA1CCTL2 &= ~ CCIFG;
    TA1CCTL0 &= ~ CCIFG;
}

void TimerA0_Config_0_deg(unsigned int on_time) {           // rotate servo for a specific deg
    TA0CTL = TACLR ;
    TA0CTL &= ~ TAIFG ;
    TA0CTL |= TASSEL_2 + MC_0 ;   //smclock, stop mode
    TA0CCR0 = 0x6666 ;
    TA0CCR1 = on_time ; // max is 0x0A3D, min is 0x0276 (jumps of 33 for rotating of 3 deg)
    TA0CCTL1 = 0x00 ;
    TA0CCTL1 |= OUTMOD_7 ;
    TA1CCTL2 &= ~ CCIFG;
    TA1CCTL0 &= ~ CCIFG;
}


void Timer_A0_on ( void) { 
  TA0CTL = 0x0004 ;  //Clr
  TA0CCR0 = 0x9FFF ;
  TACTL = TASSEL_2 + MC_1 + ID_3 ;  //  select: 2 - SMCLK ; control: 1 - Up ; divider: 3 - /8
  TA0CCTL0 = CCIE ;
}


void TimerA0_Config_LCD(unsigned int Delay) {
    TA0CTL = TACLR ;    //Clr
    TA0CCR0 = Delay ;
    TACTL = TASSEL_2 + MC_1 + ID_3 ;  //  select: 2 - SMCLK ; control: 1 - Up ; divider: 3 - /8
    TA0CCTL0 = CCIE ;
}


void Timer_A0_off ( void) { 
  TACTL &= MC0 ; 
}


void ADC_Config( void) {        // This function configs the ADC to sample P1.0
  ADC10CTL1 |= ADC10DIV_7 + ADC10SSEL_3 ;
  ADC10CTL0 = ADC10SHT_3 + ADC10ON + ADC10SR + MSC + ADC10IE ;
}

void ADC10_on (void) {
  ADC10CTL0 |= ENC + ADC10SC +ADC10ON  ;  // Start the ADC

}
void ADC_off(void) {
  ADC10CTL0 &= ~ADC10ON ;
  ADC10CTL0 &= ~ENC ;
}

void ADC_Switch_P1_3 (void) {
    ADC10CTL1 &= ~  INCH_0 ;
    ADC10CTL1 |= INCH_3 ;
    ADC10AE0 = 0x03 ;
}

void ADC_Switch_P1_0 (void) {
    ADC10CTL1 &= ~  INCH_0 ;
    ADC10CTL1 |= INCH_3 ;
    ADC10AE0 = 0x03 ;
}
 
          
             
            
  

