#include "mbed.h"
#include "uop_msb_2_0_0.h"
#include <iostream>
#include "BMP280_SPI.h"
#include "SDBlockDevice.h"
#include "FATFileSystem.h"

using namespace uop_msb_200;
using namespace std;

// SD Card
static SDBlockDevice sd(PB_5, PB_4, PB_3, PF_3); // SD Card Block Device
static InterruptIn sd_inserted(PF_4); // Interrupt for card insertion events

int write_sdcard() {
    printf("Initialise and write to a file\n");

    // call the SDBlockDevice instance initialisation method.
    if (0 != sd.init()) {
        printf("Init failed \n");
        return -1;
    }

    FATFileSystem fs("sd", &sd);
    FILE *fp = fopen("/sd/test.txt", "w");
    if (fp == NULL) {
        error("Could not open file for write\n");
        sd.deinit();
        return -1;
    } else {
        // Put some text in the file...
        fprintf(fp, "Nick says Hi!\n");
        // Tidy up here
        fclose(fp);
        printf("SD Write done...\n");
        sd.deinit();
        return 0;
    }
    }

    int read_sdcard() {
    printf("Initialise and read from a file\n");

    // call the SDBlockDevice instance initialisation method.
    if (0 != sd.init()) {
        printf("Init failed \n");
        return -1;
    }

    FATFileSystem fs("sd", &sd);
    FILE *fp = fopen("/sd/test.txt", "r");
    if (fp == NULL) {
    error("Could not open or find file for read\n");
    sd.deinit();
    return -1;
    } else {
        // Put some text in the file...
        char buff[64];
        buff[63] = 0;
        while (!feof(fp)) {
            fgets(buff, 63, fp);
            printf("%s\n", buff);
        }
        // Tidy up here
        fclose(fp);
        printf("SD Write done...\n");
        sd.deinit();
        return 0;
    }
}

// MATRIX DISPLAY SPI WITH OUTPUT ENABLE
static SPI spi(PC_12, PC_11, PC_10); // MOSI, MISO, SCLK
static DigitalOut cs(PB_6);          // Chip Select ACTIVE LOW
static DigitalOut oe(PB_12);         // Output Enable ACTIVE LOW

#define SCAN_RATE_MS 25
void matrix_init(void) {
  spi.format(8, 0);       // 8bits, Rising edge, +VE Logic Data
  spi.frequency(1000000); // 1MHz Data Rate
  oe = 0;
}

void matrix_scan(void) {

  for (int j = 0; j <= 7;
       j++) // NB ROW has 3 to 8 decoder so 0 to 7 to Row Select
            // COLUMNS(COL) are 0 to 255 to select each Byte
  {
    for (int i = 1; i < 255; i *= 2) // Increment LEFT Hand Block of 8
    {
      cs = 0;          // Send Data to Matrix
      spi.write(0x00); // COL RHS
      spi.write(i);    // COL LHS
      spi.write(j);    // ROW RHS
      cs = 1; // low to high will effectivelly LATCH the Shift register to
              // output
      thread_sleep_for(SCAN_RATE_MS);
    }
    for (int i = 1; i < 255; i *= 2) // Increment RIGHT Hand Block of 8
    {
      cs = 0;          // Send Data to Matrix
      spi.write(i);    // COL RHS
      spi.write(0x00); // COL LHS
      spi.write(j);    // ROW RHS
      cs = 1; // low to high will effectivelly LATCH the Shift register to
              // output
      thread_sleep_for(SCAN_RATE_MS);
    }
  }
  for (int j = 0; j <= 7; j++) 
    // NB ROW has 3 to 8 decoder so 0 to 7 to Row Select    
    // COLUMNS(COL) are 0 to 255 to select each Byte
  {
    {
      cs = 0;          // Send Data to Matrix
      spi.write(0xFF); // COL RHS
      spi.write(0xFF); // COL LHS
      spi.write(j);    // ROW RHS
      cs = 1; // low to high will effectivelly LATCH the Shift register to
              // output
      thread_sleep_for(SCAN_RATE_MS);
    }
  }
  for (int j = 7; j >= 0; j--) 
       // NB ROW has 3 to 8 decoder so 0 to 7 to Row Select
       // COLUMNS(COL) are 0 to 255 to select each Byte
  {
    {
      cs = 0;          // Send Data to Matrix
      spi.write(0xFF); // COL RHS
      spi.write(0xFF); // COL LHS
      spi.write(j);    // ROW RHS
      cs = 1; // low to high will effectivelly LATCH the Shift register to
              // output
      thread_sleep_for(SCAN_RATE_MS);
    }
  }
}

