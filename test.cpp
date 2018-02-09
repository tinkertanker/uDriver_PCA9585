/*
 * test.ts
 * UDriver PCA9685 Tests - CPP
*/

#define private public //Test Private members
#define protected public //Test proptected members

#define DEBUG 1

#include "udriver_pca9685.h"
#include "utest/utest.h"
#include "utest/utest.c"

using namespace pxt;
using namespace UDriver_PCA9685;

namespace Test
{
    void test_register_rw()
    {
        PCA9685 device;
        device.register_write(0x06, 0xFF);
        TEST_EQUAL(device.register_read(0x06), 0xFF);
    } 

    void test_configure_mode()
    {
        PCA9685 device;
        device.configure_mode(Mode_Sleep, 0);
        uint8_t mode_reg = device.register_read(0x0);
        TEST_EQUAL((mode_reg & (1 << Mode_Sleep)), 0);

        device.configure_mode(Mode_Sleep, 1);
        mode_reg = device.register_read(0x0);
        
        TEST_EQUAL(((mode_reg & (1 << Mode_Sleep)) >> Mode_Sleep), 1);
    }

    void test_software_reset()
    {
        PCA9685 device;
        device.register_write(0x06, 0xFF);
        device.software_reset();
        TEST_EQUAL(device.register_read(0x6), 0x0);
    }
    
    void test_sleep()
    {
        PCA9685 device;
        device.sleep();
        uint8_t mode_reg = device.register_read(0x0);
        TEST_EQUAL(((mode_reg & (1 << Mode_Sleep)) >> Mode_Sleep), 1);

        device.wake();
        mode_reg = device.register_read(0x0);
        TEST_EQUAL((mode_reg & (1 << Mode_Sleep)), 0);
    }

    /* Write tests assume that there is a LED on Pin0 */
    void test_pwm_write()
    {
        PCA9685 device;
        
        for(int i = 0; i <= 4095; i += 8)
        {
            device.pwm_write(Pin_P0, i);

            TEST_EQUAL(device.register_read(0x06), 0x00);
            TEST_EQUAL(device.register_read(0x07), 0x00);
            TEST_EQUAL(device.register_read(0x08),(i & 0xFF));
            TEST_EQUAL(device.register_read(0x09), ((i >> 8) & 0x0F));
        }

        for(int i = 4095; i >= 0; i -= 12)
        {
            device.pwm_write(Pin_P0, i);

            TEST_EQUAL(device.register_read(0x06), 0x00);
            TEST_EQUAL(device.register_read(0x07), 0x00);
            TEST_EQUAL(device.register_read(0x08),(i & 0xFF));
            TEST_EQUAL(device.register_read(0x09), ((i >> 8) & 0x0F));
        }
    }


    void test_digital_write()
    {
        PCA9685 device;
    
        for(int i = 0; i <= 10; i++)
        {
            device.digital_write(Pin_P0, i % 2);

            if(i % 2 == 1) TEST_EQUAL(device.register_read(0x07), 0x10); //ON High Register
            else TEST_EQUAL(device.register_read(0x09), 0x10); //Off High register
            uBit.sleep(250);
        }
    }

    
    void test_pwm_write_all()
    {
        PCA9685 device;
        
        DPRINT("Unfortunately, there is now way to test the all() methods,\r\n your MUST TEST using visual inspection");

        for(int i = 0; i <= 4095; i += 8)
        {
            device.pwm_write_all(i);
        }

        for(int i = 4095; i >= 0; i -= 12)
        {
            device.pwm_write_all(i);
        }
    }

    
    void test_digital_write_all()
    {
        PCA9685 device;

        DPRINT("Unfortunately, there is now way to test the all() methods,\r\n your MUST TEST using visual inspection");
    
        for(int i = 0; i <= 10; i++)
        {
            device.digital_write_all(i % 2);

            uBit.sleep(250);
        }
    }
    
    void test_set_pwm_frequency()
    {
        PCA9685 device;
        
        device.set_pwm_frequency(1526);
        TEST_EQUAL(device.register_read(0xFE), 0x03);
        device.set_pwm_frequency(24);
        TEST_EQUAL(device.register_read(0xFE), 0xFD);
        device.set_pwm_frequency(200);
    }

