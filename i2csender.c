#include "i2csender.h"


//#define EPD_DC_PIN pinDC
/*#define EPD_CS_PIN pinAltCS
//#define EPD_PWR_PIN TBD ?
#define EPD_RST_PIN EPD_RST_PIN
*/









#include "EPD_4in2b_V2.h"



// Display resolution
#define EPD_4IN2_WIDTH       400
#define EPD_4IN2_HEIGHT      300
#define UBYTE uint8_t 
#define UWORD uint16_t 



/**
 * image color
**/
#define WHITE          0xFF
#define BLACK          0x00
#define RED            BLACK

#define IMAGE_BACKGROUND    WHITE
#define FONT_FOREGROUND     BLACK
#define FONT_BACKGROUND     WHITE


/**
 * Image attributes
**/
typedef struct {
    UBYTE *Image;
    UWORD Width;
    UWORD Height;
    UWORD WidthMemory;
    UWORD HeightMemory;
    UWORD Color;
    UWORD Rotate;
    UWORD Mirror;
    UWORD WidthByte;
    UWORD HeightByte;
    UWORD Scale;
} PAINT;
extern PAINT Paint;

/**
 * Display rotate
**/
#define ROTATE_0            0
#define ROTATE_90           90
#define ROTATE_180          180
#define ROTATE_270          270


/**
 * Display Flip
**/
typedef enum {
    MIRROR_NONE  = 0x00,
    MIRROR_HORIZONTAL = 0x01,
    MIRROR_VERTICAL = 0x02,
    MIRROR_ORIGIN = 0x03,
} MIRROR_IMAGE;
#define MIRROR_IMAGE_DFT MIRROR_NONE






static const unsigned char EPD_4IN2_lut_vcom0[] = {
    0x00, 0x08, 0x08, 0x00, 0x00, 0x02,	
	0x00, 0x0F, 0x0F, 0x00, 0x00, 0x01,	
	0x00, 0x08, 0x08, 0x00, 0x00, 0x02,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 					
	};
static const unsigned char EPD_4IN2_lut_ww[] = {
	0x50, 0x08, 0x08, 0x00, 0x00, 0x02,	
	0x90, 0x0F, 0x0F, 0x00, 0x00, 0x01,	
	0xA0, 0x08, 0x08, 0x00, 0x00, 0x02,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	};
static const unsigned char EPD_4IN2_lut_bw[] = {
	0x50, 0x08, 0x08, 0x00, 0x00, 0x02,	
	0x90, 0x0F, 0x0F, 0x00, 0x00, 0x01,	
	0xA0, 0x08, 0x08, 0x00, 0x00, 0x02,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	};
static const unsigned char EPD_4IN2_lut_wb[] = {
	0xA0, 0x08, 0x08, 0x00, 0x00, 0x02,	
	0x90, 0x0F, 0x0F, 0x00, 0x00, 0x01,	
	0x50, 0x08, 0x08, 0x00, 0x00, 0x02,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	};
static const unsigned char EPD_4IN2_lut_bb[] = {
	0x20, 0x08, 0x08, 0x00, 0x00, 0x02,	
	0x90, 0x0F, 0x0F, 0x00, 0x00, 0x01,	
	0x10, 0x08, 0x08, 0x00, 0x00, 0x02,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	};












void DEV_Digital_Write(GpioPinRecord pin, bool state)
{
      FURI_LOG_D("DO Write", "%d on pin %s number %d", state, pin.name, pin.number);
      furi_hal_gpio_write(pin.pin, state);
}

bool DEV_Digital_Read(GpioPinRecord pin)
{
    bool state;
    state = furi_hal_gpio_read(pin.pin);
    FURI_LOG_I("DO Read", "%d on pin %s number %d", state, pin.name, pin.number);
    return state;
}

