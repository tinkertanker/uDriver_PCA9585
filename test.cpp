/*
 * test.ts
 * UDriver PCA9685 Tests - CPP
*/

#define private public //Test Private members

#define DEBUG 1

#include "pxt.h"
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
    

    void test_pwm_write()
    {
        PCA9685 device;
        
        for(int i = 0; i <= 4095; i += 5)
        {
            DPRINTF("Testing PWM level: %d\r\n", i);
            device.pwm_write(Pin_P0, i);

            TEST_EQUAL(device.register_read(0x06), 0x00);
            TEST_EQUAL(device.register_read(0x07), 0x00);
            TEST_EQUAL(device.register_read(0x08),(i & 0xFF));
            TEST_EQUAL(device.register_read(0x09), ((i >> 8) & 0x0F));
        }

        for(int i = 4095; i >= 0; i --)
        {
            device.pwm_write(Pin_P0, i);

            TEST_EQUAL(device.register_read(0x06), 0x00);
            TEST_EQUAL(device.register_read(0x07), 0x00);
            TEST_EQUAL(device.register_read(0x08),(i & 0xFF));
            TEST_EQUAL(device.register_read(0x09), ((i >> 8) & 0x0F));
        }
    }


    //%
    void unit_test()
    {
        TEST_BEGIN;
        TEST(test_register_rw);
        TEST(test_configure_mode);
        TEST(test_software_reset);
        TEST(test_pwm_write);
        TEST_END;
    }
}
