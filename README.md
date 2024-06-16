# MSP430-Embedded-Radar-Detection-System

Welcome to this GitHub repository showcasing an advanced embedded system project for MSP430 microcontrollers, programmed in C, leveraging a finite state machine (FSM) for managing a radar detection system. This project is designed for intricate hardware control, making it ideal for educational purposes and advanced embedded development.

# Features

- **Modular Design**: The code is organized into Application (APP), Board Support Package (BSP), Hardware Abstraction Layer (HAL), and Application Programming Interface (API) layers, enhancing both maintainability and scalability.
- **Finite State Machine (FSM)**: Manages system states and transitions for effective control of operational modes, ensuring a robust and responsive system.
- **Peripheral Management**: Utilizes UART for efficient data transfer between the PC and MCU, facilitating remote control and monitoring of the radar detection system.
- **Low Power Management**: Implements various low-power modes to optimize power consumption, crucial for battery-operated and energy-efficient applications.

## FSM States and Operations

The FSM in this project manages different aspects of system functionality through these states and functions:

- **State 0**: Idle mode.
- **State 1**: Control servo motor to a specific angle.
- **State 2**: Measure distance using the ultrasonic sensor.
- **State 3**: Perform radar scanning by rotating the servo motor.
- **State 4**: Script mode to automate tasks.
- **State 5**: Display LDR measurements on an LCD.
- **State 6**: Measure LDR values and send data via UART.
- **State 7**: Reserved for future use.
- **State 8**: Waiting for new state commands.

## Peripheral Utilization

Key Peripherals:

- **UART**
- **GPIOs**
- **LCD**
- **Servo Motor**
- **Ultrasonic Sensor**
- **LDR (Light Dependent Resistor)**
- **Timers**
- **Interrupts**

## Project Structure

- **api.h & api.c**: Define and implement interactions with peripherals.
- **app.h**: Declares FSM states, modes, and global variables.
- **bsp.h & bsp.c**: Include hardware-specific configurations and initializations.
- **halGPIO.h & halGPIO.c**: Provide lower-level hardware management functions.
- **main.c**: Initializes the system and manages the operational loop.

## Usage

1. **Setup**: Ensure you have the necessary hardware components connected to the MSP430 microcontroller.
2. **Build and Flash**: Compile the code and flash it onto the MSP430 microcontroller using an appropriate programmer.
3. **Run**: Power up the system and interact with it using UART commands via a PC to control the radar detection system.

This project demonstrates how to effectively manage a radar detection system using an FSM in an embedded C environment, showcasing advanced techniques in modular design, peripheral management, and low-power operation.
For more info, go to https://youtu.be/k-vPiOiqaU4
