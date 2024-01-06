#pragma once

#include <furi.h>
#include <furi_hal.h>
#include "i2cscanner.h"

typedef enum {
    scd4xStatus_STOPPED,
    scd4xStatus_RUNNING
} scd4xStatusType;

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
 scd4xStatusType scd4xStatus;

    i2cScanner* scanner;
} i2cSender;

void i2c_send();

i2cSender* i2c_sender_alloc();
void i2c_sender_free(i2cSender* i2c_sender);
