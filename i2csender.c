#include "i2csender.h"

void i2c_send(i2cSender* i2c_sender) {
    furi_hal_i2c_acquire(I2C_BUS);
    uint8_t adress = i2c_sender->scanner->addresses[i2c_sender->address_idx] << 1;

if(i2c_sender->scd4xStatus == scd4xStatus_STOPPED)
{
// start_periodic_measurement
    i2c_sender->value = 0xb121;
i2c_sender->error = furi_hal_i2c_trx(
        I2C_BUS,
        adress,
        (uint8_t*)&i2c_sender->value,
        sizeof(i2c_sender->value),
        i2c_sender->recv,
        sizeof(i2c_sender->recv),
        I2C_TIMEOUT);
         FURI_LOG_I("TEST", "i2c start err: %d", i2c_sender->error);
        FURI_LOG_I("TEST", "i2c start periodic measurement");

        if(i2c_sender->error)
        {
   i2c_sender->scd4xStatus = scd4xStatus_RUNNING;
        }
         
}
else if(i2c_sender->scd4xStatus == scd4xStatus_RUNNING)

{
    //read_measurement
    i2c_sender->value = 0x05ec;

i2c_sender->error = furi_hal_i2c_trx(
        I2C_BUS,
        adress,
        (uint8_t*)&i2c_sender->value,
        sizeof(i2c_sender->value),
        i2c_sender->recv,
        sizeof(i2c_sender->recv),
        I2C_TIMEOUT);


               
i2c_sender->co2_ppm = ((uint16_t)i2c_sender->recv[0]<<8)+i2c_sender->recv[1];
i2c_sender->temperature = -45 + 175 * (   (double)(  ((uint16_t)i2c_sender->recv[3]<<8)+i2c_sender->recv[4]   )/(2<<15)   );
i2c_sender->humidity = 100 * (   (double)(  ((uint16_t)i2c_sender->recv[6]<<8)+i2c_sender->recv[7]   )/(2<<15)   );

//TBD: check CRC
//TBD: handle errors on trx

    FURI_LOG_I("TEST", "i2c trx: %d", i2c_sender->error);
    FURI_LOG_I("TEST", "sizeof tx: %d", sizeof(i2c_sender->value));
    FURI_LOG_I("TEST", "sizeof rx: %d", sizeof(i2c_sender->recv));
    FURI_LOG_I("TEST", "tx: %02x %02x", ((uint8_t*)&i2c_sender->value)[0], ((uint8_t*)&i2c_sender->value)[1]);
    FURI_LOG_I("TEST", "co2: %d", i2c_sender->co2_ppm);
    FURI_LOG_I("TEST", "T: %f", i2c_sender->temperature);
    FURI_LOG_I("TEST", "T raw: %d",(  ((uint16_t)i2c_sender->recv[3]<<8)+i2c_sender->recv[4]   ));
    FURI_LOG_I("TEST", "T raw2: %f",(   (double)(  ((uint16_t)i2c_sender->recv[3]<<8)+i2c_sender->recv[4]   )/(2<<15)   ));
    for (int i =0; i<3;i++)
    {
        FURI_LOG_I("TEST", "rx: %02x %02x %02x", i2c_sender->recv[3*i+0], i2c_sender->recv[3*i+1], i2c_sender->recv[3*i+2]);

    }
}
else{
            FURI_LOG_I("TEST", "i2c ERROR THIS SHOULD NOT HAPPEN - unhandled state");

}
    
 
    furi_hal_i2c_release(I2C_BUS);
    i2c_sender->must_send = false;
    i2c_sender->sended = true;
}

i2cSender* i2c_sender_alloc() {
    i2cSender* i2c_sender = malloc(sizeof(i2cSender));
    i2c_sender->must_send = false;
    i2c_sender->sended = false;
    return i2c_sender;
}

void i2c_sender_free(i2cSender* i2c_sender) {
    furi_assert(i2c_sender);
    free(i2c_sender);
}