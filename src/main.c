// Sample code for ECE 198

// Written by Bernie Roehl, August 2021

// This file contains code for a number of different examples.
// Each one is surrounded by an #ifdef ... #endif block inside of main().

// To run a particular example, you should remove the comment (//) in
// front of exactly ONE of the following lines:

#define GAME
//#define TESTING_ROTARY_ENCODER1
//#define TESTING_ROTARY_ENCODER2
//#define ROTARY_ENCODER2
//#define TESTING_LED

#include <stdbool.h> // booleans, i.e. true and false
#include <stdio.h>   // sprintf() function
#include <stdlib.h>  // srand() and random() functions

#include "ece198.h"
#include "LiquidCrystal.h"

int main(void) // hello world
{

    //void game(); 

    HAL_Init(); // initialize the Hardware Abstraction Layer


    // Peripherals (including GPIOs) are disabled by default to save power, so we
    // use the Reset and Clock Control registers to enable the GPIO peripherals that we're using.

    __HAL_RCC_GPIOA_CLK_ENABLE(); // enable port A (for the on-board LED, for example)
    __HAL_RCC_GPIOB_CLK_ENABLE(); // enable port B (for the rotary encoder inputs, for example)
    __HAL_RCC_GPIOC_CLK_ENABLE(); // enable port C (for the on-board blue pushbutton, for example)

    // initialize the pins to be input, output, alternate function, etc...
    InitializePin(GPIOA, GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);  // on-board LED
    InitializePin(GPIOA, GPIO_PIN_10, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);
    // note: the on-board pushbutton is fine with the default values (no internal pull-up resistor
    // is required, since there's one on the board)

    // set up for serial communication to the host computer
    // (anything we write to the serial port will appear in the terminal (i.e. serial monitor) in VSCode)

    SerialSetup(9600);

    // as mentioned above, only one of the following code sections will be used
    // (depending on which of the #define statements at the top of this file has been uncommented)

    //Initialize the display, specifying what port and pins to use:

#ifdef GAME
    // Display the numbers 0 to 9 inclusive on the 7-segment display, pausing for a second between each one.
    // (remember that the GND connection on the display must go through a 220 ohm current-limiting resistor!)
    Initialize7Segment1();
    InitializePin(GPIOB, GPIO_PIN_13, GPIO_MODE_INPUT, GPIO_PULLUP, 0);   // initialize CLK pin
    InitializePin(GPIOB, GPIO_PIN_14, GPIO_MODE_INPUT, GPIO_PULLUP, 0);   // initialize DT pin
    InitializePin(GPIOB, GPIO_PIN_15, GPIO_MODE_INPUT, GPIO_PULLUP, 0);  // initialize SW pin
    
    bool previousClk1 = false;  // needed by ReadEncoder() to store the previous state of the CLK pin
    int count1 = 0;             

    Initialize7Segment();
    InitializePin(GPIOB, GPIO_PIN_5, GPIO_MODE_INPUT, GPIO_PULLUP, 0);   // initialize CLK pin
    InitializePin(GPIOB, GPIO_PIN_3, GPIO_MODE_INPUT, GPIO_PULLUP, 0);   // initialize DT pin
    InitializePin(GPIOB, GPIO_PIN_4, GPIO_MODE_INPUT, GPIO_PULLUP, 0);  // initialize SW pin
    
    bool previousClk = false;  // needed by ReadEncoder() to store the previous state of the CLK pin
    int count = 0;             // this gets incremented or decremented as we rotate the encoder

    while (true)
    {
        // int check = 0;
        int delta = ReadEncoder(GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_3, &previousClk);  // update the count by -1, 0 or +1
        int delta1 = ReadEncoder1(GPIOB, GPIO_PIN_13, GPIOB, GPIO_PIN_14, &previousClk1);  // update the count by -1, 0 or +1

        if (delta != 0 || delta1 != 0) {
            count -= delta;
            if(count == 10)
            {
                count = 0;
            }
            else if(count == -1)
            {
                count = 9;
            }
            char buff[100];
            sprintf(buff, "%d  \r", count);
            SerialPuts(buff);
            // if(count == 2)
            // {
            //     check = 1;
            // }
            // else
            // {
            //     check = 0;
            // }
            count1 -= delta1;
            if(count1 == 10)
            {
                count1 = 0;
            }
            else if(count1 == -1)
            {
                count1 = 9;
            }
            char buff1[100];
            sprintf(buff1, "%d  \r", count1);
            SerialPuts(buff1);
            if(count1 == 7 && count == 4)
            {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, true);
            }
            else
            {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, false);
            }

            Display7Segment(count);
            Display7Segment1(count1);
        }

    }

#endif

