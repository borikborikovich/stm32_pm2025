#include "stm32f10x.h"
#include <stdint.h>

#define SSD1306_CS_LOW()   (GPIOA->BSRR = GPIO_BSRR_BR4)
#define SSD1306_CS_HIGH()  (GPIOA->BSRR = GPIO_BSRR_BS4)

#define SSD1306_DC_COMMAND() (GPIOA->BSRR = GPIO_BSRR_BR3)
#define SSD1306_DC_DATA()    (GPIOA->BSRR = GPIO_BSRR_BS3)

#define SSD1306_RST_LOW()  (GPIOA->BSRR = GPIO_BSRR_BR2)
#define SSD1306_RST_HIGH() (GPIOA->BSRR = GPIO_BSRR_BS2)




void delay_ms(uint32_t ms) {
    for(uint32_t i = 0; i < ms * 8000; i++) {
        __NOP();
    }
}

void SPI1_SendByte(uint8_t data) {
    while (!(SPI1->SR & SPI_SR_TXE));
    SPI1->DR = data;
    while (!(SPI1->SR & SPI_SR_RXNE));
    volatile uint8_t dummy = SPI1->DR;
    (void)dummy;
}


void SSD1306_SendData(uint8_t data) {
    SSD1306_DC_DATA();
    SSD1306_CS_LOW();
    SPI1_SendByte(data);
    SSD1306_CS_HIGH();
}


void SSD1306_SendCommand(uint8_t cmd) {
    SSD1306_DC_COMMAND();
    SSD1306_CS_LOW();
    SPI1_SendByte(cmd);
    SSD1306_CS_HIGH();
}



void SSD1306_Init(void) {
    SSD1306_RST_LOW();
    delay_ms(20);
    SSD1306_RST_HIGH();

    SSD1306_SendCommand(0xAE); 
    SSD1306_SendCommand(0x20); 
    SSD1306_SendCommand(0x00); 

    SSD1306_SendCommand(0xB0); 
    SSD1306_SendCommand(0xC8); 

    SSD1306_SendCommand(0x00); 
    SSD1306_SendCommand(0x10); 

    SSD1306_SendCommand(0x40); 
    SSD1306_SendCommand(0x81);
    SSD1306_SendCommand(0x7F);

    SSD1306_SendCommand(0xA1); 
    SSD1306_SendCommand(0xA6); 

    SSD1306_SendCommand(0xA8); 
    SSD1306_SendCommand(0x3F);

    SSD1306_SendCommand(0xA4); 
    SSD1306_SendCommand(0xD3); 
    SSD1306_SendCommand(0x00);

    SSD1306_SendCommand(0xD5); 
    SSD1306_SendCommand(0xF0);

    SSD1306_SendCommand(0xD9); 
    SSD1306_SendCommand(0x22);

    SSD1306_SendCommand(0xDA); 
    SSD1306_SendCommand(0x12);

    SSD1306_SendCommand(0xDB); 
    SSD1306_SendCommand(0x20);

    SSD1306_SendCommand(0x8D); 
    SSD1306_SendCommand(0x14);

    SSD1306_SendCommand(0xAF); 
}

void SSD1306_TestPattern(void) {

    for (uint8_t page = 0; page < 8; page++) {

        SSD1306_SendCommand(0xB0 + page);
        SSD1306_SendCommand(0x00);
        SSD1306_SendCommand(0x10);


        uint8_t y_zone = page & 1;  

        for (uint8_t col = 0; col < 128; col++) {

            uint8_t pixel;

            if (col < 32 || col >= 96) {
                pixel = 0x00;
            }
            else {
                uint8_t x_zone = (col - 32) / 8;  

                if ((x_zone ^ y_zone) & 1)
                    pixel = 0xFF;
                else
                    pixel = 0x00;
            }




            SSD1306_SendData(pixel);
        }
    }
}

void SPI1_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_SPI1EN;

    GPIOA->CRL &= ~(
        GPIO_CRL_CNF5 | GPIO_CRL_MODE5 |
        GPIO_CRL_CNF7 | GPIO_CRL_MODE7);


    GPIOA->CRL |=
        GPIO_CRL_MODE5_1 | GPIO_CRL_MODE5_0 | GPIO_CRL_CNF5_1 |
        GPIO_CRL_MODE7_1 | GPIO_CRL_MODE7_0 | GPIO_CRL_CNF7_1;


    GPIOA->CRL &= ~(GPIO_CRL_CNF4 | GPIO_CRL_MODE4 |
                    GPIO_CRL_CNF3 | GPIO_CRL_MODE3 |
                    GPIO_CRL_CNF2 | GPIO_CRL_MODE2);


    GPIOA->CRL |=
        GPIO_CRL_MODE4_0 | GPIO_CRL_MODE4_1 |
        GPIO_CRL_MODE3_0 | GPIO_CRL_MODE3_1 |
        GPIO_CRL_MODE2_0 | GPIO_CRL_MODE2_1;




    SSD1306_CS_HIGH();

    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_BR_1 | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE;
}

int main(void) {
    SPI1_Init();
    SSD1306_Init();
    SSD1306_TestPattern();
}