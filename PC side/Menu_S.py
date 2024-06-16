import numpy as np
import matplotlib
matplotlib.use('TkAgg')
import PySimpleGUI as sg
import matplotlib.pyplot as plt
import serial as ser
import time
from queue import Queue



#Global variables
global state_flag, input_degree
r_max = 100.0
dist_list = []
angle_list = []
angle_list3 = []
dist_list3 = []
voltage_list = []
voltage_list4 = []
angle_list4 = []
dist_list4 = []
angle_list6 = []
dist_list6 = []
voltage_dist_list4 = []


def start_communication():
    serial_comm = ser.Serial('COM5', baudrate=9600, bytesize=ser.EIGHTBITS,
                             parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                             timeout=1)
    enableTX = True
    serial_comm.flush()  # clear the port
    serial_comm.reset_input_buffer()
    serial_comm.reset_output_buffer()
    return serial_comm

def transmit_data(data):
    MSP_state = str(data)
    s.write(bytes(MSP_state, 'ascii'))
    print("the data transimtted is: ", bytes(MSP_state, 'ascii'))
    while (s.out_waiting > 0):     # while the output buffer isn't empty
        if s.out_waiting == 0:
            break
    print("the transimtted end ")
    return None

def translate_script_file(lines):
    command_map = {
        "inc_lcd": "01",
        "dec_lcd": "02",
        "rra_lcd": "03",
        "set_delay": "04",
        "clear_lcd": "05",
        "servo_deg": "06",
        "servo_scan": "07",
        "sleep": "08"
    }

    translated_lines = []     # List to accumulate the translated lines

    for line in lines:
        parts = line.strip().split()
        command = parts[0]
        if command not in command_map:
            raise ValueError("Unknown command: {command}")
        hex_value = command_map[command]

        if len(parts) > 1:
            if command == "servo_scan":
                servo_args = parts[1].split(',')
                if len(servo_args) != 2:
                    raise ValueError("Invalid number of arguments for servo_scan")
                servo_value = format(int(servo_args[0]), '02X') + format(int(servo_args[1]), '02X')
                hex_value += servo_value
            else:
                value = int(parts[1])
                hex_value += format(value, '02X')

        translated_lines.append(hex_value + "\n")

    return translated_lines

def transmit_script_file(lines) :
    lines[-1] += "\0"
    for line in lines:
        for char in line:
            str_char = str(char)
            print(str_char)
            sending_message = bytes(str_char, 'ascii')
            s.write(sending_message)
            while (s.out_waiting > 0):       # while the output buffer isn't empty
                if s.out_waiting == 0:
                    break

def get_angle() :
    sg.theme('SandyBeach')              # Add a touch of color
    layout_angle = [[sg.Text('Please Insert angle from 0 to 180')],
                   [sg.Text("Enter new angle:")],
                   [sg.Input(key='input_entry')],
                   [sg.Button("Submit")]]

    window_angle = sg.Window('Angle Input',layout_angle)
    while True:
        window = window_angle
        event,values = window.read()
        if event in ("Submit", sg.WIN_CLOSED):
            angle = values['input_entry']
            if angle.isnumeric() and 0 <= int(angle) <= 180:
                window_angle.close()
                return int(angle)
            else:
                sg.popup("Invalid angle! Please enter a value from 0 to 180.")
        window.close
        break

def get_samp_req(samp_distance) :
    layout_samp = [[sg.Text('Press to single Sample the LDR in distance ' + str(samp_distance))],
                    [sg.Button('Sample', size=(20,1))]]
    window_samp = sg.Window('Single Sample the LDR' , layout_samp)
    while True :
        window = window_samp
        event , values = window.read()
        if event == 'Sample' :
            transmit_data('S')
            sample = LDR_Calib()
            window.close()
            break
    return sample

def read_script_file(script_file_name):
    with open(script_file_name + '.txt') as f:
        lines = f.readlines()
        num_lines = len(lines)
        print('Data of ' + str(script_file_name))
        for line in lines:
            print(line)
        print("Number of lines = " + str(num_lines))
    return lines, num_lines

