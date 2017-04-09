#ifndef __GPIO_H__
#define __GPIO_H__
#define ON 1
#define OFF 0

#define GPIO_HIGH 1
#define GPIO_LOW 0
#define GPIO_OUTPUT 1
#define GPIO_INPUT 0


void GPIOInit (void);
void gpio_dir(uint32_t gpio, uint32_t dir);
void gpio_ctrl(uint32_t gpio, uint32_t value);

#endif
