/*
 * udriver_pca9685.ts
 * PCA9685 driver for the Microbit
*/

/** 
 * PCA9685 μDriver allows the MicroBit to leverge the PCA9685 pletoras of analog
 * pins for applications such controling a muiltitude of LEDs or servos.
 * Attach your PCA9685 to your MicroBit via the i2c port.
*/
//% color=#b58900 icon="\uf2db" block="μDriver PCA9685"
namespace UDriver_PCA9685 
{
    /**
     * Defines the analog GVS pins on the PCA9685 
    */
    export enum Pin
    {
        P0 = 0, /* Pin value autoincrements 0...15*/
        P1,
        P2,
        P3,
        P4,
        P5,
        P6,
        P7,
        P8,
        P9,
        P10,
        P11,
        P12,
        P13,
        P14,
        P15
    }

    let pwm_frequency:number = 200; //Frequency for input checking

    /** 
     *  Write a value between 0 and 1023 as an analog signal
     *  to the given 'pin' on the PCA9685
    */
    //%blockId=UDriver_PCA9685_analog_write
    //%block="analog write|to pin %pin|value %value"
    //%shim=UDriver_PCA9685::analog_write 
    export function analog_write(pin:Pin, value:number)
    {
        if(value < 0 || value > 1023)
        {
            console.log("uDriver PCA9685: analog_write(): Invaild Argument " +
                "- Analog Write value not a value  between 0 to 1023");
            return;
        }

        //Dummy implementation for the Microbit simulator
        console.log("PWM UDriver_PCA9685: analog_write: " + value);
    }

    /** 
     *  Write a value between 0 and 1023 as an analog signal
     *  to every single pin on the PCA9685
    */
    //%blockId=UDriver_PCA9685_analog_write_all
    //%block="analog write|value %value|to all pins"
    //%shim=UDriver_PCA9685::analog_write_all
    export function analog_write_all(value:number)
    {
        if(value < 0 || value > 1023)
        {
            console.log("uDriver PCA9685: analog_write_all(): Invaild Argument " +
                "- Analog Write value not a value  between 0 to 1023");
            return;
        }
        //Dummy implementation for the Mircobit simulator
        console.log("PWM UDriver_PCA9685: analog_write_all: " + value);
    }

    /**
     * Write value 0 or 1 as a digital LOW or high respectively to the given 
     * 'pin' on the PCA9685
    */
    //%blockId=UDriver_PCA9685_digital_write
    //%block="digital write|to pin %pin|value %value"
    //%shim=UDriver_PCA9685::digital_write
    export function digital_write(pin:Pin, value:number)
    {
        if(value != 0 && value != 1)
        {
            console.log("uDriver PCA9685: digital_write(): Invaild Argument " +
                "- Digital Write value should be 0 or 1.");
            return;
        }
        //Dummy Implmentation for the Microbit simulator
        console.log("Simulate:uDriver PCA9685:digital_write:" + value);
    }


    /**
     * Write value 0 or 1 as a digital LOW or high respectively
     * to every single pin on the PCA9685
    */
    //%blockId=UDriver_PCA9685_digital_write_all
    //%block="digital write|value %value|to all pins"
    //%shim=UDriver_PCA9685::digital_write_all
    export function digital_write_all(value:number)
    {
        if(value != 0 && value != 1)
        {
            console.log("uDriver PCA9685: digital_write_all(): Invaild Argument " +
                "- Digital Write value should be 0 or 1.");
            return;
        }
        //Dummy Implmentation for the Microbit simulator
        console.log("Simulate:uDriver PCA9685:digital_write_all" + value);
    }


    /**
     * Make the servo attached on the given 'pin' move its shaft to the position
     * given by an angle in degrees between 0 and 180 degrees, with 90 degrees
     * as the servos neutral position
    */
    //%blockId=UDriver_PCA9685_move_servo
    //%block="move servo at|pin %pin |to %angle_deg|degrees"
    //%shim=UDriver_PCA9685::move_servo
    export function move_servo(pin:Pin, angle_deg:number)
    {
        if(angle_deg < 0 || angle_deg > 180)
        {
            console.log("uDriver PCA9685: move_servo(): Invaild Argument " +
                "- Only angles between 0 degree and 180 degrees are supported");
        }

        //Dummy Implmentation for the Microbit simulator
        //Simulates the PWM signals that control the servo 
        console.log("PWM Simulate:uDriver PCA9685: move_servo:" + angle_deg);
    }

    
    /**
     * Write value between 0 and 4095 as the width of the PWM signal sent to
     * the given 'pin' on the PCA9685
    */
    //%blockId=UDriver_PCA9685_pwm_write
    //%block="PWM write|to pin %pin|value %value"
    //%shim=UDriver_PCA9685::pwm_write
    export function pwm_write(pin:Pin, value:number)
    {
        if(value < 0 || value > 4095)
        {
            console.log("uDriver PCA9685: pwm_write(): Invaild Argument " +
                "- PWM Write value should be between 0 and 4095.");
            return;
        }
        //Dummy Implmentation for the Microbit simulator
        console.log("PWM Simulate:uDriver PCA9685:pwm_write: " + value );
    }