def script_mode_window():
    layout_script_mode = [[sg.Text('Please choose file to send')],
                          [sg.Button('Load Script 1')],
                          [sg.Button('Load Script 2')],
                          [sg.Button('Load Script 3')],
                          [sg.Button('Play Script 1')],
                          [sg.Button('Play Script 2')],
                          [sg.Button('Play Script 3')],
                          [sg.Button('Exit Script Mode')]
                          ]
    window_script_mode = sg.Window("Script Mode Window", layout_script_mode)

    while True:
            window = window_script_mode
            event, values = window.read()
            if event == 'Load Script 1':
                lines, num_lines = read_script_file(script_file_name = 'Script1')
                transmit_data('L')
                new_lines = translate_script_file(lines)
                for line in new_lines:
                    print(line)
                transmit_script_file(new_lines)
            elif event == 'Load Script 2':
                lines, num_lines = read_script_file(script_file_name = 'Script2')
                transmit_data('L')
                new_lines = translate_script_file(lines)
                for line in new_lines:
                    print(line)
                transmit_script_file(new_lines)
            elif event == 'Load Script 3':
                lines, num_lines = read_script_file(script_file_name = 'Script3')
                transmit_data('L')
                new_lines = translate_script_file(lines)
                for line in new_lines:
                    print(line)
                transmit_script_file(new_lines)
            elif event == 'Play Script 1':
                transmit_data('W')
                transmit_data(1)
                script_get_func()
            elif event == 'Play Script 2':
                transmit_data('W')
                transmit_data(2)
                script_get_func()
            elif event == 'Play Script 3':
                transmit_data('W')
                transmit_data(3)
                script_get_func()

            elif event in ('Exit Script Mode', sg.WIN_CLOSED):
                transmit_data('X')
                window.close()
                break

def plot_radar_view(dist_list, angle_list):
    global r_max
    # Create the radar plot
    angles = np.radians(angle_list)[:180]
    fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})
    ax.set_theta_zero_location("E")
    ax.set_theta_direction(1)
    ax.set_ylim(0, r_max)  # Set the radar plot's radial axis limit to include the max distance
    ax.scatter(angles, dist_list, marker='o',color='blue', label='Detected Objects')
    ax.set_rlabel_position(90)
    ax.set_title("Radar View", pad=30)

    # Add grid lines and legend
    ax.grid(True)
    ax.legend(loc='upper right')

    # Display the plot
    plt.show()

def plot_LDR_radar_view(dist_list, angle_list):
    global r_max
    # Create the radar plot
    angles = np.radians(angle_list)[:180]
    fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})
    ax.set_theta_zero_location("N")
    ax.set_theta_direction(-1)
    ax.set_ylim(0, r_max)  # Set the radar plot's radial axis limit to include the max distance
    ax.scatter(angles, dist_list, marker='o',color='red', label='Detected Light Source')
    ax.set_rlabel_position(90)
    ax.set_title("Radar View", pad=30)

    # Add grid lines and legend
    ax.grid(True)
    ax.legend(loc='upper right')

    # Display the plot
    plt.show()

def plot_Object_Light_view(updated_dist_list, voltage_dist_list, angle_list):
    global r_max
    # Create the radar plot
    angles = np.radians(angle_list)[:180]
    fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})
    ax.set_theta_zero_location("E")
    ax.set_theta_direction(1)
    ax.set_ylim(0, r_max)  # Set the radar plot's radial axis limit to include the max distance

    # Plot the updated_dist_list
    ax.scatter(angles, updated_dist_list, marker='o', color='blue', label='Detected Objects')

    # Plot the voltage_dist_list with a different color and shape
    ax.scatter(angles, voltage_dist_list, marker='x', color='red', label='Detected Light Sources')

    ax.set_rlabel_position(90)
    ax.set_title("Radar View", pad=30)

    # Add grid lines and legend
    ax.grid(True)
    ax.legend(loc='upper right')

    # Display the plot
    plt.show()

def radar_detector_system():
    global dist_list, angle_list, r_max
    r_max = 100
    angle = 0
    while angle < 180 :           #add cond of back button
        if s.in_waiting > 0:
            data_enc = s.readline()
            data_str = data_enc.decode('utf-8').strip()
            dist_str, angle_str = data_str.split(',')
            dist , angle = int(dist_str) , int(angle_str)
            dist = dist * 34645 / (2 * pow(10, 6))
            print("the dist is:", dist, "cm, the angle is:", angle_str)
            if dist > r_max:
                dist = 0.0  # measuring more than r_max, it's likely inaccurate
            dist_list.append(dist)
            angle_list.append(angle)

    plot_radar_view(dist_list, angle_list)

def Telemeter():
    global dist, samp_q
    print("def Telemater")
    layout = [
        [sg.Text("The distance is: 0.00", size=(30, 1), key='-OUTPUT-')],
        [sg.Button("Exit")]
    ]
    window = sg.Window("Average Distance", layout)
    while True:
        if s.in_waiting > 0:
            data_enc = s.readline()
            data_str = data_enc.decode('utf-8').strip()
            dist_str, angle_str = data_str.split(',')
            dist = int(dist_str)
            dist = dist * 34645 / (2 * pow(10, 6))
            print("the distance is: " , dist)
            window['-OUTPUT-'].update("The distance is: {:.2f}".format(dist))

        event, values = window.read(timeout=1000)  # Update every 1000 milliseconds (1 second)
        if event == sg.WIN_CLOSED or event == "Exit":
            transmit_data('X')
            window.close()
            break

