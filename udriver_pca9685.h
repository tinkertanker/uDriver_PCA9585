#ifndef UDRIVER_PCA9685
#define UDRIVER_PCA9685

#include "pxt.h"
#include "utest/utest.h"

#define UDRIVER_PCA9685_PANIC_CODE 90

namespace UDriver_PCA9685 
{
    typedef uint8_t I2CAddress;
    typedef enum pin_t
    {
        Pin_P0 = 0,
        Pin_P1 = 1,
        Pin_P2 = 2,
        Pin_P3 = 3,
        Pin_P4 = 4,
        Pin_P5 = 5,
        Pin_P6 = 6,
        Pin_P7 = 7,
        Pin_P8 = 8,
        Pin_P9 = 9,
        Pin_P10 = 10,
        Pin_P11 = 11,
        Pin_P12 = 12,
        Pin_P13 = 13,
        Pin_P14 = 14,
        Pin_P15 = 15
    }Pin;
    
    const I2CAddress I2C_ADDRESS_ALL_CALL = 0xE0;
    
    class PCA9685
    {
    public:
        /* Construct a new instance of PCA9685 for the optional i2c address
         * If no i2c address is given would use all call address
        */
        PCA9685 (I2CAddress addr=I2C_ADDRESS_ALL_CALL){}
        
        /* PWM write value between 0-4095 to the given GVS Pin on the PCA9685 */
        //%
        void pwm_write(Pin pin, int value);
        
        /* Change the PWM modulation frequency to the given frequency in hertz 
         * NOTE: This function assumes that no external clock is used, and the
         * internal osicalltor, with a clockfrequency of 25MHz, is used. 
        */
        void set_pwm_frequency(int frequency);

        /* Activate sleep mode on the PCA9685 for the given number of 
         * microseconds.
        */
        void sleep(int msec);
    
        /* Make the PCA9685 do a software reset */
        void software_reset();

    private:
        I2CAddress address;
        uint16_t mode_register;
        
        void configure_mode(uint8_t setting, uint8_t value);
        void register_write(uint8_t reg_addr, uint8_t value);
        uint8_t register_read(uint8_t reg_addr);
    }
    
    //Functional Callbacks for makecode package
    //%
    void pwm_write(Pin pin, int value)
    //%
    void set_pwm_frequency(int frequency);
    //%
    void sleep(int msec);
    //%
    void software_reset();
    //%
    void set_address(int address);
}

#endif /* ifndef UDRIVER_PCA9685 */