bool DEV_SPI_WriteByte(uint8_t reg)
{
    bool result;
   //     uint8_t buf[1];
      //  buf = {};
        result = furi_hal_spi_bus_tx(
               &furi_hal_spi_bus_handle_external, &reg, 1, 50);
              FURI_LOG_D("TEST", "DEV_SPI_WriteByte: result=%d reg=%02x", result, reg);

              return result;
}
/* 
int DEV_Module_Init(void)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
	//DEV_Digital_Write(EPD_PWR_PIN, 1);
    DEV_Digital_Write(EPD_RST_PIN, 1);
		return 0;
}

 */
/* void DEV_Delay_ms(uint32_t time_ms)
{
    furi_delay_ms(time_ms);
} */




/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void EPD_4IN2_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void EPD_4IN2_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
void EPD_4IN2_ReadBusy(void)
{
    FURI_LOG_I("EPAPER", "e-Paper busy");
	EPD_4IN2_SendCommand(0x71);
    while(DEV_Digital_Read(EPD_BUSY_PIN) == 0) {      //LOW: idle, HIGH: busy
		EPD_4IN2_SendCommand(0x71);
        DEV_Delay_ms(100);
    }
    FURI_LOG_I("EPAPER", "e-Paper busy release");
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
static void EPD_4IN2_TurnOnDisplay(void)
{
    EPD_4IN2_SendCommand(0x12);
    DEV_Delay_ms(100);
    EPD_4IN2_ReadBusy();
}

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
static void EPD_4IN2_Reset(void)
{
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(10);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(10);

    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(10);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(10);
	
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(10);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(10);
}

/******************************************************************************
function :	set the look-up tables
parameter:
******************************************************************************/
static void EPD_4IN2_SetLut(void)
{
	unsigned int count;
	EPD_4IN2_SendCommand(0x20);
	for(count=0;count<36;count++)	     
		{EPD_4IN2_SendData(EPD_4IN2_lut_vcom0[count]);}

	EPD_4IN2_SendCommand(0x21);
	for(count=0;count<36;count++)	     
		{EPD_4IN2_SendData(EPD_4IN2_lut_ww[count]);}   
	
	EPD_4IN2_SendCommand(0x22);
	for(count=0;count<36;count++)	     
		{EPD_4IN2_SendData(EPD_4IN2_lut_bw[count]);} 

	EPD_4IN2_SendCommand(0x23);
	for(count=0;count<36;count++)	     
		{EPD_4IN2_SendData(EPD_4IN2_lut_wb[count]);} 

	EPD_4IN2_SendCommand(0x24);
	for(count=0;count<36;count++)	     
		{EPD_4IN2_SendData(EPD_4IN2_lut_bb[count]);}   
}


//UC8176C
void EPD_4IN2_Init_Fast(void)
{
	EPD_4IN2_Reset();
	EPD_4IN2_SendCommand(0x01);			//POWER SETTING 
	EPD_4IN2_SendData (0x03);	          
	EPD_4IN2_SendData (0x00);
	EPD_4IN2_SendData (0x2b);  
	EPD_4IN2_SendData (0x2b);

	EPD_4IN2_SendCommand(0x06);         //boost soft start
	EPD_4IN2_SendData (0x17);		//A
	EPD_4IN2_SendData (0x17);		//B
	EPD_4IN2_SendData (0x17);		//C       

	EPD_4IN2_SendCommand(0x04);  
	EPD_4IN2_ReadBusy();

	EPD_4IN2_SendCommand(0x00);			//panel setting
	EPD_4IN2_SendData(0xbf);		//KW-bf   KWR-2F	BWROTP 0f	BWOTP 1f


	EPD_4IN2_SendCommand(0x30);			
	EPD_4IN2_SendData (0x3c);      	// 3A 100HZ   29 150Hz 39 200HZ	31 171HZ

	EPD_4IN2_SendCommand(0x61);			//resolution setting
	EPD_4IN2_SendData (0x01);        	 
	EPD_4IN2_SendData (0x90);	 //400	
	EPD_4IN2_SendData (0x01);	 //300
	EPD_4IN2_SendData (0x2c);	   


	EPD_4IN2_SendCommand(0x82);			//vcom_DC setting  	
	EPD_4IN2_SendData (0x12);	

	EPD_4IN2_SendCommand(0X50);
	EPD_4IN2_SendData(0x97);

	EPD_4IN2_SetLut();		
		
}	
/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_4IN2_Clear(void)
{
    UWORD Width, Height;
    Width = (EPD_4IN2_WIDTH % 8 == 0)? (EPD_4IN2_WIDTH / 8 ): (EPD_4IN2_WIDTH / 8 + 1);
    Height = EPD_4IN2_HEIGHT;

    EPD_4IN2_SendCommand(0x10);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_4IN2_SendData(0xFF);
        }
    }

    EPD_4IN2_SendCommand(0x13);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_4IN2_SendData(0xFF);
        }
    }

	EPD_4IN2_SendCommand(0x12);		 //DISPLAY REFRESH 		
	DEV_Delay_ms(1);	
    EPD_4IN2_TurnOnDisplay();
}


