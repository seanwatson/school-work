Real-Time Embedded Systems
==========================

Course Description
------------------

Review of embedded processors, memory systems, basic peripherals; real-time kernel configurations, task control blocks, interrupt service routines, real-time clocks/timers, multi-tasking, schedulability analysis, inter-task communication, signals, message queues, cooperative/pre-emptive multi-tasking, priority scheduling, priority inversion problems, timing considerations, deadline handing, input-output handling; practical issues in computer control; design, development, and testing techniques.

Project Description
-------------------

The major objective of this project was to implement a closed loop control system using two microprocessor development boards. A cylinder with two open ends was provided. At one end of the cylinder there was a fan and at the other end there was an infrared position sensor. A ping pong ball was placed in the cylinder and I was required to implement a system that could allow a user to set and adjust the ball height in the cylinder by controlling the fan speed and by using the feedback from the position sensor. The system was broken down between the two boards as follows:

### Board A

Board A handled all of the necessary user interface tasks including

1. Reading a potentiometer input using analog to digital conversion to determine the desired height of the ball
2. Transmitting the desired height to Board B via a serial connection
3. Allowing the user to select between text or graphical display modes. Text mode showed all of the system's internal values while the graphical mode created a simple depiction of the cylinder and the ball's position within it.
4. Allowing the user to select between variable (potentiometer) or step input modes
5. Handling all communication between the development computer and the microprocessor via serial link

All tasks were implemented using a co-operative multitasking structure

### Board B

Board B was responsible for all control related tasks

1. Reading the position sensor using analog to digital conversion
2. Scaling the position sensor reading using linear approximation
3. Reading the desired position from the serial port
4. Calculating the error and control signals using a PID control system
5. Generating an appropriate PWM output signal to drive the fan
6. Reading push button inputs that could be used to adjust the control system gains on the fly

Board B's tasks were also implemented in a co-operative structure with the exception of the push buttons and PWM generation. The push buttons were implemented using external interrupts and the PWM generation task used an internal timer interrupt to ensure maximum accuracy.

Incorporated Topics
-------------------

- Analog to digital conversion
- Standard input and output
- Interrupts and service routines
- Timers
- Serial communication
- Multitasking
- Control systems
- PWM generation
- Intertask communication
- Priority scheduling
