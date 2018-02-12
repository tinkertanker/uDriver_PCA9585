/*
 * udriver_pca9685.cpp
 * PCA9685 Driver for the MicroBit
*/

#include "udriver_pca9685.h"

#undef printf
#define PCA9685_PIN_MIN 0
#define PCA9685_PIN_MAX 15

using namespace pxt;
using namespace UDriver_PCA9685;

//PCA9685 Class
PCA9685::PCA9685(I2CAddress addr)
{
    this->address = addr;
    this->wake();
}

void PCA9685::register_write(uint8_t addr, uint8_t value)
{
    MicroBitI2C bus_i2c(I2C_SDA0, I2C_SCL0);

    uint8_t packet[2] = { addr, value };

    if(bus_i2c.write(this->address, (const char *)packet, sizeof(uint8_t) * 2) \
            != MICROBIT_OK) 
    {
        uBit.serial.printf("Failed to write to PCA9685 register. Is the PCA9685 connected?");
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
        uBit.serial.printf("Failed to read from PCA9685 register. Is the PCA9685 connected?");
        uBit.panic(UDRIVER_PCA9685_PANIC_CODE);
    }

    if(bus_i2c.read(this->address, (char *)&data, sizeof(uint8_t)) \
            != MICROBIT_OK) 
    {
        uBit.serial.printf("Failed to read from PCA9685 register. Is the PCA9685 connected?");
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
}

void PCA9685::digital_write_all(int value)
{
    if(value < 0 || value > 1)
        return; 

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

    this->pulse_mode = 0;
}

void PCA9685::pwm_write(Pin pin, int value)
{
    if(value < UDRIVER_PCA9685_PWM_MIN || value > UDRIVER_PCA9685_PWM_MAX)
        return;

    //ON Register
    this->register_write(REG_ADDR_ON_L(pin), 0x00);
    this->register_write(REG_ADDR_ON_H(pin), 0x00);

    uint8_t off_lsb = (value & 0xFF); //Get least significant 8 bits
    value >>= 8;
    uint8_t off_hsb = (value & 0x0F); //Get most significant 4 bits
    
    //OFF Register
    this->register_write(REG_ADDR_OFF_L(pin), off_lsb);
    this->register_write(REG_ADDR_OFF_H(pin), off_hsb);
    
}

void PCA9685::pwm_write_all(int value)
{
    if(value < UDRIVER_PCA9685_PWM_MIN || value > UDRIVER_PCA9685_PWM_MAX)
        return; 

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

    this->pulse_mode = 0;
}
 
void PCA9685::pwm_pulse(Pin pin, int pulse_us)
{
    //Time per PWM division in microseconds.      | ms      | us
    double tick = (double) (1.0 / this->pwm_freq) * 1000.0 * 1000.0 / 4095.0;
    int pwm_pulse = round((double) pulse_us / tick);
    
    this->pulse_len[pin] = pulse_us;
    this->pulse_mode |= (1 << pin); //Mark that this pin operates in pulse mode
    
    this->pwm_write(pin, pwm_pulse);
}

#define REG_ADDR_PRESCALE 0xFE
#define PRESCALE_VALUE(freq) (round(25000000.0/(4096.0 * (double)freq)) - 1)
void PCA9685::set_pwm_frequency(int frequency)
{
    if(PRESCALE_VALUE(frequency) < 0x03 || PRESCALE_VALUE(frequency) > 0xFF)
        return;
    
    this->sleep();
    this->register_write(REG_ADDR_PRESCALE, PRESCALE_VALUE(frequency));
    this->restore_mode();
    this->pwm_freq = frequency;
    //Reconfigure PWM ticks based on new PWM frequency 
    for(int pin = PCA9685_PIN_MIN; pin <= PCA9685_PIN_MAX; pin ++)
    {
        if(this->pulse_mode & (1UL << pin))
        {
            this->pwm_pulse((Pin)pin, this->pulse_len[pin]);
        }
    }
}

#define REG_ADDR_ACALL 0x05
#define REG_ADDR_SUB(n) (0x01 +  n)
void PCA9685::change_address(I2CAddress addr)
{
    if(addr <= 0x07 || addr >= 0xF0) return; //Reject Reserved Addresses
    this->configure_mode(Mode_AllCall_Addr, 1);
    this->register_write(REG_ADDR_ACALL, addr);
    this->address = addr;
}

void PCA9685::add_alt_address(I2CAddress addr)
{
    if(addr <= 0x07 || addr >= 0xF0) return; //Reject Reserved Addresses
    this->sub_addr = (this->sub_addr + 1) % 4;

    if(this->sub_addr == 1) this->configure_mode(Mode_SubCall1_Addr, 1);
    if(this->sub_addr == 2) this->configure_mode(Mode_SubCall2_Addr, 1);
    if(this->sub_addr == 3) this->configure_mode(Mode_SubCall3_Addr, 1);
    this->register_write(REG_ADDR_SUB(this->sub_addr), addr);
}

//PCA9685 Servo Controller Class
PCA9685ServoController::PCA9685ServoController(I2CAddress addr)
{
    this->address = addr;
    this->wake();

    //Set default values
    for(int pin = PCA9685_PIN_MIN; pin <= PCA9685_PIN_MAX; pin ++)
    {
        this->pulse_min[pin] = 1000; //1000 usec
        this->pulse_max[pin] = 2000; //2000 usec
    }
    
    this->set_pwm_frequency(50); //50 H4
}

void PCA9685ServoController::configure_servo(Pin pin, int min_us, int max_us)
{
    this->pulse_max[pin] = max_us;
    this->pulse_min[pin] = min_us;
}

void PCA9685ServoController::pwm_pulse(Pin pin, int pulse_us)
{
    if(this->servo_mode & (1UL << pin))
    {
        pulse_us = (pulse_us < this->pulse_min[pin]) ? this->pulse_min[pin] 
            : pulse_us;
        pulse_us = (pulse_us > this->pulse_max[pin]) ? this->pulse_max[pin] 
            : pulse_us;
    }
    
    PCA9685::pwm_pulse(pin, pulse_us);
}

void PCA9685ServoController::move_servo(Pin pin, double angle_deg)
{
    angle_deg = (angle_deg > 180.0) ? 180.0 : angle_deg;
    angle_deg = (angle_deg < 0.0) ? 0.0 : angle_deg;

    int pulse_us = round((angle_deg / 180.0) * (2000.0 - 1000.0) + 1000.0);
    
    this->servo_mode |=  (1 << pin); //Mark this pin as servo pin.
    this->pwm_pulse(pin, pulse_us);
}

//Functional Callbacks for makecode package
namespace UDriver_PCA9685
{
    static PCA9685ServoController *pca_device = new PCA9685ServoController;
    //%
    void digital_write(int pin, int value){ pca_device->digital_write((Pin)pin, value); }
    //%
    void digital_write_all(int pin, int value){ pca_device->digital_write_all(value); }
    //%
    void pwm_write(int pin, int value){ pca_device->pwm_write((Pin)pin, value); }
    //%
    void pwm_write_all(int value){ pca_device->pwm_write_all(value); }
    //%
    void analog_write(int pin, int value){ 
        int pwm_value = (int)((double) value / 1023.0 * 4095.0);
        pwm_write(pin, pwm_value);
    }
    //%
    void analog_write_all(int value){ 
        int pwm_value = (int)((double) value / 1023.0 * 4095.0);
        pwm_write_all(pwm_value);
    }
    //%
    void pwm_pulse(int pin, int pulse_us) { pca_device->pwm_pulse((Pin)pin, pulse_us); }
    //%
    void set_pwm_frequency(int frequency){ pca_device->set_pwm_frequency(frequency); }
    //%
    void sleep(){ pca_device->sleep(); }
    //%
    void wake(){ pca_device->wake(); }
    //%
    void software_reset(){ pca_device->software_reset(); }
    //%
    void move_servo(int pin, int angle_deg){ pca_device->move_servo((Pin)pin, angle_deg);}
    //%
    void configure_servo(int pin, int min, int max){ pca_device->configure_servo((Pin)pin, min, max); }
}
