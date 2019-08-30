#ifndef _LED_H_
#define _LED_H_

#define LED_MAGIC 'f'
#define LEDON  _IOW(LED_MAGIC, 0, int)
#define LEDOFF _IOW(LED_MAGIC, 1, int)

#endif /* led.h */
