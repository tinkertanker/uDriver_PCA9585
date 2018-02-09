#ifndef UDRIVER_PCA9685
#define UDRIVER_PCA9685

#include "pxt.h"

#define UDRIVER_PCA9685_PANIC_CODE 90
#define UDRIVER_PCA9685_PWM_MAX 4095
#define UDRIVER_PCA9685_PWM_MIN 0 
namespace UDriver_PCA9685 
{
    typedef uint8_t I2CAddress;
    
    /* Defines the GVS pins on the PCA9685 */
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
    
    /* Defines the modes/settings on the PCA9685 */
    typedef enum mode_t
    {
        Mode_AllCall_Addr = 0,
        Mode_SubCall1_Addr = 1,
        Mode_SubCall2_Addr = 2,
        Mode_SubCall3_Addr = 3,
        Mode_Sleep = 4,
        Mode_AutoInc = 5,
        Mode_ExtClock = 6,
        Mode_Restart = 7,
    }Mode;

    const I2CAddress I2C_ADDRESS_ALL_CALL = 0xE0;
    
    /* Represents an PCA9685 */
    class PCA9685
    {
    public:
        /* Construct a new instance of PCA9685 for the optional i2c address
         * If no i2c address is given would use all call address
        */
        PCA9685(I2CAddress addr=I2C_ADDRESS_ALL_CALL);

        /* digital write 0 or 1 to the given PWM Pin on the PCA9685 */
        void digital_write(Pin pin, int value);

        /* digital write 0 or 1 to all PWM Pins on the PCA9685 */
        void digital_write_all(int value);
        
        /* PWM write value between 0-4095 to the given PWM Pin on the PCA9685 */
        void pwm_write(Pin pin, int value);

        /* PWM write value between 0-4095 to all PWM Pins on the PCA9685 */
        void pwm_write_all(int value);
    
        /* PWM pulse - pulse for the given microseconds for every PWM cycle */
        virtual void pwm_pulse(Pin pin, int pulse_us);

        /* Change the PWM modulation frequency to the given frequency in hertz.
         * NOTE: This function assumes that no external clock is used, and the
         * internal osicalltor, with a clockfrequency of 25MHz, is used. 
        */
        void set_pwm_frequency(int frequency);

        /* Activate low-power sleep mode on the PCA9685.
        */
        void sleep();
        
        /* Deactivate low-power sleep mode on the PCA9685.
        */
        void wake();
    
        /* Make the PCA9685 do a software reset */
        void software_reset();

        /* Change the PCA9685's main address to a new i2c address*/
        void change_address(I2CAddress addr);
        
    protected:
        I2CAddress address;
        uint8_t sub_addr = 0;
        uint8_t prev_mode = 0;
        uint16_t pwm_freq = 200;
        uint16_t pulse_mode = 0;
        uint16_t pulse_len[16];
        
        void register_write(uint8_t reg_addr, uint8_t value);
        uint8_t register_read(uint8_t reg_addr);
        void configure_mode(Mode setting, uint8_t value);
        void restore_mode();
        void add_alt_address(I2CAddress addr);
    };

    /* Represents a PCA9685 that can control servos */
    class PCA9685ServoController : public PCA9685
    {
    public:
        PCA9685ServoController(I2CAddress addr=I2C_ADDRESS_ALL_CALL);
    
        /* Move the servo's shaft to a certain angle in degrees */
        void move_servo(Pin pin, double angle_deg);
        
        /* Configure the minimal pulse, maximum pulse  in microseconds 
         * for the servo for the given Pin. */
        void configure_servo(Pin pin, int min_us, int max_us);

        /* Send PWM pulse to the servo */
        virtual void pwm_pulse(Pin pin, int pulse_us);
        
    protected:
        uint16_t servo_mode;
        uint16_t pulse_min[16];
        uint16_t pulse_max[16];
    };
}
#endif /* ifndef UDRIVER_PCA9685 */