PAINT Paint;

/******************************************************************************
function: Create Image
parameter:
    image   :   Pointer to the image cache
    width   :   The width of the picture
    Height  :   The height of the picture
    Color   :   Whether the picture is inverted
******************************************************************************/
void Paint_NewImage(UBYTE *image, UWORD Width, UWORD Height, UWORD Rotate, UWORD Color)
{
    Paint.Image = NULL;
    Paint.Image = image;

    Paint.WidthMemory = Width;
    Paint.HeightMemory = Height;
    Paint.Color = Color;    
	Paint.Scale = 2;
		
    Paint.WidthByte = (Width % 8 == 0)? (Width / 8 ): (Width / 8 + 1);
    Paint.HeightByte = Height;    
//    printf("WidthByte = %d, HeightByte = %d\r\n", Paint.WidthByte, Paint.HeightByte);
//    printf(" EPD_WIDTH / 8 = %d\r\n",  122 / 8);
   
    Paint.Rotate = Rotate;
    Paint.Mirror = MIRROR_NONE;
    
    if(Rotate == ROTATE_0 || Rotate == ROTATE_180) {
        Paint.Width = Width;
        Paint.Height = Height;
    } else {
        Paint.Width = Height;
        Paint.Height = Width;
    }
}
/* void DEV_Module_Exit(void)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);

    //close 5V
	//DEV_Digital_Write(EPD_PWR_PIN, 0);
    DEV_Digital_Write(EPD_RST_PIN, 0);
}
 */
//from EPD_test(void) from EPD_4in2_test.c
int Zenon_EPD_test()
{
    FURI_LOG_I("EPAPER", "EPD_4IN2_test Demo");
    if(DEV_Module_Init()!=0){
        return -1;
    }

    FURI_LOG_I("EPAPER", "e-Paper Init and Clear...");
    EPD_4IN2B_V2_Init();
    EPD_4IN2B_V2_Clear();
    DEV_Delay_ms(500);

//Create a new image cache named IMAGE_BW and fill it with white
    UBYTE *BlackImage, *RYImage; // Red or Yellow
    UWORD Imagesize = ((EPD_4IN2B_V2_WIDTH % 8 == 0)? (EPD_4IN2B_V2_WIDTH / 8 ): (EPD_4IN2B_V2_WIDTH / 8 + 1)) * EPD_4IN2B_V2_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        FURI_LOG_I("EPAPER", "Failed to apply for black memory...");
        return -1;
    }
    if((RYImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        FURI_LOG_I("EPAPER", "Failed to apply for yellow memory...");
        return -1;
    }
            FURI_LOG_I("EPAPER", "NewImage:BlackImage and RYImage");

    Paint_NewImage(BlackImage, EPD_4IN2B_V2_WIDTH, EPD_4IN2B_V2_HEIGHT, 180, WHITE);
    Paint_NewImage(RYImage, EPD_4IN2B_V2_WIDTH, EPD_4IN2B_V2_HEIGHT, 180, WHITE);

    //Select Image
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_SelectImage(RYImage);
    Paint_Clear(WHITE);

////
// ommitted a lot of code from the EPD_4in2_test.c here
///

printf("Clear...\r\n");
    EPD_4IN2B_V2_Clear();

    printf("Goto Sleep...\r\n");
    EPD_4IN2B_V2_Sleep();
    free(BlackImage);
    free(RYImage);
    BlackImage = NULL;
    RYImage = NULL;
    DEV_Delay_ms(2000);//important, at least 2s
    // close 5V
    printf("close 5V, Module enters 0 power consumption ...\r\n");
    FURI_LOG_I("EPAPER", "close 5V, Module enters 0 power consumption ...");
    DEV_Module_Exit();
    
       return 0;
}


