#include "VFDdriver.h"

uint8_t din = DIN; // DA
uint8_t clk = CLK; // CK
uint8_t cs = CS; // CS
uint8_t Reset = RESET; // RS
uint8_t en = EN; // EN 0
//vfd时钟驱动部分
void spi_write_data(unsigned char w_data)
{
  unsigned char i;
  for (i = 0; i < 8; i++)
  {
    digitalWrite(clk, LOW);
    if ((w_data & 0x01) == 0x01)
    {
      digitalWrite(din, HIGH);
    }
    else
    {
      digitalWrite(din, LOW);
    }
    w_data >>= 1;
    digitalWrite(clk, HIGH);
  }
}

void VFD_cmd(unsigned char command)
{
  digitalWrite(cs, LOW);
  spi_write_data(command);
  digitalWrite(cs, HIGH);
  delayMicroseconds(5);
}

void VFD_show(void)
{
  digitalWrite(cs, LOW);
  spi_write_data(0xe8); // 开显示命令
  digitalWrite(cs, HIGH);
}
/******************************
  VFD 亮度 0 - 255 
*******************************/
void VFD_setBrightness(int brightness) {
  // 0 - 255
  digitalWrite(cs, LOW);
  spi_write_data(0xe4);
  delayMicroseconds(5);
  spi_write_data(brightness); // 亮度
  digitalWrite(cs, HIGH);
  delayMicroseconds(5);
}

void VFD_init()
{
  digitalWrite(cs, LOW);
  spi_write_data(0xe0);
  delayMicroseconds(5);
  spi_write_data(0x07);   // 8 digtal 0x07
  digitalWrite(cs, HIGH);
  delayMicroseconds(5);
}
/******************************
  在指定位置打印字符串
  (仅适用于英文,标点,数字)
  x:0~11;str:要显示的字符串
*******************************/
void VFD_WriteStr(unsigned char x, char *str)
{
  digitalWrite(cs, LOW);
  spi_write_data(0x20 + x);   // 地址寄存器起始位置
  while (*str)
  {
    spi_write_data(*str);
    str++;
  }
  digitalWrite(cs, HIGH);
  VFD_show();
}
/******************************
  在指定位置打印自定义字符
  
  x:位置，有ROOM位置;*s:要显示的字符字模
*******************************/
void VFD_WriteUserFont(unsigned char x, unsigned char y,unsigned char *s)
{
  unsigned char i = 0;

  // 设置显示位置
  digitalWrite(cs, LOW);
  spi_write_data(0x20 + x); // 行地址
  spi_write_data(0x00 + y); // 列地址
  digitalWrite(cs, HIGH);

  // 设置自定义字符的地址
  digitalWrite(cs, LOW);
  spi_write_data(0x40 + y); // 地址寄存器起始位置
  for (i = 0; i < 5; i++)
  {
    spi_write_data(s[i]);
  }
  digitalWrite(cs, HIGH);

  VFD_show();

}
/******************************
  VFD 开关  
*******************************/
void VFD_enable(bool enable) {
  if (enable) {
    digitalWrite(en, HIGH);    
  } else {
    digitalWrite(en, LOW);    
  }
  delayMicroseconds(100);
}