/* Function to turn LED on after 2.5 seconds, then turn it off
#ifdef TESTING_LED

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, true);

    //delays LED turn on for 2.5s
    HAL_Delay(2500);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, false);

    //Solenoid Lock
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, true);

    //7 Segment Display

    // Initialize7Segment();
    // while (true)
    //     for (int i = 0; i < 10; ++i)
    //     {
    //         Display7Segment(i);
    //         HAL_Delay(1000);  // 1000 milliseconds == 1 second
    //     }

    LiquidCrystal(GPIOC, GPIO_PIN_10, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15, GPIO_PIN_2, GPIO_PIN_3);
    setCursor(0, 0);
    print("Hello World");

#endif

#ifdef ROTARY_ENCODER2
    // Display the numbers 0 to 9 inclusive on the 7-segment display, pausing for a second between each one.
    // (remember that the GND connection on the display must go through a 220 ohm current-limiting resistor!)
    
    Initialize7Segment1();
    InitializePin(GPIOB, GPIO_PIN_13, GPIO_MODE_INPUT, GPIO_PULLUP, 0);   // initialize CLK pin
    InitializePin(GPIOB, GPIO_PIN_14, GPIO_MODE_INPUT, GPIO_PULLUP, 0);   // initialize DT pin
    InitializePin(GPIOB, GPIO_PIN_15, GPIO_MODE_INPUT, GPIO_PULLUP, 0);  // initialize SW pin
    
    bool previousClk1 = false;  // needed by ReadEncoder() to store the previous state of the CLK pin
    int count1 = 0;             // this gets incremented or decremented as we rotate the encoder

    while (true)
    {
        int delta1 = ReadEncoder1(GPIOB, GPIO_PIN_13, GPIOB, GPIO_PIN_14, &previousClk1);  // update the count by -1, 0 or +1
        if (delta1 != 0) {
            count1 -= delta1;
            if(count1 == 10)
            {
                count1 = 0;
            }
            else if(count1 == -1)
            {
                count1 = 9;
            }
            char buff[100];
            sprintf(buff, "%d  \r", count1);
            SerialPuts(buff);
            if(count1 == 4)
            {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, true);
            }
            else
            {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, false);
            }
        }
        Display7Segment1(count1);
    }

#endif

#ifdef TESTING_ROTARY_ENCODER1
    // Read values from the rotary encoder and update a count, which is displayed in the console.

    InitializePin(GPIOB, GPIO_PIN_5, GPIO_MODE_INPUT, GPIO_PULLUP, 0);   // initialize CLK pin
    InitializePin(GPIOB, GPIO_PIN_3, GPIO_MODE_INPUT, GPIO_PULLUP, 0);   // initialize DT pin
    InitializePin(GPIOB, GPIO_PIN_4, GPIO_MODE_INPUT, GPIO_PULLUP, 0);  // initialize SW pin
    
    bool previousClk = false;  // needed by ReadEncoder() to store the previous state of the CLK pin
    int count = 0;             // this gets incremented or decremented as we rotate the encoder



    //%d - means printing integer (the integer is count)
    while (true)
    {   
        int delta = ReadEncoder(GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_3, &previousClk);  // update the count by -1, 0 or +1
        if (delta != 0) {
            count -= delta;
            char buff[100];
            sprintf(buff, "%d  \r", count);
            SerialPuts(buff);
        }
        bool sw = !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4);  // read the push-switch on the encoder (active low, so we invert it using !)
    }
#endif

#ifdef TESTING_ROTARY_ENCODER2
    // Read values from the rotary encoder and update a count, which is displayed in the console.

    InitializePin(GPIOB, GPIO_PIN_13, GPIO_MODE_INPUT, GPIO_PULLUP, 0);   // initialize CLK pin
    InitializePin(GPIOB, GPIO_PIN_14, GPIO_MODE_INPUT, GPIO_PULLUP, 0);   // initialize DT pin
    InitializePin(GPIOB, GPIO_PIN_15, GPIO_MODE_INPUT, GPIO_PULLUP, 0);  // initialize SW pin
    
    bool previousClk = false;  // needed by ReadEncoder() to store the previous state of the CLK pin
    int count = 0;             // this gets incremented or decremented as we rotate the encoder

    //%d - means printing integer (the integer is count)
    while (true)
    {   
        int delta = ReadEncoder(GPIOB, GPIO_PIN_13, GPIOB, GPIO_PIN_14, &previousClk);  // update the count by -1, 0 or +1
        if (delta != 0) {
            count -= delta;
            char buff[100];
            sprintf(buff, "%d  \r", count);
            SerialPuts(buff);
        }
    }
#endif
*/
    return 0;
}

// This function is called by the HAL once every millisecond
void SysTick_Handler(void)
{
    HAL_IncTick(); // tell HAL that a new tick has happened
    // we can do other things in here too if we need to, but be careful
}
