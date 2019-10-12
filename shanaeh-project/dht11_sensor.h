#ifndef DHT11_SENSOR_H
#define DHT11_SENSOR_H


void dht11_init(void);

unsigned int dht11_read(int dht11_data[5]);

#endif
