# Embedded Systems Labs
**Author:** Sofija Joksimović

## Overview
This repository contains summary of practical lab exercises from the course **Microprocessor Systems** (Mikroprocesorski sistemi).  
The labs demonstrate proficiency in embedded programming, microcontroller peripherals, and simulation using STM32CubeMX, Eclipse, and Proteus.

## Technologies Used
- **STM32CubeMX** – microcontroller initialization and configuration  
- **Eclipse IDE** – development environment  
- **C language** – embedded programming  
- **Proteus** – simulation of microcontroller circuits  
- **Peripherals used:** ADC, PWM, GPIO, Timers, UART, I2C, EXTI (External Interrupts)  
- **FreeRTOS** – real-time multitasking  

### Advanced Features
Labs demonstrate advanced embedded programming techniques:
- **EXTI (External Interrupts)** – handling push-button inputs and triggering ISR (Interrupt Service Routine).  
- **FreeRTOS** – task creation, scheduling, and synchronization for multitasking applications.


## Project Overview
This lab implements a system that reads temperature, wind speed, wind direction, and rainfall intensity, and controls a fan accordingly using **STM32F103R6**.  
The project demonstrates the use of:


### Peripherals & Components
- **Motor** – controlled via PWM (fan speed).  
- **Multiplexer 74HC4051** – selects analog sensor input.  
- **LM35 Temperature Sensor** – outputs temperature voltage.  
- **Anemometer** – outputs PWM signal for wind speed.  
- **Wind Vane** – outputs analog voltage for azimuth.  
- **Rain Gauge** – outputs pulses for rainfall intensity.  
- **LCD LM041L / HD44780** – displays temperature, wind speed, azimuth, rainfall.  
- **Virtual Terminal (USART1)** – prints formatted values.  

### Features Implemented
1. LCD display with custom arrows ↑ and ↓ to indicate increasing/decreasing values.  
2. Periodic update of:
   - **Tem:** temperature (3rd row)  
   - **Brz:** wind speed (2nd row)  
   - **Azi:** wind azimuth (1st row)  
   - **Kis:** rainfall intensity (4th row)  
3. Virtual terminal output `/A=<a>/B=<b>/T=<t>/K=<k>` updated every 200ms.  
4. Fan speed control based on temperature:
   - 0–30°C → fan off  
   - 30–35°C → 50% max speed  
   - 35–60°C → full speed  
5. Tasks scheduled every 200ms for reading sensors and updating displays.  

### Technologies Used
- **C / C++** for embedded programming  
- **STM32CubeMX** for peripheral configuration  
- **Proteus** for simulation  
- **EXTI & FreeRTOS** for task scheduling and interrupt handling  

### How to Run
1. Open `.ioc` file in STM32CubeMX for configuration reference.  
2. Open `.pdsprj` file in Proteus to simulate the system.  
3. Build project in preferred IDE (Eclipse/STM32CubeIDE).  
4. Observe LCD display, virtual terminal, and fan behavior.

### Notes
- Sensor readings that fail are displayed as `?`.  
- All periodic updates occur every 200ms.  
- Custom LCD characters (arrows) are used to indicate trend directions.