def light_source_detector_system():
    global voltage_list, angle_list3, dist_list3
    angle = 0
    while angle < 180:
        if s.in_waiting > 0:
            data_enc = s.readline()
            data_str = data_enc.decode('utf-8').strip()
            volt_str, angle_str = data_str.split(',')
            volt, angle = int(volt_str), int(angle_str)
            print("the voltage is:", volt, " the angle is:", angle)
            voltage_list.append(volt)
            angle_list3.append(angle)
    dist_list3 = find_closest_indexes(voltage_list, full_array)

    plot_LDR_radar_view(dist_list3, angle_list3)

def Object_and_light_Detector():
    global voltage_list4, angle_list4, dist_list4, full_array, voltage_dist_list4
    angle = 0
    LDR_flag = 1
    while angle < 180:
        if (LDR_flag == 1):
            if s.in_waiting > 0:
                data_enc = s.readline()
                data_str = data_enc.decode('utf-8').strip()
                volt_str, angle_str = data_str.split(',')
                volt = int(volt_str)
                print("the voltage is:", volt, " the angle is:", int(angle_str))
                voltage_list4.append(volt)
                LDR_flag = 0
        if (LDR_flag == 0):
            if s.in_waiting > 0:
                data_enc = s.readline()
                data_str = data_enc.decode('utf-8').strip()
                dist_str, angle_str = data_str.split(',')
                dist, angle = int(dist_str), int(angle_str)
                dist = dist * 34645 / (2 * pow(10, 6))
                print("the dist is:", dist, "cm, the angle is:", angle_str)
                if dist > r_max:
                    dist = 0.0                 # measuring more than r_max, it's likely inaccurate
                dist_list4.append(dist)
                angle_list4.append(angle)
                LDR_flag = 1
    updated_dist_list4 = update_dist_list(full_array, voltage_list4, dist_list4)
    voltage_dist_list4 = find_closest_indexes(voltage_list4, full_array)

    plot_Object_Light_view(updated_dist_list4, voltage_dist_list4, angle_list4)

def find_closest_indexes(voltage_array, full_array):
    global dist_list3
    full_array = np.array(full_array)
    output_array = []
    for voltage_value in voltage_array:
        if voltage_value > full_array[-1]:
            closest_index = 0
        else:
            closest_index = np.abs(full_array - voltage_value).argmin()
        output_array.append(closest_index)
    return output_array

def LDR_Calib():
    global voltage, LDR_dist, empty_array
    LDR_dist = 0
    while LDR_dist < 50 :
        if s.in_waiting > 0:
            data_enc = s.readline()
            data_str = data_enc.decode('utf-8').strip()
            voltage_str, LDR_dist_str = data_str.split(',')
            voltage , LDR_dist = int(voltage_str) , int(LDR_dist_str)
            print("the voltage is:", voltage, "the distance is:", LDR_dist)
            if LDR_dist ==0 :
                empty_array[LDR_dist] = voltage
            else :
                empty_array[LDR_dist-1] = voltage

def fill_with_linear_interpolation(arr):
    if len(arr) != 50:
        raise ValueError("Input array length must be 50.")
    existing_indexes = np.where(arr != 0)[0]           # Find the indexes with non-zero values (indexes that are multiples of 5)
    existing_values = arr[existing_indexes]             # Get the values at the existing indexes
    filled_arr = np.zeros(50)                           # Create an array with all zero values
    filled_arr[existing_indexes] = existing_values     # Fill the array with the existing values
    zero_indexes = np.where(filled_arr == 0)[0]        # Find the indexes with zero values
    closest_existing_indexes = np.argmin(np.abs(zero_indexes[:, None] - existing_indexes), axis=1)         # Find the indexes with non-zero values that are closest to each zero index
    interpolated_values = np.interp(zero_indexes, existing_indexes[closest_existing_indexes], existing_values[closest_existing_indexes])
    filled_arr[zero_indexes] = np.rint(interpolated_values).astype(int)  # Round the interpolated values to the nearest integers
    return filled_arr

def update_dist_list(full_array, voltage_list4, dist_list4):
    # Find the maximum value in the full_array
    max_full_array = np.max(full_array)

    for i, voltage_value in enumerate(voltage_list4):
        if voltage_value < max_full_array:
            # Set the corresponding element in dist_list4 to 0
            dist_list4[i] = 0.0

    return dist_list4

def script_get_func():
    while True:
        if s.in_waiting > 0:
            data_enc = s.readline().decode('ascii')
            print("the script state is: ", data_enc)
            if (data_enc == '6') :
                script_func_6()
            if (data_enc == '7') :
                script_func_7()
            if (data_enc == 'F'):
                break

