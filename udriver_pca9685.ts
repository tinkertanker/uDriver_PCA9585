/*
 * udriver_pca9685.ts
 * PCA9685 driver for the Microbit
*/

/** 
 * PCA9685 μDriver allows the MicroBit to leverge the PCA9685 pletoras of pins 
 * for applications such controling a muiltitude of LEDs or servos.
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

    //%shim=UDriver_PCA9685::digital_write
    export function digital_write(pin:Number, value:Number){}
    
    //%shim=UDriver_PCA9685::digital_write_all
    export function digital_write_all(pin:Number, value:Number){}
    
    //%shim=UDriver_PCA9685:pwm_write
    export function pwm_write(pin:Number, value:Number){}

    //%shim=UDriver_PCA9685:pwm_write_all
    export function pwm_write_all(pin:Number, value:Number){}

    //%shim=Udriver_PCA9685::pwm_pulse
    export function pwm_pulse(pin:Number, pulse_us:Number) {}

    //%shim=Udriver_PCA9685::set_pwm_frequency
    export function set_pwm_frequency(frequency:Number){}

    //%shim=Udriver_PCA9685::sleep
    export function sleep(){}

    //%shim=Udriver_PCA9685::wake
    export function wake(){}
    
    //%shim=Udriver_PCA9685::software_reset
    export function software_reset(){}
    
    //%shim=Udriver_PCA9685::move_servo
    export function move_servo(pin:Number, angle_deg:Number){}
    //
    //%shim=Udriver_PCA9685::configure_servo
    export function configure_servo(pin:Number, min:Number, max:Number){}
}
