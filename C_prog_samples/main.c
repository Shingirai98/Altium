
//********************************************************************
//*                    EEE2046F C                             *
//*==================================================================*
//* WRITTEN BY: Shingirai Maburutse                                  *
//* DATE CREATED:  02/06/2019                                        *
//* MODIFIED:                                                        *
//*==================================================================*
//* PROGRAMMED IN: Eclipse Neon 3 Service Release 1 (4.4.1)          *
//* DEV. BOARD:    UCT STM32 Development Board                       *
//* TARGET:       STMicroelectronics STM32F051C6                     *
//*==================================================================*
//* DESCRIPTION:                                                     *
//*                                                                  *
//********************************************************************
// INCLUDE FILES
//====================================================================
#include "lcd_stm32f0.h"
#include "stm32f0xx.h"
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
//====================================================================
// SYMBOLIC CONSTANTS
//====================================================================
#define TRUE 1
#define FALSE 0
#define DELAY0 1500
#define DELAY1 900
#define MAX_COUNT 255
#define MIN_COUNT 0
//====================================================================
// NEW VARIABLE TYPES
//====================================================================
typedef uint8_t flag_t;
//====================================================================
// GLOBAL VARIABLES
//====================================================================
flag_t startFlag = FALSE;
flag_t upFlag = FALSE;
flag_t downFlag = FALSE;
int16_t count = 0;
char* countASCII = NULL;
uint16_t ALL_LED_ON = GPIO_ODR_0 | GPIO_ODR_1 | GPIO_ODR_2 | GPIO_ODR_3 |
        GPIO_ODR_4 | GPIO_ODR_5 | GPIO_ODR_6 | GPIO_ODR_7;

//====================================================================
// FUNCTION DECLARATIONS
//====================================================================
void init_GPIO(void);
void Delay(void);
void display(void);
void checkPB(void);
void counter(void);
void convert2BCDASCII(const uint8_t countValue, char* resultPtr);
//====================================================================
// MAIN FUNCTION
//====================================================================
void main (void)
{
    init_LCD();
    init_GPIO();
    countASCII = (char*)malloc(sizeof(char));

    for(;;)
    {

        checkPB();
        display();
        counter();
        Delay();
    }
}                                        // End of main

//====================================================================
// FUNCTION DEFINITIONS
//====================================================================
void init_GPIO(void)
{
    //enable clocks
    RCC -> AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN;

    //Initialization of Push buttons
    //set to digital input mode
    GPIOA -> MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1 | GPIO_MODER_MODER2);
    //enable pull-up resistors
    GPIOA -> PUPDR |= GPIO_PUPDR_PUPDR0 | GPIO_PUPDR_PUPDR1 | GPIO_PUPDR_PUPDR2;
    GPIOA -> PUPDR &= ~(GPIO_PUPDR_PUPDR0_1 | GPIO_PUPDR_PUPDR1_1 | GPIO_PUPDR_PUPDR2_1);
    //set default values
    GPIOA -> IDR |= GPIO_IDR_0 | GPIO_IDR_1 | GPIO_IDR_2;

    //Initialization of LEDs
    //set to digital output mode
    GPIOB -> MODER |= GPIO_MODER_MODER0 | GPIO_MODER_MODER1 | GPIO_MODER_MODER2 | GPIO_MODER_MODER3 |
                            GPIO_MODER_MODER4 | GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7;
    GPIOB -> MODER &= ~(GPIO_MODER_MODER0_1 | GPIO_MODER_MODER1_1 | GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1 |
                            GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);
    //set default LED states
    GPIOB -> ODR &= ~(ALL_LED_ON);
    GPIOB -> ODR |= GPIO_ODR_0;
}

void Delay(void)
{
    for(volatile int i = 0; i < DELAY0; i++)
        for(volatile int j = 0; j < DELAY1; j++);

}

