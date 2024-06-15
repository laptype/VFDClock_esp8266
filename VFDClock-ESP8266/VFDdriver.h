#ifndef VFDDRIVER_H
#define VFDDRIVER_H

#include <Arduino.h>

#define DIN   12  // DA
#define CLK   14  // CK
#define CS    4   // CS
#define RESET 1   // RS
#define EN    3   // EN 0

extern uint8_t din; // DA
extern uint8_t clk; // CK
extern uint8_t cs; // CS
extern uint8_t Reset; // RS
extern uint8_t en; // EN 0

void spi_write_data(unsigned char w_data);
void VFD_cmd(unsigned char command);
void VFD_show(void);
void VFD_init(void);
void VFD_WriteStr(unsigned char x, char *str);
void VFD_WriteUserFont(unsigned char x, unsigned char y, unsigned char *s);
void VFD_enable(bool enable);
void VFD_setBrightness(int brightness);
#endif