//const GpioPin* const pin13 = &gpio_ext_pb6;
//const GpioPin* const pin14 = &gpio_ext_pb7;


void i2c_send(i2cSender* i2c_sender) {
UNUSED(i2c_sender);

//GpioPinRecord pin13 = gpio_pins[8];
//GpioPinRecord pin14 = gpio_pins[9];
/*
GpioPinRecord EPD_RST_PIN = gpio_pins[8]; //12
GpioPinRecord pinDC = gpio_pins[9]; //13
GpioPinRecord EPD_CS_PIN = gpio_pins[10]; //14 alternative CS for debug
GpioPinRecord pinInput1 = gpio_pins[11]; // 16
*/

furi_hal_spi_acquire(&furi_hal_spi_bus_handle_external);
  /*
        bool result;
        uint8_t buf[8];
      //  buf = {};
        result = furi_hal_spi_bus_tx(
               &furi_hal_spi_bus_handle_external, (uint8_t*)&buf, 1, 50);
              FURI_LOG_I("TEST", "SPI: %d", result);

              */
  
// pin indexes of gpio_pins[X] defined in furi_hal_resources.c line 71

furi_hal_gpio_init_simple(EPD_DC_PIN.pin, GpioModeOutputPushPull);
furi_hal_gpio_init_simple(EPD_RST_PIN.pin, GpioModeOutputPushPull);
furi_hal_gpio_init_simple(EPD_CS_PIN.pin, GpioModeOutputPushPull);
furi_hal_gpio_init_simple(EPD_BUSY_PIN.pin, GpioModeInput);
furi_hal_gpio_init_simple(pinInput1.pin, GpioModeInput);
//furi_hal_gpio_init_simple(pin14, GpioModeInput);
     //  FURI_LOG_I("TEST", "TrueFalse: %d on pin %s number %d", i2c_sender->TrueFalse, pin13.name, pin13.number);
    // furi_hal_gpio_write(gpio_pins[8].pin, i2c_sender->TrueFalse);



Zenon_EPD_test();




FURI_LOG_I("TEST", "input: %d on %s number %d", furi_hal_gpio_read(pinInput1.pin), pinInput1.name, pinInput1.number);
       FURI_LOG_I("TEST", "Status: RST (%s:%d): %d \t DC (%s:%d): %d \t AltCS (%s:%d): %d", 
                        EPD_RST_PIN.name,    EPD_RST_PIN.number,  furi_hal_gpio_read(EPD_RST_PIN.pin),
                        EPD_DC_PIN.name,     EPD_DC_PIN.number,   furi_hal_gpio_read(EPD_DC_PIN.pin),
                        EPD_CS_PIN.name,  EPD_CS_PIN.number,furi_hal_gpio_read(EPD_CS_PIN.pin)          );
       //FURI_LOG_I("TEST", "pin14: %d", furi_hal_gpio_read(pin14));
    furi_hal_spi_release(&furi_hal_spi_bus_handle_external);









           i2c_sender->must_send = false;
    i2c_sender->sended = true;
}


/*

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
*/





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