#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430g2553.h>          // MSP430x2xx


#define   debounceVal      250
#define   half_sec_delay   1000

//-------------------------------------------------------------
//            LCD abstraction 
//-------------------------------------------------------------
// LCD Data
#define LCD_Data_Sel    P1SEL
#define LCD_Data_Dir    P1DIR
#define LCD_Data_Write  P1OUT
#define LCD_Data_Read   P1IN

// LCD Control
#define LCD_CTL_Sel     P2SEL
#define LCD_CTL_Dir     P2DIR
#define LCd_CTL_Write   P2OUT


//-------------------------------------------------------------
//            UART abstraction: P1.1 - P1.2
//-------------------------------------------------------------
#define UART_Port_Sel      P1SEL
#define UART_Port_Sel2     P1SEL2
#define UART_Port_Dir      P1DIR
#define UART_Port_Out      P1OUT
#define UART_Port_In       P1IN

//-------------------------------------------------------------
//            LDR abstraction: P1.3 and P1.0
//-------------------------------------------------------------
#define LDR_Port_Sel     P1SEL
#define LDR_Port_Dir     P1DIR
#define LDR_Port_Read    P1IN

 //-------------------------------------------------------------
  //            PushButtons abstraction P2.0
  //-------------------------------------------------------------
#define PBsArrPort         P2IN
#define PBsArrIntPend      P2IFG
#define PBsArrIntEn        P2IE
#define PBsArrIntEdgeSel   P2IES
#define PBsArrPortSel      P2SEL
#define PBsArrPortDir      P2DIR
#define PB0                0x01

 //-------------------------------------------------------------
 //            Servo Engine abstraction P2.1
 //-------------------------------------------------------------
#define Servo_Port_Sel    P2SEL
#define Servo_Port_Dir    P2DIR
#define Servo_Port_Out    P2OUT


 //-------------------------------------------------------------
 //            UltraSonic abstraction P2.3
 //-------------------------------------------------------------
#define Sonic_Port_Sel    P2SEL
#define Sonic_Port_Dir    P2DIR
#define Sonic_Port_Out    P2OUT
#define Sonic_Port_In     P2IN



extern void GPIOconfig(void);
extern void InitTIMER(void);
extern void TimerA0_Config_Delay(unsigned int Delay) ;
extern void TimerA0_Config_servo(unsigned int DutyCycle) ;
extern void TimerA0_Config_0_deg(unsigned int on_time) ;
extern void Config_Timer_for_Servo_2(unsigned int period , unsigned int on_time) ; 
extern void Timer_A0_on (void) ; 
extern void Timer_A0_off (void) ; 
extern void ADC_Config (void) ;
extern void ADC_Switch_P1_3 (void);
extern void ADC_Switch_P1_0 (void);
extern void ADC10_on (void) ;
extern void ADC_off (void) ;
extern void TimerA0_Config_LCD(unsigned int Delay);

#endif





