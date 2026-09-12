# STM32F401 Smart Garage Access Control System

An embedded smart garage access-control system developed using the **STM32F401CCU6**, featuring password authentication, servo-based gate control, vehicle counting, emergency handling, lockout protection, and a 16x2 LCD user interface.

The project was implemented using a modular embedded-software architecture with custom peripheral drivers and a **non-blocking gate state machine**.

---

## Project Overview

The system implements the control logic of a smart garage entrance.

Users interact with the system through a **4x4 keypad** and receive real-time feedback through a **16x2 LCD**.

After successful authentication, the system controls a servo-operated garage gate while maintaining the number of available parking spaces.

The firmware also includes safety and security mechanisms such as:

- Password authentication
- Failed-attempt lockout protection
- Emergency operating mode
- Vehicle entry and exit counting
- Garage capacity management
- Non-blocking servo gate control
- LCD status messages
- LED status indication

---

## Hardware Implementation

The project was tested on a physical breadboard prototype using the STM32F401CCU6.

Main hardware components include:

- STM32F401CCU6 development board
- 16x2 LCD
- 4x4 matrix keypad
- Servo motor
- Status LEDs
- Breadboard
- Jumper wires
- External power connections

---

## Hardware Demonstration

### System Startup

The LCD displays the firmware identification during system startup.

![Smart Garage Firmware](assets/smart_garage_firmware.jpg)

---

### Password Authentication

The user enters the garage password using the keypad.

![Enter Password](assets/enter_password.jpg)

---

### Access Granted

After entering the correct password, the system grants access and opens the garage gate.

![Access Granted](assets/access_granted.jpg)

---

### Vehicle Counter

The system keeps track of vehicles inside the garage and updates the number of available spaces.

![Vehicle Counter](assets/vehicle_counter.jpg)

---

### Vehicle Exit

When a vehicle exits, the internal vehicle count is updated.

![Vehicle Exit](assets/vehicle_exit.jpg)

---

### Emergency Mode

The firmware includes an emergency operating mode that changes the normal system behavior and provides a reset option.

![Emergency Mode](assets/emergency_mode.jpg)

---

## System Architecture

The firmware is organized into separate modules to improve readability, maintainability, and scalability.

```text
stm32f401-smart-garage/
│
├── Inc/
│   ├── access_control.h
│   ├── app_config.h
│   ├── gpio_driver.h
│   ├── keypad.h
│   ├── lcd.h
│   ├── servo.h
│   └── timebase.h
│
├── Src/
│   ├── access_control.c
│   ├── gpio_driver.c
│   ├── keypad.c
│   ├── lcd.c
│   ├── main.c
│   ├── servo.c
│   ├── syscalls.c
│   ├── sysmem.c
│   └── timebase.c
│
├── Startup/
│   └── startup_stm32f401ccux.s
│
├── assets/
│
├── .cproject
├── .project
├── .gitignore
├── STM32F401CCUX_FLASH.ld
└── README.md
```

---

## Firmware Modules

### `main.c`

Contains the main application loop and coordinates the different software modules.

### `access_control.c`

Implements the main smart-garage access logic, including authentication, garage state management, and system behavior.

### `gpio_driver.c`

Provides GPIO configuration and control functions used by the application modules.

### `keypad.c`

Handles scanning and reading input from the 4x4 matrix keypad.

### `lcd.c`

Provides the interface used to display system messages and garage information on the 16x2 LCD.

### `servo.c`

Controls the servo motor responsible for opening and closing the garage gate.

### `timebase.c`

Provides timing functionality used by the firmware and supports non-blocking system operation.

---

## Access Control

The system requires the user to enter a password through the keypad.

The LCD provides interactive feedback during the authentication process.

The general authentication flow is:

```text
System Start
     │
     ▼
Enter Password
     │
     ▼
Validate Password
     │
 ┌───┴────┐
 │        │
Correct  Wrong
 │        │
 ▼        ▼
Access   Retry /
Granted  Lockout
 │
 ▼
Open Gate
```

---

## Non-Blocking Gate Control

Instead of relying on long blocking delays for gate movement, the firmware uses a state-based approach.

This allows the application to continue handling other system functions while the gate sequence is active.

A simplified gate sequence is:

```text
CLOSED
   │
   ▼
OPENING
   │
   ▼
OPEN
   │
   ▼
CLOSING
   │
   ▼
CLOSED
```

This architecture is more suitable for real embedded systems than a design based entirely on blocking delays.

---

## Vehicle Counting

The system maintains the number of vehicles currently inside the garage.

When a vehicle successfully enters:

```text
Vehicle Entered
      │
      ▼
Car Count + 1
      │
      ▼
Available Spaces - 1
```

When a vehicle exits:

```text
Vehicle Exited
      │
      ▼
Car Count - 1
      │
      ▼
Available Spaces + 1
```

The updated information is displayed on the LCD.

---

## Emergency Mode

An emergency operating state is implemented in the firmware.

When emergency mode is activated, the normal garage workflow is interrupted and the system provides a dedicated emergency interface.

The LCD informs the user that the system is operating in emergency mode and provides the appropriate reset instruction.

---

## Lockout Protection

To improve access security, the firmware includes protection against repeated incorrect password attempts.

After multiple failed authentication attempts, normal access can be temporarily restricted according to the firmware's lockout logic.

This prevents unlimited password attempts and demonstrates a basic embedded access-control security mechanism.

---

## Embedded Software Design

The project demonstrates several important embedded-system concepts:

- Bare-metal C programming
- Modular firmware architecture
- GPIO peripheral control
- Matrix keypad scanning
- Character LCD interfacing
- PWM-based servo control
- State-machine design
- Non-blocking timing
- Password authentication
- Access-control logic
- Vehicle counting
- Emergency-state handling
- Hardware/software integration

---

## Development Tools

- **Microcontroller:** STM32F401CCU6
- **IDE:** STM32CubeIDE
- **Programming Language:** C
- **Hardware Interface:** GPIO
- **Display:** 16x2 Character LCD
- **Input:** 4x4 Matrix Keypad
- **Actuator:** Servo Motor
- **Development Method:** Modular embedded firmware

---

## Project Goals

The main objective of this project was to build a practical embedded control system rather than a single isolated peripheral demonstration.

The project combines multiple hardware and software components into one application and focuses on:

- Structured firmware development
- Real-time system behavior
- Modular driver development
- State-based control
- User interaction
- Safety and security logic
- Hardware integration

---

## Future Improvements

Possible extensions include:

- Proteus simulation and full virtual system validation
- IR or ultrasonic vehicle detection
- RFID-based authentication
- Bluetooth or Wi-Fi monitoring
- Mobile application integration
- EEPROM/Flash password storage
- Real-time parking-space sensors
- Event logging
- IoT dashboard integration
- PCB implementation

---

## Author

**Zyad Khalaf Amen**

Mechatronics Engineering Student  
Misr University for Science and Technology (MUST)

Interests:

- Robotics
- Embedded Systems
- Computer Vision
- Industrial Automation
- Mechatronics Systems
