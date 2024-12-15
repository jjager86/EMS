#ifndef QUEUE_DEFINITIONS_H
#define QUEUE_DEFINITIONS_H

struct MQTT_HA_ConfigMessage {
char obj_name[24];  // Adjust the size based on your needs
  char mqtt_name[24];
  char mqtt_unit[4];
  char mqtt_class[18];
  char mqtt_unique_id[48];
};


static QueueHandle_t MeterDataQueue;
static QueueHandle_t InverterDataQueue;
static QueueHandle_t AutoConfigQueue123;

#endif  // QUEUE_DEFINITIONS_H