#include "STM32L432KC.h"
#include <stm32l432xx.h>
#define USART_ID USART2_ID
#define TIM_SEND TIM15

int send_message(void) {

// Initialize USART
USART_TypeDef * USART = initUSART(USART_ID, 9600);

// Initialize timer
RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
initTIM(TIM_SEND);

char msg[28] = "Happy Hacking!\n\r";


while(1){
  int i = 0;
  do {
    sendChar(USART, msg[i]);
    i += 1;
  } while (msg[i]);
  delay_millis(TIM_SEND, 2000);
  }
}