def script_func_6():            #Servo_deg_func6
    global dist
    print("i'm in def Script Func_6")
    layout = [
        [sg.Text("The distance is: 0.00", size=(30, 1), key='-OUTPUT-')],
        [sg.Button("Exit")]
    ]
    window = sg.Window("Average Distance Script Func_6", layout)
    while True:
        if s.in_waiting > 0:
            data_enc = s.readline()
            data_str = data_enc.decode('utf-8').strip()
            dist_str, angle_str = data_str.split(',')
            dist = int(dist_str)
            dist = dist * 34645 / (2 * pow(10, 6))
            print("the distance is: " , dist)

            window['-OUTPUT-'].update("The distance is: {:.2f}".format(dist))

        event, values = window.read(timeout=1000)  # Update every 1000 milliseconds (1 second)
        if event == sg.WIN_CLOSED or event == "Exit":
            transmit_data('B')
            s.reset_input_buffer()
            window.close()
            break

def script_func_7():
    global dist_list6, angle_list6, r_max, max_angle_int
    print("i'm in Script Func_7")
    r_max = 100
    angle = 0
    max_angle_int = 180
    while (angle < max_angle_int -3):
        if s.in_waiting > 0:
            data_enc = s.readline()
            if b"," not in data_enc:
                max_angle =  data_enc.decode('ascii')
                max_angle_int = int(max_angle)
                print("i got a max angle:" ,max_angle)

            else:
                data_str = data_enc.decode('utf-8').strip()
                dist_str, angle_str = data_str.split(',')
                dist, angle = int(dist_str), int(angle_str)
                dist = dist * 34645 / (2 * pow(10, 6))
                print("the dist is:", dist, "cm, the angle is:", angle_str)
                if dist > r_max:
                    dist = 0.0  # measuring more than r_max, it's likely inaccurate
                dist_list6.append(dist)
                angle_list6.append(angle)

def state1():
    global state_flag
    state_flag = 1
    transmit_data(state_flag)
    time.sleep(0.5)
    print("State1")
    radar_detector_system()

def state2():
    global state_flag, samp_q
    state_flag = 2
    transmit_data(state_flag)
    time.sleep(0.5)
    print("i'm asking for angle")
    angle = get_angle()
    CCR1 = str(int((int(angle) * 8.87) + 630))
    s.write(bytes(CCR1 + '\0', 'ascii'))
    time.sleep(0.5)
    print('state2')
    Telemeter()

def state3():
    global state_flag
    state_flag = 3
    transmit_data(state_flag)
    time.sleep(0.5)
    print("State3")
    light_source_detector_system()

def state4():
    global state_flag
    state_flag = 4
    transmit_data(state_flag)
    time.sleep(0.5)
    print("State4")
    Object_and_light_Detector()

def Script_Mode() :
    global state_flag
    state_flag = 5
    transmit_data(state_flag)
    script_mode_window()
    print("S")

def state6():
    global state_flag
    global empty_array, full_array
    empty_array = np.zeros(50)
    state_flag = 6
    transmit_data(state_flag)
    time.sleep(0.5)
    LDR_Calib()
    full_array = fill_with_linear_interpolation (empty_array)   #the dictionary of LDR distances
    print(full_array)

s = start_communication()


sg.theme('DarkAmber')   # Add a touch of color
layout_main = [[sg.Text('Please choose from the menu')],
            [sg.Button('Objects Detector System')],
            [sg.Button('Telemeter')],
            [sg.Button('Light Sources Detector System')],
            [sg.Button('Light Sources and Objects Detector System')],
            [sg.Button('Script Mode')],
            [sg.Button('LDR Calib', button_color=('white', 'green'))],
            [sg.Button('Exit', button_color=('white', 'red'))]
                 ]
window_main = sg.Window('Radar Detector System', layout_main)

while True:
    window = window_main
    event, values = window.read()
    if event == 'Objects Detector System':
        s.reset_input_buffer()
        s.reset_output_buffer()
        state1()

    elif event == 'Telemeter':
        s.reset_input_buffer()
        s.reset_output_buffer()
        state2()

    elif event == 'Light Sources Detector System':
        s.reset_input_buffer()
        s.reset_output_buffer()
        state3()

    elif event == 'Light Sources and Objects Detector System':
        s.reset_input_buffer()
        s.reset_output_buffer()
        state4()

    elif event == 'Script Mode':
        s.reset_input_buffer()
        s.reset_output_buffer()
        Script_Mode()

    elif event == 'LDR Calib':
        s.reset_input_buffer()
        s.reset_output_buffer()
        state6()

    elif event == 'Exit':
        break




