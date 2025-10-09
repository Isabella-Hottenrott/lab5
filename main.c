// Isabella Hottenrott
// ihottenrott@g.hmc.edu
// main.c for Lab5_ih
// 9/10/2025

#include <stdio.h>
#include "main.h"

///Function Declarations:
int case_prev_curr(int prev, int curr);
void gpio_interrupt(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
///

volatile bool flagA = false;
volatile bool flagB = false;
volatile bool A = false;
volatile bool B = false;
volatile long long ref = 0;
volatile double pulses = 0;
volatile double velocity=0;
volatile int encoding=0;
volatile int prev = 0;
volatile int curr = 0;


/// Main.c Begin

int main(void) {

  
  //config gpio
  gpioEnable(GPIO_PORT_A);
  pinMode(ENCODER_A, GPIO_INPUT);
  pinMode(ENCODER_B, GPIO_INPUT);

  //config timers
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
  initTIM(UPDATE_TIM);
  RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
  initTIM(REPORT_TIM);

  
  gpio_interrupt(); //config interrupts for gpio


  while(1){
    delay_millis(REPORT_TIM, 1000);
      while(!(REPORT_TIM->SR & 1)){

        delay_millis(UPDATE_TIM, 100);
          while(!(UPDATE_TIM->SR & 1)){// Wait for UIF to go high

            if(flagA|flagB){
         //     printf("A= %d\n", flagA);
           //   printf("B= %d\n", flagB);

              flagA=0; // Reset the flags
              flagB=0;

              A = digitalRead(2);  // Read both pins
              B = digitalRead(3);
              curr = ((A<<1) | B); // Create encoding
              A=0; // Reset the variables
              B=0;

              encoding = case_prev_curr(curr, prev);
              prev = curr;
              pulses += encoding; // Add the necessary encoding

              
              velocity = (pulses*10)/(4*408); //PPR = 408, 4 edges detected, 10 samples per second
            }

          }
          UPDATE_TIM->SR &= ~(0x1); // Clear UIF flag
          printf("%.4f \n", velocity);

          pulses = 0; // Clear the velocity and pulses 
          velocity = 0; // For the next reading


        }
        REPORT_TIM->SR &= ~(0x1); // Clear UIF flag


  }
}


int case_prev_curr(int curr, int prev){
  switch(curr) {
        case 0: //Encoding based on the CL produced from Reference Manual
                  switch(prev) {
                            case 0:
                                return 0;
                                break;
                            case 1:
                                return 1;
                                break;
                            case 2:
                                return -1;
                                break;
                            case 3:
                                return 0;
                                break;
                            default:
                                printf("error1"); // For Debug
                                break; // Print error when no encoding exists
                  }
                  break;
        case 1:
                  switch(prev) {
                            case 0:
                                return -1;
                                break;
                            case 1:
                               return 0;
                                break;
                            case 2:
                                return 0;
                                break;
                            case 3:
                                return 1;
                                break;
                            default:
                                printf("error2");
                                break;
                  }
                  break;
        case 2:
                  switch(prev) {
                            case 0:
                                return 1;
                                break;
                            case 1:
                                return 0;
                                break;
                            case 2:
                                return 0;
                                break;
                            case 3:
                                return -1;
                                break;
                            default:
                                printf("error3");
                                break;
                  }
                  break;
        case 3:
                  switch(prev) {
                            case 0:
                                return 0;
                                break;
                            case 1:
                                return -1;
                                break;
                            case 2:
                               return 1;
                                break;
                            case 3:
                                return 0;
                                break;
                            default:
                                printf("error4");
                                break;
                  }
                  break;
        default:
            printf("error5");
            break;
	}
}

void gpio_interrupt(void) {

    // 1. Enable SYSCFG clock domain in RCC
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    // 2. Configure EXTICR for the Encoder A interrupt
    SYSCFG->EXTICR[0] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI2, 0b000); // Select PA2
    // 2. Configure EXTICR for the Encoder B interrupt
    SYSCFG->EXTICR[0] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI3, 0b000); // Select PA3
    // Enable interrupts globally
    __enable_irq();

        // 1. Configure mask bit
    EXTI->IMR1 |= (1 << gpioPinOffset(ENCODER_A)); // Configure the mask bit
    EXTI->IMR1 |= (1 << gpioPinOffset(ENCODER_B)); // Configure the mask bit
    // 2. Enable rising edge triggers
    EXTI->RTSR1 |= (1 << gpioPinOffset(ENCODER_A));// Enable rising edge trigger
    EXTI->RTSR1 |= (1 << gpioPinOffset(ENCODER_B));// Enable rising edge trigger
    // 3. Enable falling edge triggers
    EXTI->FTSR1 |= (1 << gpioPinOffset(ENCODER_A));// Enable falling edge trigger
    EXTI->FTSR1 |= (1 << gpioPinOffset(ENCODER_B));// Enable falling edge trigger
 
    // 4. Turn on EXTI interrupt in NVIC_ISER
    NVIC->ISER[0] |= (1 << EXTI2_IRQn);
    NVIC->ISER[0] |= (1 << EXTI3_IRQn);


}


void EXTI2_IRQHandler(void){
    // Check that Encoder A was what triggered our interrupt
    if (EXTI->PR1 & (1 << ENCODER_A)){
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 << ENCODER_A);
        flagA = true;
    }
}

void EXTI3_IRQHandler(void){
    // Check that Encoder B was what triggered our interrupt
    if (EXTI->PR1 & (1 << ENCODER_B)){
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 << ENCODER_B);
        flagB = true;
    }

}


