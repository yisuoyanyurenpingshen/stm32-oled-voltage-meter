# OLED I2C 调试记录

## 问题现象

在 STM32 电压测量小程序调试过程中，OLED 一开始没有正常显示内容。

当时的现象包括：

- 程序可以正常编译
- STM32 可以正常下载程序
- OLED 没有显示预期内容
- 不确定是代码问题、I2C 地址问题，还是硬件供电问题

---

## 初步判断

OLED 使用 I2C 通信，常见问题通常包括：

1. OLED 没有正常供电
2. GND 没有和 STM32 共地
3. SCL / SDA 接反
4. CubeMX 中 I2C 引脚配置错误
5. OLED I2C 地址不正确
6. OLED 驱动代码中的地址格式和 HAL 函数要求不一致

因此不能一开始就盲目修改 OLED 驱动代码，而是先确认硬件连接和 I2C 地址。

---

## I2C 地址扫描代码

为了确认 STM32 是否能检测到 OLED，我使用了 I2C 地址扫描代码。

### 全局变量

```c
/* USER CODE BEGIN 0 */

volatile uint8_t found_addr = 0;      // 扫描到的 I2C 地址
volatile uint8_t scan_addr = 0;       // 当前正在扫描的地址
volatile uint8_t i2c_scan_done = 0;   // 是否扫描完成

/* USER CODE END 0 */
```

这里使用 `volatile`，是为了方便在 Keil Debug 的 Watch 窗口中观察变量，避免变量被编译器优化掉。

---

### while(1) 中的扫描代码

```c
/* USER CODE BEGIN WHILE */
while (1)
{
    if (i2c_scan_done == 0)
    {
        found_addr = 0;

        for (scan_addr = 1; scan_addr < 127; scan_addr++)
        {
            if (HAL_I2C_IsDeviceReady(&hi2c1, scan_addr << 1, 2, 10) == HAL_OK)
            {
                found_addr = scan_addr;
                break;
            }
        }

        i2c_scan_done = 1;
    }

    HAL_Delay(500);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
}
/* USER CODE END 3 */
```

---

## Debug 观察方法

进入 Keil Debug 后，打开：

```text
View -> Watch Windows -> Watch 1
```

在 Watch 窗口中添加以下变量：

```text
found_addr
scan_addr
i2c_scan_done
```

然后运行程序，观察变量值。

---

## 扫描结果判断

### 情况 1：成功扫描到 OLED

如果 Watch 中看到：

```text
found_addr = 0x3C
scan_addr = 0x3C
i2c_scan_done = 1
```

说明 STM32 已经成功检测到 OLED。

OLED 常见 7 位地址为：

```text
0x3C
0x3D
```

---

### 情况 2：没有扫描到设备

如果 Watch 中看到：

```text
found_addr = 0x00
scan_addr = 0x7F
i2c_scan_done = 1
```

说明从地址 1 扫描到 126，都没有 I2C 设备响应。

这时优先检查：

1. OLED VCC 是否有电
2. OLED GND 是否和 STM32 GND 共地
3. SCL / SDA 是否接反
4. CubeMX 中 I2C 是否配置正确
5. 实际接线是否和 CubeMX 引脚一致
6. I2C 速度是否过高，可以先使用 100kHz

---

## 使用万用表排查供电问题

在本次调试中，我还使用万用表测量 OLED 的 VCC 和 GND 之间的电压。

测量方法：

```text
红表笔接 OLED VCC
黑表笔接 OLED GND
万用表打到直流电压档
```

通过测量发现 OLED 一开始没有正常供电，因此 OLED 不显示并不一定是代码错误，也可能是硬件供电问题。

这一步说明，在嵌入式调试中，不能只盯着代码，也要检查硬件电压。

---

## 关于 I2C 地址左移一位

STM32 HAL 的 I2C 函数通常需要传入左移一位后的地址。

例如 OLED 的 7 位地址是：

```text
0x3C
```

传给 HAL 函数时通常写成：

```c
0x3C << 1
```

也就是：

```text
0x78
```

因此，有些教程写 OLED 地址是 `0x3C`，有些代码里写 `0x78`，它们可能指的是同一个设备，只是地址格式不同。

---

## 本次调试收获

通过这次调试，我理解到：

1. OLED 不显示不一定是代码问题，也可能是供电或接线问题。
2. I2C 模块接入后，应该先用扫描代码确认设备是否在线。
3. Debug + Watch 窗口可以观察程序变量，帮助判断程序运行状态。
4. 万用表是嵌入式硬件调试中非常重要的工具。
5. STM32 HAL 的 I2C 地址通常需要左移一位，这和很多教程中的 7 位地址写法不同。

---

## 后续改进

后续在接入新的 I2C 模块时，例如：

- INA219 电流检测模块
- AHT20 温湿度传感器
- BMP280 气压传感器
- AT24C02 EEPROM

都会先进行以下步骤：

1. 检查 VCC 和 GND
2. 检查是否共地
3. 检查 SCL / SDA 接线
4. 使用 I2C 扫描代码确认设备地址
5. 再开始移植驱动代码
