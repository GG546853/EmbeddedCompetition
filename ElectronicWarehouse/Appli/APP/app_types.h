#ifndef __APP_TYPES_H
#define __APP_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    char pa[32];
    char pc[32];
    char name[48];
    char type[64];
    uint16_t quantity;
    bool judgment_out;
} InventoryItem;


typedef struct {
	bool ESP32_State;
	bool WiFi_State;
	bool MQTT_State;
	bool System_state;
	bool VL53L1X_state;
	uint16_t distance_mm;
	uint16_t distance_mm_th;
	bool Aht10_state;
    float temperature;
    float humidity;
    float temperature_th;
    float humidity_th;
    bool Scan_state;
    int Count;
    char User[32];
} SystemState;


typedef struct {
    char time[32];
    char action[8];
    char pc[32];
    char user[32];
    uint8_t cabinet_id;
    uint16_t quantity;
} HistoryRecord;

typedef struct{
	char pc[32];
	uint8_t cabinet_id;
	uint16_t quantity;
}AlarmRecord;



extern SystemState Cabinet;

#endif
