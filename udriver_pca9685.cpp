/*
 * udriver_pca9685.cpp
 * PCA9685 Driver for the MicroBit
*/

#define DEBUG
#include "udriver_pca9685.h"


using namespace pxt;
using namespace UDriver_PCA9685;

//PCA9685 Class
PCA9685::PCA9685(I2CAddress addr)
{
    this->address = addr;
}

void PCA9685::register_write(uint8_t addr, uint8_t value)
{
    MicroBitI2C bus_i2c(I2C_SDA0, I2C_SCL0);

    uint8_t packet[2] = { addr, value };

    if(bus_i2c.write(this->address, (const char *)packet, sizeof(uint8_t) * 2) \
            != MICROBIT_OK) 
    {
        DPRINT("Failed to write to PCA9685 register. Is the PCA9685 connected?");
        uBit.panic(UDRIVER_PCA9685_PANIC_CODE);
    }
}

uint8_t PCA9685::register_read(uint8_t addr)
{
    MicroBitI2C bus_i2c(I2C_SDA0, I2C_SCL0);
    
    uint8_t data;
    if(bus_i2c.write(this->address, (char *)&addr, sizeof(uint8_t)) \
            != MICROBIT_OK) 
    {
        DPRINT("Failed to read from PCA9685 register. Is the PCA9685 connected?");
        uBit.panic(UDRIVER_PCA9685_PANIC_CODE);
    }

    if(bus_i2c.read(this->address, (char *)&data, sizeof(uint8_t)) \
            != MICROBIT_OK) 
    {
        DPRINT("Failed to read from PCA9685 register. Is the PCA9685 connected?");
        uBit.panic(UDRIVER_PCA9685_PANIC_CODE);
    }
    return data;
}

/* Macros to determine control register address. Ref Datasheet */
#define REG_ADDR_ON_L(pin) (pin * 4 + 0 + 6)
#define REG_ADDR_ON_H(pin) (pin * 4 + 1 + 6)
#define REG_ADDR_OFF_L(pin) (pin * 4 + 2 + 6)
#define REG_ADDR_OFF_H(pin) (pin * 4 + 3 + 6)

void PCA9685::pwm_write(Pin pin, int value)
{
    if(value < UDRIVER_PCA9685_PWM_MIN || value > UDRIVER_PCA9685_PWM_MAX)
        return;

    //ON Register
    this->register_write(REG_ADDR_ON_H(pin), 0x1);
    this->register_write(REG_ADDR_ON_L(pin), 0x1);

    uint8_t off_lsb = (value & 0xFF); //Get least significant 8 bits
    value >>= 8;
    uint8_t off_hsb = (value & 0xF); //Get most signification 4 bits
    
    //OFF Register
    this->register_write(REG_ADDR_OFF_H(pin), off_hsb);
    this->register_write(REG_ADDR_OFF_L(pin), off_lsb);

}
