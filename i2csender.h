#pragma once

#include <furi.h>
#include <furi_hal.h>
#include "i2cscanner.h"

void DEV_Digital_Write(GpioPinRecord pin, bool state);

bool DEV_Digital_Read(GpioPinRecord pin);

#define EPD_BUSY_PIN (gpio_pins[7]) //12
#define EPD_RST_PIN (gpio_pins[8]) //13
#define EPD_DC_PIN (gpio_pins[9]) //14
#define EPD_CS_PIN (gpio_pins[10]) //15 alternative CS for debug
#define pinInput1 (gpio_pins[11]) // 16


#define UBYTE uint8_t 
#define UWORD uint16_t 
#define UDOUBLE uint32_t

#define DEV_Delay_ms(__xms) furi_delay_ms(__xms);
#define Debug(__xstring) FURI_LOG_I("DBG", __xstring);
#define printf(__xstring) FURI_LOG_I("DBG", __xstring);


typedef enum {
    scd4xStatus_STOPPED,
    scd4xStatus_RUNNING
} scd4xStatusType;

typedef enum {
    epaperStatus_STOPPED,
    epaperStatus_RUNNING
} epaperStatusType;

typedef struct {
    uint8_t address_idx;
    uint16_t value;
    uint8_t recv[9];
    bool must_send;
    bool sended;
    bool error;
    uint16_t co2_ppm;
    double temperature;
    double humidity;
    bool TrueFalse;
    uint16_t x_offset;
    uint16_t y_offset;
 scd4xStatusType scd4xStatus;
  epaperStatusType epaperStatus;


    i2cScanner* scanner;
} i2cSender;

void i2c_send();

i2cSender* i2c_sender_alloc();
void i2c_sender_free(i2cSender* i2c_sender);