void display(void)
{
    //clear contents of screen
    lcd_command(CLEAR);
    lcd_command(CURSOR_HOME);

    if(startFlag == FALSE && upFlag == FALSE && downFlag == FALSE)
    {
        //output to screen
        lcd_putstring("EEE2046F Prac3B");
        lcd_command(LINE_TWO);
        lcd_putstring("GIFT MUGWENI");

        GPIOB -> ODR |= GPIO_ODR_0 | GPIO_ODR_7;
    }
    else if(startFlag == TRUE && upFlag == FALSE && downFlag == FALSE)
    {
        lcd_putstring("Binary Counter");
        lcd_command(LINE_TWO);
        lcd_putstring("SW1:Up SW2:Down");

        GPIOB -> ODR &= ~(ALL_LED_ON);
        count = 0;
    }
    else if(startFlag == TRUE && upFlag == TRUE && downFlag == FALSE)
    {
        //output to screen
                lcd_putstring("Counter [U]:");
                lcd_command(LINE_TWO);
                lcd_putstring("count = ");

                convert2BCDASCII(count,countASCII);
                lcd_putstring(countASCII);

                GPIOB -> ODR &= ~(ALL_LED_ON);
                GPIOB -> ODR |= (uint16_t)count;
    }
    else if(startFlag == TRUE && upFlag == FALSE && downFlag == TRUE)
    {
        //output to screen
                lcd_putstring("Counter [D]:");
                lcd_command(LINE_TWO);
                lcd_putstring("count = ");

                convert2BCDASCII(count,countASCII);
                lcd_putstring(countASCII);

                GPIOB -> ODR &= ~(ALL_LED_ON);
                GPIOB -> ODR |= (uint16_t)count;
    }

}

void checkPB(void)
{
    //check if buttons are pressed
    uint8_t sw0 = GPIOA -> IDR & GPIO_IDR_0;
    uint8_t sw1 = GPIOA -> IDR & GPIO_IDR_1;
    uint8_t sw2 = GPIOA -> IDR & GPIO_IDR_2;

    //sw0 is pressed
    if(!sw0)
    {
        startFlag = TRUE;
        upFlag = FALSE;
        downFlag = FALSE;

        GPIOA -> IDR |= GPIO_IDR_0;
    }
    //sw1 is pressed
    else if(!sw1)
    {
        startFlag = TRUE;
        upFlag = TRUE;
        downFlag = FALSE;

        GPIOA -> IDR |= GPIO_IDR_1;
    }
    //sw2 is pressed
    else if(!sw2)
    {
        startFlag = TRUE;
        upFlag = FALSE;
        downFlag = TRUE;

        GPIOA -> IDR |= GPIO_IDR_2;
    }
}

void counter(void)
{
    if(startFlag == TRUE && upFlag == TRUE && downFlag == FALSE)
    {
        if(count <= MAX_COUNT)
        {
            count++;
        }
        else
        {
            count = 0;
        }
    }
    else if(startFlag == TRUE && upFlag == FALSE && downFlag == TRUE)
    {
        if(count >= MIN_COUNT)
        {
            count--;
        }
        else
        {
            count = MAX_COUNT;
        }
    }
}

void convert2BCDASCII(const uint8_t countValue, char* resultPtr)
{
    //temporary storage variables
    int arraySize = 3;
    uint8_t tempValue = countValue;
    char cointainer[] = {'0', '0', '0', '\0'};
    int tempContainer[arraySize];

    for(int i = 0, j = arraySize - 1 ; i < arraySize; i++,j--)
    {
        //convert to Binary coded decimal
        int power = pow(10,i + 1);
        tempContainer[i] = tempValue % power;
        tempValue -= tempContainer[i];

        //convert to ASCII
        cointainer[j] = (tempContainer[i] / (power/10)) + '0';
    }

    //make pointer point to array
    for(int i = 0; i < arraySize + 1; i++)
    {
        *(resultPtr + i) = cointainer[i];
    }
}
//********************************************************************
// END OF PROGRAM
//********************************************************************
