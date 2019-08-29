#ifndef _CHRDEV_H_
#define _CHRDEV_H_

typedef struct led_node
{
	int which;
	int status;
}led_node_t;

#define LED_MAGIC 'w'
#define LEDON  _IOW(LED_MAGIC, 0, struct led_node)
#define LEDOFF _IOW(LED_MAGIC, 1, struct led_node)

#endif /* chrdev.h */
