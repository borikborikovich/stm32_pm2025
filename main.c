#include <stm32f10x.h>


volatile uint16_t speeds[] = {700, 550, 400, 250, 100};
volatile uint8_t speed_index = 0;
volatile uint8_t button_state = 0;




void init_gpio(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;


    GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
    GPIOC->CRH |= GPIO_CRH_MODE13_1;


    GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0);
    GPIOA->CRL |= GPIO_CRL_CNF0_1;
    GPIOA->ODR |= GPIO_ODR_ODR0;
}

uint8_t is_button_pressed(void) {
    if ((GPIOA->IDR & GPIO_IDR_IDR0) == 0) {

        for (volatile int i = 0; i < 80000; i++); 

        if ((GPIOA->IDR & GPIO_IDR_IDR0) == 0) {
            if (!button_state) {
                button_state = 1;
                return 1;
            }
        }
    }
    else {
        button_state = 0;
    }



    return 0;
}

void init_timer(void) {

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC = 7200 - 1;
    TIM2->ARR = speeds[speed_index] * 10 - 1;
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->CR1 |= TIM_CR1_CEN;

    NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;
        GPIOC->ODR ^= GPIO_ODR_ODR13; 
    }
}





int main(void) {
    init_gpio();
    init_timer();

    while (1) {
        if (is_button_pressed()) {
            speed_index = (speed_index + 1) % 5;
            TIM2->ARR = speeds[speed_index] * 10 - 1;
        }
    }
}