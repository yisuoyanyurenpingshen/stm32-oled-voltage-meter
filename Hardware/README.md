本文件夹用于存放接线说明、原理图、PCB 文件和硬件设计资料。
使用的OLED为SSD1306 地址为0x3C 在hal库里面写0x78 但是可以写成 #define OLED_ADDR (0x3C <<1 )
