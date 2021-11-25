// Support routines for ECE 198 (University of Waterloo)

// Written by Bernie Roehl, July 2021

#include <stdbool.h>  // for bool datatype

#include "ece198.h"

///////////////////////
// Initializing Pins //
///////////////////////

// Initialize a pin (or pins) to a particular mode, with optional pull-up or pull-down resistors
// and possible alternate function
// (we use this so students don't need to know about structs)

void InitializePin(GPIO_TypeDef *port, uint16_t pins, uint32_t mode, uint32_t pullups, uint8_t alternate)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = pins;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = pullups;
    GPIO_InitStruct.Alternate = alternate;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void InitializePin1(GPIO_TypeDef *port, uint16_t pins, uint32_t mode, uint32_t pullups, uint8_t alternate)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = pins;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = pullups;
    GPIO_InitStruct.Alternate = alternate;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}
/////////////////
// Serial Port //
/////////////////

UART_HandleTypeDef UART_Handle;  // the serial port we're using

// initialize the serial port at a particular baud rate (PlatformIO serial monitor defaults to 9600)

HAL_StatusTypeDef SerialSetup(uint32_t baudrate)
{
    __USART2_CLK_ENABLE();        // enable clock to USART2
    __HAL_RCC_GPIOA_CLK_ENABLE(); // serial port is on GPIOA

    GPIO_InitTypeDef GPIO_InitStruct;

    // pin 2 is serial RX
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // pin 3 is serial TX (most settings same as for RX)
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // configure the serial port
    UART_Handle.Instance = USART2;
    UART_Handle.Init.BaudRate = baudrate;
    UART_Handle.Init.WordLength = UART_WORDLENGTH_8B;
    UART_Handle.Init.StopBits = UART_STOPBITS_1;
    UART_Handle.Init.Parity = UART_PARITY_NONE;
    UART_Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UART_Handle.Init.Mode = UART_MODE_TX_RX;

    return HAL_UART_Init(&UART_Handle);
}

// wait for a character to arrive from the serial port, then return it

char SerialGetc()
{
    while ((UART_Handle.Instance->SR & USART_SR_RXNE) == 0)
        ;                             // wait for the receiver buffer to be Not Empty
    return UART_Handle.Instance->DR;  // return the incoming character
}

// send a single character out the serial port

void SerialPutc(char c)
{
    while ((UART_Handle.Instance->SR & USART_SR_TXE) == 0)
        ;                          // wait for transmitter buffer to be empty
    UART_Handle.Instance->DR = c;  // send the character
}

// write a string of characters to the serial port

void SerialPuts(char *ptr)
{
    while (*ptr)
        SerialPutc(*ptr++);
}

// get a string of characters (up to maxlen) from the serial port into a buffer,
// collecting them until the user presses the enter key;
// also echoes the typed characters back to the user, and handles backspacing

void SerialGets(char *buff, int maxlen)
{
    int i = 0;
    while (1)
    {
        char c = SerialGetc();
        if (c == '\r') // user pressed Enter key
        {
            buff[i] = '\0';
            SerialPuts("\r\n"); // echo return and newline
            return;
        }
        else if (c == '\b') // user pressed Backspace key
        {
            if (i > 0)
            {
                --i;
                SerialPuts("\b \b"); // overwrite previous character with space
            }
        }
        else if (i < maxlen - 1) // user pressed a regular key
        {
            buff[i++] = c;  // store it in the buffer
            SerialPutc(c);  // echo it
        }
    }
}

////////////////////
// Rotary Encoder //
////////////////////

// read a rotary encoder (handles the quadrature encoding)
//(uses a previousClk boolean variable provided by the caller)

int ReadEncoder(GPIO_TypeDef *clkport, int clkpin, GPIO_TypeDef *dtport, int dtpin, bool *previousClk)
{
    bool clk = HAL_GPIO_ReadPin(clkport, clkpin);
    bool dt = HAL_GPIO_ReadPin(dtport, dtpin);
    int result = 0;  // default to zero if encoder hasn't moved
    if (clk != *previousClk)           // if the clk signal has changed since last time we were called...
    {
        result = dt != clk ? 1 : -1;   // set the result to the direction (-1 if clk == dt, 1 if they differ)
    }

    *previousClk = clk;                // store for next time
    return result;
}

int ReadEncoder1(GPIO_TypeDef *clkport, int clkpin, GPIO_TypeDef *dtport, int dtpin, bool *previousClk)
{
    bool clk = HAL_GPIO_ReadPin(clkport, clkpin);
    bool dt = HAL_GPIO_ReadPin(dtport, dtpin);
    int result = 0;  // default to zero if encoder hasn't moved
    if (clk != *previousClk)           // if the clk signal has changed since last time we were called...
    {
        result = dt != clk ? 1 : -1;   // set the result to the direction (-1 if clk == dt, 1 if they differ)
    }

    *previousClk = clk;                // store for next time
    return result;
}