    /* Servo Tests assumes that there are servos on pins 13,15 */
    void test_configure_servo()
    {
        PCA9685ServoController device;
        device.configure_servo(Pin_P13, 300, 3000);
        device.configure_servo(Pin_P15, 300, 3000);

        TEST_EQUAL(device.pulse_min[Pin_P13], 300);
        TEST_EQUAL(device.pulse_max[Pin_P13], 3000);
        TEST_EQUAL(device.pulse_min[Pin_P15], 300);
        TEST_EQUAL(device.pulse_max[Pin_P15], 3000);
    }

#define REG_ADDR_ON_L(pin) (pin * 4 + 0 + 6)
#define REG_ADDR_ON_H(pin) (pin * 4 + 1 + 6)
#define REG_ADDR_OFF_L(pin) (pin * 4 + 2 + 6)
#define REG_ADDR_OFF_H(pin) (pin * 4 + 3 + 6)

    void test_pwm_pulse_servo()
    {
        PCA9685ServoController device;
        device.set_pwm_frequency(50);
        device.servo_mode |=  (1 << Pin_P13); //Mark this pin as servo pin.
        device.pwm_pulse(Pin_P13, 1500);
        TEST_EQUAL(device.pulse_len[Pin_P13], 1500);
        double tick = (1.0/50.0) * 1000.0 * 1000.0 / 4095.0;
        uint16_t pwm_pulse = floor(1500.0 / tick);

        TEST_EQUAL(device.register_read(REG_ADDR_ON_L(Pin_P13)), 0x0);
        TEST_EQUAL(device.register_read(REG_ADDR_ON_H(Pin_P13)), 0x0);
        
        uint8_t l_pwm_pulse = pwm_pulse & 0xFF;
        pwm_pulse >>= 8;
        uint8_t h_pwm_pulse = pwm_pulse & 0x0F;
        TEST_EQUAL(device.register_read(REG_ADDR_OFF_L(Pin_P13)), l_pwm_pulse);
        TEST_EQUAL(device.register_read(REG_ADDR_OFF_H(Pin_P13)), h_pwm_pulse);
    

        device.servo_mode |=  (1 << Pin_P15); //Mark this pin as servo pin.
        device.configure_servo(Pin_P15, 544, 2400);
        device.pwm_pulse(Pin_P15, 9999);

        pwm_pulse = floor(2400.0 / tick);

        l_pwm_pulse = pwm_pulse & 0xFF;
        pwm_pulse >>= 8;
        h_pwm_pulse = pwm_pulse & 0x0F;
        TEST_EQUAL(device.register_read(REG_ADDR_OFF_L(Pin_P15)), l_pwm_pulse);
        TEST_EQUAL(device.register_read(REG_ADDR_OFF_H(Pin_P15)), h_pwm_pulse);

        device.pwm_pulse(Pin_P15,-9999);

        pwm_pulse = floor(544.0 / tick);

        l_pwm_pulse = pwm_pulse & 0xFF;
        pwm_pulse >>= 8;
        h_pwm_pulse = pwm_pulse & 0x0F;
        TEST_EQUAL(device.register_read(REG_ADDR_OFF_L(Pin_P15)), l_pwm_pulse);
        TEST_EQUAL(device.register_read(REG_ADDR_OFF_H(Pin_P15)), h_pwm_pulse);
    }
    
    void test_move_servo()
    {
        PCA9685ServoController device;
        device.move_servo(Pin_P13, 90);
        device.move_servo(Pin_P15, 90);
        uBit.sleep(1000);
        TEST_EQUAL(device.pulse_len[Pin_P13], 1500);
        TEST_EQUAL(device.pulse_len[Pin_P15], 1500);

        device.move_servo(Pin_P13, 180);
        device.move_servo(Pin_P15, 180);
        uBit.sleep(1000);
        TEST_EQUAL(device.pulse_len[Pin_P13], 2000);
        TEST_EQUAL(device.pulse_len[Pin_P15], 2000);

        device.move_servo(Pin_P13, 0);
        device.move_servo(Pin_P15, 0);
        uBit.sleep(1000);
        TEST_EQUAL(device.pulse_len[Pin_P13], 1000);
        TEST_EQUAL(device.pulse_len[Pin_P15], 1000);

        device.move_servo(Pin_P13, 90);
        device.move_servo(Pin_P15, 90);
    }
    
    //%
    void unit_test()
    {
        TEST_BEGIN;
        TEST(test_register_rw);
        TEST(test_configure_mode);
        TEST(test_pwm_write);
        TEST(test_sleep);
        TEST(test_digital_write);
        //TEST(test_pwm_write_all);
        //TEST(test_digital_write_all);
        TEST(test_set_pwm_frequency);
        TEST(test_software_reset);
        TEST(test_configure_servo);
        TEST(test_pwm_pulse_servo);
        TEST(test_move_servo);
        TEST_END;
    }
        
}
