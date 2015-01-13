PLC simulator
=============
Written in C++/Qt. Uses Boost C++ classic Spirit parser.
Create two subdirs: 'debug' and 'release' for containing the executable and the VirtualPLC lib (static linking).


Input format
------------
Loop light:

    //------------------------------------------------ const declarations
    CONST INT time0 = 10;
    CONST INT time1 = 20;
    CONST INT time2 = 30;
    CONST INT time3 = 40;
    CONST INT time4 = 50;

    //------------------------------------------------------ declarations
    INT counter1 = 1111;
    BOOL input0 = false;
    BOOL input1 = false;
    BOOL output0 = false;
    BOOL output1 = false;
    BOOL output2 = false;
    BOOL output3 = false;
    BOOL output4 = false;
    BOOL output5 = false;
    INT counter2 = 9999;

    // Function blocks --------------------------------------------------
    TIMER_M TIMER0C(input0, time0);
    TIMER_M TIMER1C(input0, time1);
    TIMER_M TIMER2C(input0, time2);
    TIMER_M TIMER3C(input0, time3);
    TIMER_M TIMER4C(input0, time4);

    // Mapping to I/O ---------------------------------------------------
    X0 = input0;
    X1 = input1;
    Y0 = output0;
    Y1 = output1;
    Y2 = output2;
    Y3 = output3;
    Y4 = output4;
    Y5 = output5;
    
    // Ladder diagram and logic expressions -----------------------------
    
    TIMER0S                           output0
    ---[]--------------------------------(S)-----
    
    TIMER1S                 output0   output1
    ---[]----------------------(R)-------(S)-----
    
    TIMER2S                 output1   output2
    ---[]----------------------(R)-------(S)-----
    
    TIMER3S                 output2   output3  
    ---[]----------------------(R)-------(S)----
    
    TIMER4S     output3   TIMER0C  TIMER1C  TIMER2C  TIMER3C  TIMER4C
    ---[]------------(R)-------(R)------(R)------(R)------(R)------(R)---
    


Simulates a PLC and controls a number of actuators.

Under construction.
