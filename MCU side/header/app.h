#ifndef _app_H_
#define _app_H_

extern int state, T_on_delay, s3_first,PWM_status, x; // global variable
enum SYSmode{mode0,mode1,mode2,mode3,mode4}; // global variable
extern unsigned int RX_index, Menu_index, angle, more_data_flag ;
extern char T_on[] ;
extern int LDR_res, samp, first_round, flag_1, flag_3, flag_4,flag_6, sand_samp_flag, timer_state;
extern const char  CountUp_Str[];
extern unsigned int delay_half_sec ;

//variable for UltraSonic Sensor
extern volatile int temp[2] ,diff;
extern unsigned int i  ;
extern char str_diff[13];
extern int cap_flag, j, test[60], t, cci_flag ;

//variable for Servo motor
extern int current_angle, DC_PWM, servo_index, t_on_tel;

//variable for LDR Functions
extern int LDR_dist , send_flag;
extern char str_LDR[13];

//variable for Script
extern int play_mode ,load_mode, ShouldWrite, read_script, script_num, script_cmd_done ;
extern int opc, op1, op2, d, z;
extern char cmd[];
extern unsigned int r_index, counterUp, counterDown ;
extern char *Flash_ptr, *first_script_ptr, *second_script_ptr, *third_script_ptr, *read_ptr;

#endif






