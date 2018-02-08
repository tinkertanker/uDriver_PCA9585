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
    this->address = addr; //I2c Address to communicate with the PCA9685
    this->wake();
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

#define REG_ADDR_MODE 0x0

void PCA9685::configure_mode(Mode setting, uint8_t value)
{
    value = !!value; //Force value into 0 or 1
    
    uint8_t mode_register = this->register_read(REG_ADDR_MODE);
    this->prev_mode = mode_register;
    //Change setting bit
    mode_register &= ~(1 << setting); //Unset Setting Bit
    mode_register |= (value << setting);  //Set setting bit if specified value
    this->register_write(REG_ADDR_MODE, mode_register);
}

void PCA9685::restore_mode()
{
    this->register_write(REG_ADDR_MODE, this->prev_mode);
}

void PCA9685::software_reset()
{
    MicroBitI2C bus_i2c(I2C_SDA0, I2C_SCL0);
    uint8_t swrst_code = 0x6;
    bus_i2c.write(0x0, (char *)&swrst_code, sizeof(uint8_t));
}
void PCA9685::sleep()
{
    this->configure_mode(Mode_Sleep, 1);
}

void PCA9685::wake()
{
    this->configure_mode(Mode_Sleep, 0);
}

/* Macros to determine control register address. Ref Datasheet */
#define REG_ADDR_ON_L(pin) (pin * 4 + 0 + 6)
#define REG_ADDR_ON_H(pin) (pin * 4 + 1 + 6)
#define REG_ADDR_OFF_L(pin) (pin * 4 + 2 + 6)
#define REG_ADDR_OFF_H(pin) (pin * 4 + 3 + 6)
#define REG_ADDR_ALL_ON_L 0xFA
#define REG_ADDR_ALL_ON_H 0xFB
#define REG_ADDR_ALL_OFF_L 0xFC
#define REG_ADDR_ALL_OFF_H 0xFD

void PCA9685::digital_write(Pin pin, int value)
{
    if(value < 0 || value > 1) return;
    
    this->wake();

    if(value == 1)
    {
        this->register_write(REG_ADDR_OFF_L(pin), 0x00);
        this->register_write(REG_ADDR_OFF_H(pin), 0x00);
        this->register_write(REG_ADDR_ON_L(pin), 0x00);
        this->register_write(REG_ADDR_ON_H(pin), 0x10);
    }
    else
    {
        this->register_write(REG_ADDR_ON_L(pin), 0x00);
        this->register_write(REG_ADDR_ON_H(pin), 0x00);
        this->register_write(REG_ADDR_OFF_L(pin), 0x00);
        this->register_write(REG_ADDR_OFF_H(pin), 0x10);
    }

    this->restore_mode();
}

void PCA9685::digital_write_all(int value)
{
    if(value < 0 || value > 1)
        return; 

    this->wake();

    if(value == 1)
    {
        this->register_write(REG_ADDR_ALL_OFF_L, 0x00);
        this->register_write(REG_ADDR_ALL_OFF_H, 0x00);
        this->register_write(REG_ADDR_ALL_ON_L, 0x00);
        this->register_write(REG_ADDR_ALL_ON_H, 0x10);
    }
    
    else
    {
        //Write default value
        this->register_write(REG_ADDR_ALL_ON_L, 0x00);
        this->register_write(REG_ADDR_ALL_ON_H, 0x10);
        this->register_write(REG_ADDR_ALL_OFF_L, 0x00);
        this->register_write(REG_ADDR_ALL_OFF_H, 0x10);
    }

    this->restore_mode();
}


void PCA9685::pwm_write(Pin pin, int value)
{
    if(value < UDRIVER_PCA9685_PWM_MIN || value > UDRIVER_PCA9685_PWM_MAX)
        return;

    this->wake();

    //ON Register
    this->register_write(REG_ADDR_ON_L(pin), 0x00);
    this->register_write(REG_ADDR_ON_H(pin), 0x00);

    uint8_t off_lsb = (value & 0xFF); //Get least significant 8 bits
    value >>= 8;
    uint8_t off_hsb = (value & 0x0F); //Get most significant 4 bits
    
    //OFF Register
    this->register_write(REG_ADDR_OFF_L(pin), off_lsb);
    this->register_write(REG_ADDR_OFF_H(pin), off_hsb);
    
    this->restore_mode();
}

void PCA9685::pwm_write_all(int value)
{
    if(value < UDRIVER_PCA9685_PWM_MIN || value > UDRIVER_PCA9685_PWM_MAX)
        return; 

    this->wake();
    
    if(value == 0)
    {
        //Write default value
        this->register_write(REG_ADDR_ALL_ON_L, 0x00);
        this->register_write(REG_ADDR_ALL_ON_H, 0x10);
        this->register_write(REG_ADDR_ALL_OFF_L, 0x00);
        this->register_write(REG_ADDR_ALL_OFF_H, 0x10);
    }
    else
    {
        //ON Register
        this->register_write(REG_ADDR_ALL_ON_L, 0x00);
        this->register_write(REG_ADDR_ALL_ON_H, 0x00);

        uint8_t off_lsb = (value & 0xFF); //Get least significant 8 bits
        value >>= 8;
        uint8_t off_hsb = (value & 0x0F); //Get most significant 4 bits
        
        //OFF Register
        this->register_write(REG_ADDR_ALL_OFF_L, off_lsb);
        this->register_write(REG_ADDR_ALL_OFF_H, off_hsb);

    }
    this->restore_mode();
}

#define REG_ADDR_PRESCALE 0xFE
#define PRESCALE_VALUE(freq) (round(25000000/(4096 * freq)) - 1)
void PCA9685::set_pwm_frequency(int frequency)
{
    if(PRESCALE_VALUE(frequency) < 0x03 || PRESCALE_VALUE(frequency) > 0xFF)
        return;
    
    this->sleep();
    this->register_write(REG_ADDR_PRESCALE, PRESCALE_VALUE(frequency));
    this->restore_mode();
}