void clearMatrix(void) {
  cs = 0;          // CLEAR Matrix
  spi.write(0x00); // COL RHS
  spi.write(0x00); // COL LHS
  spi.write(0x00); // ROX RHS
  cs = 1;
}
// Enable Output NB can be more selective than this if you wish

// C Function Prototypes
extern int write_sdcard();
extern int read_sdcard(); 
extern void matrix_init(void);
extern void matrix_scan(void);
extern void clearMatrix(void);

//Digital Inputs (you could use DigitalIn or InterruptIn instead)
Buttons btns;

//Analogue Inputs
AnalogIn ldr(AN_LDR_PIN);

//LED Outputs
LatchedLED ledStrip(LatchedLED::STRIP);
LatchedLED ledDigit(LatchedLED::SEVEN_SEG);

//Buzzer
Buzzer buzz;

//LCD Display
LCD_16X2_DISPLAY lcd_disp;

// Remember that main() runs in its own thread in the OS
int main()
{
    //Play sound
    buzz.playTone("C");
    ThisThread::sleep_for(500ms);
    buzz.rest();

    //Spin on a switch
    cout << "Press the blue button" << endl;
    while (btns.BlueButton == 0);

    //Led Strip Display
    ledStrip.setGroup(LatchedLED::LEDGROUP::BLUE);  //Set mode to BLUE led bar
    ledStrip = 0b10101010;                          //Set data pattern

    ledStrip.setGroup(LatchedLED::LEDGROUP::GREEN); //Set mode to GREEN led bar
    ledStrip = 0b11001100;                          //Set data pattern

    ledStrip.setGroup(LatchedLED::LEDGROUP::RED);  //Set mode to RED led bar
    ledStrip = 0b11100111;                         //Set data pattern

    ledStrip.enable(true);                         //Enable outputs (turn them all on!)

    // LED Digit
    ledDigit.setGroup(LatchedLED::LEDGROUP::TENS);
    ledDigit = 4;
    ledDigit.setGroup(LatchedLED::LEDGROUP::UNITS);
    ledDigit = 2;
    ledDigit.enable(true);                         //Enable outputs

    // LCD Display
    lcd_disp.cls();                                 //Clear screen
    lcd_disp.locate(0, 0);                          //Set cursor (row,col)
    lcd_disp.printf("COMP2004");
    lcd_disp.locate(1, 0);
    lcd_disp.printf("Coursework C1W2");

    //Environmental sensor
    BMP280_SPI bmp280(PB_5, PB_4, PB_3, PB_2);
    bmp280.initialize();

    //SD Card (see example_code.cpp)
    write_sdcard();
    read_sdcard();

    //Quick matrix display (see example_code.cpp)
    matrix_init();      // Do once
    matrix_scan();      // Demo
    clearMatrix();      // Clear the matrix

    // Demo of sensors (not a good sampling method!)
    while (true) {
        // Light Levels
        float light = ldr;
        printf("Light level: %5.3f\n", light);

        //Temperatre and pressure
        float temperature=bmp280.getTemperature();
        float pressure=bmp280.getPressure();
        printf("%.1fC %.1fmBar\n",temperature,pressure);        

        ThisThread::sleep_for(500ms);
    }
}


