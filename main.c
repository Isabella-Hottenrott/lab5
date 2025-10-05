/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : main.c
Purpose : Generic application start

*/

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

/*********************************************************************
*
*       main()
*
*  Function description
*   Application entry point.
*/

int main(void) {
  long long ref = 0;
  long pulses = 0;
  long velocity=0;
  int encoding=0;


  //config gpio
  gpioEnable(GPIO_PORT_A);
  pinMode(ENCODER_A, GPIO_INPUT);
  pinMode(ENCODER_B, GPIO_INPUT);

  //config timers
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
  initTIM(UPDATE_TIM);
  RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
  initTIM(REPORT_TIM);

  //config interrupts for gpio
  gpio_interrupt();

  int prev=0;
  int curr=0;

  while(1){
    delay_millis(REPORT_TIM, 1000);
      while(!(REPORT_TIM->SR & 1)){

        delay_millis(UPDATE_TIM, 100);
          while(!(UPDATE_TIM->SR & 1)){// Wait for UIF to go high

            if(flagA|flagB){
              printf("flagged");
             // if B = ~A -> CW, if  B == A -> CCW, 
              curr = ((flagA<<1) | flagB);
              encoding = case_prev_curr(curr, prev);

              prev = curr;
              curr = 0;
              // need ppr conversion
              pulses += encoding;

              //PPR = 120
              velocity = pulses/(10*120);
            }

          }
          UPDATE_TIM->SR &= ~(0x1); // Clear UIF flag
          printf("velocity = %d", velocity); 

        }
        REPORT_TIM->SR &= ~(0x1); // Clear UIF flag


  }
}


int case_prev_curr(int curr, int prev){
  switch(curr) {
        case 0:
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
                                printf("error1");
                                break;
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
    // 2. Enable rising edge trigger
    EXTI->RTSR1 |= (1 << gpioPinOffset(ENCODER_A));// Enable rising edge trigger
    EXTI->RTSR1 |= (1 << gpioPinOffset(ENCODER_B));// Enable rising edge trigger
    // 3. Enable falling edge trigger
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

        
        // Read the two bits of both GPIO pins
        // Add to fn
    }
}

void EXTI3_IRQHandler(void){
    // Check that Encoder B was what triggered our interrupt
    if (EXTI->PR1 & (1 << ENCODER_B)){
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 |= (1 << ENCODER_B);
        flagB = true;

        // Read the two bits of both GPIO pins
        // Add to fn

    }

}




/*************************** End of file ****************************/
