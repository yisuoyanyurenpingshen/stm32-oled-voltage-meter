#include "oled.h"
#include "oledfont.h"
#include "i2c.h"
#include "stm32f1xx_hal.h"

#define OLED_ADDR 0x78

static void OLED_WriteCommand(uint8_t cmd)
{
    uint8_t data[2];

    data[0] = 0x00;
    data[1] = cmd;

    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, data, 2, 100);
}

static void OLED_WriteData(uint8_t dat)
{
    uint8_t data[2];

    data[0] = 0x40;
    data[1] = dat;

    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, data, 2, 100);
}

void OLED_SetCursor(uint8_t page, uint8_t x)
{
    OLED_WriteCommand(0xB0 + page);
    OLED_WriteCommand(0x00 + (x & 0x0F));
    OLED_WriteCommand(0x10 + ((x >> 4) & 0x0F));
}

void OLED_Clear(void)
{
    uint8_t page;
    uint8_t x;

    for (page = 0; page < 8; page++)
    {
        OLED_SetCursor(page, 0);

        for (x = 0; x < 128; x++)
        {
            OLED_WriteData(0x00);
        }
    }
}

void OLED_ShowChar(uint8_t page, uint8_t x, char ch)
{
    uint8_t i;
    uint8_t index;

    if (ch < ' ' || ch > '~')
    {
        ch = ' ';
    }

    index = ch - ' ';

    OLED_SetCursor(page, x);

    for (i = 0; i < 6; i++)
    {
        OLED_WriteData(OLED_F6x8[index][i]);
    }
}

void OLED_ShowString(uint8_t page, uint8_t x, char *str)
{
    while (*str != '\0')
    {
        OLED_ShowChar(page, x, *str);
        x += 6;
        str++;

        if (x > 122)
        {
            x = 0;
            page++;

            if (page > 7)
            {
                break;
            }
        }
    }
}

void OLED_Init(void)
{
    HAL_Delay(100);

    OLED_WriteCommand(0xAE); // Display off

    OLED_WriteCommand(0x20); // Set Memory Addressing Mode
    OLED_WriteCommand(0x02); // Page Addressing Mode

    OLED_WriteCommand(0xB0); // Set Page Start Address

    OLED_WriteCommand(0xC8); // COM Output Scan Direction

    OLED_WriteCommand(0x00); // Low column address
    OLED_WriteCommand(0x10); // High column address

    OLED_WriteCommand(0x40); // Start line address

    OLED_WriteCommand(0x81); // Contrast
    OLED_WriteCommand(0x7F);

    OLED_WriteCommand(0xA1); // Segment remap

    OLED_WriteCommand(0xA6); // Normal display

    OLED_WriteCommand(0xA8); // Multiplex ratio
    OLED_WriteCommand(0x3F);

    OLED_WriteCommand(0xA4); // Display follows RAM

    OLED_WriteCommand(0xD3); // Display offset
    OLED_WriteCommand(0x00);

    OLED_WriteCommand(0xD5); // Display clock
    OLED_WriteCommand(0x80);

    OLED_WriteCommand(0xD9); // Pre-charge period
    OLED_WriteCommand(0xF1);

    OLED_WriteCommand(0xDA); // COM pins
    OLED_WriteCommand(0x12);

    OLED_WriteCommand(0xDB); // VCOMH deselect level
    OLED_WriteCommand(0x40);

    OLED_WriteCommand(0x8D); // Charge pump
    OLED_WriteCommand(0x14);

    OLED_WriteCommand(0xAF); // Display on

    OLED_Clear();
}