    /**
     * Write value between 0 and 4095 as the width of the PWM signal sent
     * to every single pin on the PCA9685. Use this to get increased precision
     * sent PWM signals
    */
    //%blockId=UDriver_PCA9685_pwm_write_all
    //%block="PWM write|value %value|to all pins"
    //%advanced=true
    //%shim=UDriver_PCA9685:pwm_write_all
    export function pwm_write_all(value:number)
    {
        if(value < 0 || value > 4095)
        {
            console.log("uDriver PCA9685: pwm_write_all(): Invaild Argument " +
                "- PWM Write value should be between 0 and 4095.");
            return;
        }
        //Dummy Implmentation for the Microbit simulator
        console.log("PWM Simulate:uDriver PCA9685:pwm_write_all:" + value);
    }

    /**
     * Pulse digital HIGH for the given number of microseconds every PWM duty
     * cycle, after which hold digital LOW for the rest of the duty cycle, after
     * the pulse. 
     * Increasing the PWM modulation frequency using set_pwm_frequency()
     * will increase the precision of actual pulse written, but decrease the 
     * allowable pulse length and vice-versa.
    */
    //%blockId=UDriver_PCA9685_pwm_pulse
    //%block="pulse for|to pin %pin |for %microseconds|microseconds every cycle"
    //%shim=UDriver_PCA9685::pwm_pulse
    export function pwm_pulse(pin:Pin, microseconds:number) 
    {
        let period_us:number = ( 1.0/pwm_frequency ) * 1000.0 * 1000.0;
        if(microseconds < 0)
        {
            console.log("uDriver PCA9685: pwm_pulse(): Invaild Argument " +
                "- PWM pulse time should not be negative");
            return;
        }
        else if(microseconds > period_us)
        {
            console.log("uDriver PCA9685: pwm_pulse(): Invaild Argument " +
                "- PWM pulse time should be smaller than PWM period:" + period_us);
            return;
        }

        //Dummy Implmentation for the Microbit simulator
        console.log("PWM Simulate:uDriver PCA9685: pwm pulse:" + microseconds);
    }

    /**
     * Set the PWM modulation frequency to the given 'frequency' in hertz.
     * Only frequencies between 24 to 1526 Hertz are supported
     * Warning: If servos are attached, please check that the servo is able to 
     * withstand the frequency before changing, or risk destorying your servo.
    */
    //%blockId=UDriver_PCA9685_set_pwm_frequency
    //%block="set PWM modulation frequency|to %frequency |Hertz"
    //%advanced=true
    //%shim=UDriver_PCA9685::set_pwm_frequency
    export function set_pwm_frequency(frequency:number)
    {
        if(frequency < 24 || frequency > 1526)
        {
            console.log("uDriver PCA9685: set_pwm_frequency(): Invaild Argument " +
                "- Only frequencies between 24 Hz and 1526 Hz are supported");
            return;
        }

        //Dummy Implmentation for the Microbit simulator
        console.log("Simulate:uDriver PCA9685:set_pwm_frequency: "+ frequency);

        pwm_frequency = frequency;
    }

    /** 
     * Cause the PCA9685 to go into low power sleep mode. 
     * During its slumber, all analog/PWM outputs on all pins would not work.
     * Call wake() to awaken the (chew)PCA9685
    */
    //%blockId=UDriver_PCA9685_sleep
    //%block="PCA9685 sleep"
    //%advanced=true
    //%shim=UDriver_PCA9685::sleep
    export function sleep()
    {
        //Dummy Implmentation for the Microbit simulator
        console.log("Simulate:uDriver PCA9685:sleep");
    }

    /**
     * Awaken the (chew)PCA9685 from its slumber.
    */
    //%blockId=UDriver_PCA9685_wake
    //%block="PCA9685 wake up"
    //%advanced=true
    //%shim=UDriver_PCA9685::wake
    export function wake()
    {
        //Dummy Implmentation for the Microbit simulator
        console.log("Simulate:uDriver PCA9685:awaken");
    }

    /**
     * Perform a software reset on the PCA9685
     * All prior configuration on the PCA9685 will be lost, including the output
     * for each pin.
    */
    //%blockId=UDriver_PCA9685_software_reset
    //%block="PCA9685 perform software reset"
    //%advanced=true
    //%shim=UDriver_PCA9685::software_reset
    export function software_reset()
    {
        //Dummy Implmentation for the Microbit simulator
        console.log("Simulate:uDriver PCA9685:software_reset");
    }
    
    /**
     * Change the minimum or maximum limits on the length pulses sent to the 
     * servo in microseconds
     * By default the pulses range from 1000 to 2000 microseconds.
     * Warning: Only change this if your servo supports it, or risk destorying 
     * your servo
    */
    //%blockId=UDriver_PCA9685_configure_servo
    //%block="configure servo|at pin %pin |set min pulse %min_microseconds |set max pulse %max_microseconds"
    //%advanced=true
    //%shim=UDriver_PCA9685::configure_servo
    export function configure_servo(pin:Pin, min_value:number, max_value:number)
    {
        //Dummy Implmentation for the Microbit simulator
        console.log("Simulate:uDriver PCA9685:configure_servo: pin:" + pin
            + " min:" + min_value + " max:" + max_value );
    } 
}