///////////////////////
// 7-Segment Display //
///////////////////////

//First 7 segment display
struct { GPIO_TypeDef *port; uint32_t pin; }
segments[] = {
    { GPIOA, GPIO_PIN_0 },  // A
    { GPIOA, GPIO_PIN_1 },  // B
    { GPIOA, GPIO_PIN_4 },  // C
    { GPIOB, GPIO_PIN_0 },  // D
    { GPIOC, GPIO_PIN_1 },  // E
    { GPIOC, GPIO_PIN_0 },  // F
    { GPIOB, GPIO_PIN_8 },  // G
    { GPIOB, GPIO_PIN_9 },  // H (also called DP)
};

//Second 7 segment display
struct { GPIO_TypeDef *port; uint32_t pin; }
segments1[] = {
    { GPIOA, GPIO_PIN_6 },  // A
    { GPIOA, GPIO_PIN_7 },  // B
    { GPIOA, GPIO_PIN_9 },  // C
    { GPIOC, GPIO_PIN_7 },  // D
    { GPIOB, GPIO_PIN_6 },  // E
    { GPIOA, GPIO_PIN_5 },  // F
    { GPIOB, GPIO_PIN_10 },  // G
    { GPIOA, GPIO_PIN_8 },  // H (also called DP)
};

// for each digit, we have a byte (uint8_t) which stores which segments are on and off
// (bits are ABCDEFGH, right to left, so the low-order bit is segment A)
uint8_t digitmap[10] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67 };
uint8_t digitmap1[10] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67 };

//First 7 segment initialize and display
void Initialize7Segment() {
    for (int i = 0; i < 8; ++i)
        InitializePin(segments[i].port, segments[i].pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);
}

void Display7Segment(int digit) {
    int value = 0;  // by default, don't turn on any segments
    if (digit >= 0 && digit <= 9)  // see if it's a valid digit
        value = digitmap[digit];   // convert digit to a byte which specifies which segments are on
    //value = ~value;   // uncomment this line for common-anode displays
    // go through the segments, turning them on or off depending on the corresponding bit
    for (int i = 0; i < 8; ++i)
        HAL_GPIO_WritePin(segments[i].port, segments[i].pin, (value >> i) & 0x01);  // move bit into bottom position and isolate it
}

//Second 7 segment initialize and display
void Initialize7Segment1() {
    for (int i = 0; i < 8; ++i)
        InitializePin(segments1[i].port, segments1[i].pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);
}

void Display7Segment1(int digit1) {
    int value1 = 0;  // by default, don't turn on any segments
    if (digit1 >= 0 && digit1 <= 9)  // see if it's a valid digit
        value1 = digitmap1[digit1];   // convert digit to a byte which specifies which segments are on
    //value = ~value;   // uncomment this line for common-anode displays
    // go through the segments, turning them on or off depending on the corresponding bit
    for (int i = 0; i < 8; ++i)
        HAL_GPIO_WritePin(segments1[i].port, segments1[i].pin, (value1 >> i) & 0x01);  // move bit into bottom position and isolate it
}

/////////
// ADC //
/////////

// Written by Rodolfo Pellizzoni, September 2021

void InitializeADC(ADC_HandleTypeDef* adc, ADC_TypeDef* whichAdc)  // whichADC might be ADC1, for example
{
    adc->Instance = whichAdc;
    adc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    adc->Init.Resolution = ADC_RESOLUTION_12B;
    adc->Init.ScanConvMode = DISABLE;
    adc->Init.ContinuousConvMode = DISABLE;
    adc->Init.DiscontinuousConvMode = DISABLE;
    adc->Init.NbrOfDiscConversion = 1;
    adc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc->Init.NbrOfConversion = 1;
    adc->Init.DMAContinuousRequests = DISABLE;
    adc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    HAL_ADC_Init(adc);
}

// read from the specified ADC channel

uint16_t ReadADC(ADC_HandleTypeDef* adc, uint32_t channel)  // channel might be ADC_CHANNEL_1 for example
{
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    HAL_ADC_ConfigChannel(adc, &sConfig);
    HAL_ADC_Start(adc);
    HAL_ADC_PollForConversion(adc, HAL_MAX_DELAY);
    uint16_t res = HAL_ADC_GetValue(adc);
    HAL_ADC_Stop(adc);
    return res;
}

