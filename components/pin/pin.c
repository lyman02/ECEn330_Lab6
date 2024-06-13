#include <stdio.h>
#include "soc/reg_base.h" // DR_REG_GPIO_BASE, DR_REG_IO_MUX_BASE
#include "driver/rtc_io.h" // rtc_gpio_*
#include "pin.h"

#define FUNC_RESET 0x100;
#define IO_MUX_RESET 0x00002900;
#define PIN_RANGE 32
// GPIO Matrix Registers
#define GPIO_OUT_REG          (DR_REG_GPIO_BASE+0x04)
#define GPIO_OUT_W1TS_REG 	  (DR_REG_GPIO_BASE+0x08)
#define GPIO_OUT_W1TC_REG 	  (DR_REG_GPIO_BASE+0x0C)
#define GPIO_OUT1_REG 	      (DR_REG_GPIO_BASE+0x10)
#define GPIO_OUT1_W1TS_REG 	  (DR_REG_GPIO_BASE+0x14)
#define GPIO_OUT1_W1TC_REG 	  (DR_REG_GPIO_BASE+0x18)

#define GPIO_ENABLE_REG           (DR_REG_GPIO_BASE+0x20)
#define GPIO_ENABLE_W1TS_REG 	  (DR_REG_GPIO_BASE+0x24)
#define GPIO_ENABLE_W1TC_REG 	  (DR_REG_GPIO_BASE+0x28)
#define GPIO_ENABLE1_REG 	      (DR_REG_GPIO_BASE+0x2C)
#define GPIO_ENABLE1_W1TS_REG 	  (DR_REG_GPIO_BASE+0x30)
#define GPIO_ENABLE1_W1TC_REG 	  (DR_REG_GPIO_BASE+0x34)

#define GPIO_PINn_REG(n)                (DR_REG_GPIO_BASE+0x88+(0x04*(n)))
#define GPIO_FUNCn_OUT_SEL_CFG_REG(n)   (DR_REG_GPIO_BASE+0x530+(0x04*(n)))

#define GPIO_IN_REG          (DR_REG_GPIO_BASE+0x3C)
#define GPIO_IN1_REG 	     (DR_REG_GPIO_BASE+0x40)

#define PAD_DRIVER 2

// NOTE: Remember to enclose the macro values in parenthesis, as above

// IO MUX Registers
#define IO_MUX_REG(n) (DR_REG_IO_MUX_BASE+PIN_MUX_REG_OFFSET[(n)])

#define FUN_WPD  7
#define FUN_WPU  8
#define FUN_IE   9
#define FUN_DRV  10
#define MCU_SEL  12

#define REG(r) (*(volatile uint32_t *)(r))
#define REG_SET_BIT(r,b) (REG(r) |= (1U << (b)))
#define REG_CLR_BIT(r,b) (REG(r) &= ~(1U << (b)))
#define REG_GET_BIT(r,b) ((REG(r) & (1U << (b))) != 0)

// Gives byte offset of IO_MUX Configuration Register
// from base address DR_REG_IO_MUX_BASE
static const uint8_t PIN_MUX_REG_OFFSET[] = {
    0x44, 0x88, 0x40, 0x84, 0x48, 0x6c, 0x60, 0x64, // pin  0- 7
    0x68, 0x54, 0x58, 0x5c, 0x34, 0x38, 0x30, 0x3c, // pin  8-15
    0x4c, 0x50, 0x70, 0x74, 0x78, 0x7c, 0x80, 0x8c, // pin 16-23
    0x90, 0x24, 0x28, 0x2c, 0xFF, 0xFF, 0xFF, 0xFF, // pin 24-31
    0x1c, 0x20, 0x14, 0x18, 0x04, 0x08, 0x0c, 0x10, // pin 32-39
};


// Reset the configuration of a pin to not be an input or an output.
// Pull-up is enabled so the pin does not float.
int32_t pin_reset(pin_num_t pin)
{
	if (rtc_gpio_is_valid_gpio(pin)) { // hand-off work to RTC subsystem
		rtc_gpio_deinit(pin);
		rtc_gpio_pullup_en(pin);
		rtc_gpio_pulldown_dis(pin);
	}

	REG(GPIO_PINn_REG(pin)) &= 0x00;
	REG(GPIO_FUNCn_OUT_SEL_CFG_REG(pin)) = FUNC_RESET;
	REG(IO_MUX_REG(pin)) = IO_MUX_RESET;
	// NOTE: By default, pin should not float, save power with FUN_WPU=1
	// Now that the pin is reset, set the output level to zero
	return pin_set_level(pin, 0);
}

// Enable or disable a pull-up on the pin.
int32_t pin_pullup(pin_num_t pin, bool enable)
{
	if (rtc_gpio_is_valid_gpio(pin)) { // hand-off work to RTC subsystem
		if (enable) return rtc_gpio_pullup_en(pin);
		else return rtc_gpio_pullup_dis(pin);
	}
	// Checks if pullup is enabled
	if(enable){
		REG_SET_BIT(IO_MUX_REG(pin), FUN_WPU);
	}else{
		REG_CLR_BIT(IO_MUX_REG(pin), FUN_WPU);
	}

	return 0;
}

// Enable or disable a pull-down on the pin.
int32_t pin_pulldown(pin_num_t pin, bool enable)
{

	if (rtc_gpio_is_valid_gpio(pin)) { // hand-off work to RTC subsystem
		if (enable) return rtc_gpio_pulldown_en(pin);
		else return rtc_gpio_pulldown_dis(pin);
	}
	
	// Checks if pulldown is enabled
	if(enable){
		REG_SET_BIT(IO_MUX_REG(pin), FUN_WPD);
	}else{
		REG_CLR_BIT(IO_MUX_REG(pin), FUN_WPD);
	}

	return 0;
}

// Enable or disable the pin as an input signal.
int32_t pin_input(pin_num_t pin, bool enable)
{
	
	// Checks if input is enabled for this pin
	if(enable){
		REG_SET_BIT(IO_MUX_REG(pin), FUN_IE);
	}else{
		REG_CLR_BIT(IO_MUX_REG(pin), FUN_IE);
	}
	return 0;
}

// Enable or disable the pin as an output signal.
int32_t pin_output(pin_num_t pin, bool enable)
{
	
	// Checks if output is enabled for this pin
	if(enable){
		//  Checks if the pin is in the range of 0-31 (ENABLE)
		//  Else the pin is in the range of 32-39 (ENABLE1)
		if(pin < PIN_RANGE){
			REG_SET_BIT(GPIO_ENABLE_W1TS_REG, pin);
		}else{
			REG_SET_BIT(GPIO_ENABLE1_W1TS_REG, (pin-PIN_RANGE));
		}
		
	}else{
		//  Checks if the pin is in the range of 0-31 (ENABLE)
		//  Else the pin is in the range of 32-39 (ENABLE1)
		if(pin < PIN_RANGE){
			REG_SET_BIT(GPIO_ENABLE_W1TC_REG, pin);
		}else{
			REG_SET_BIT(GPIO_ENABLE1_W1TC_REG, (pin-PIN_RANGE));
		}
	}
	return 0;
}

// Enable or disable the pin as an open-drain signal.
int32_t pin_odrain(pin_num_t pin, bool enable)
{
	// Checks if open-drain signal setting is enabled
	if(enable){
		REG_SET_BIT(GPIO_PINn_REG(pin), PAD_DRIVER);
	}else{
		REG_CLR_BIT(GPIO_PINn_REG(pin), PAD_DRIVER);
	}
	return 0;
}

// Sets the output signal level if the pin is configured as an output.
int32_t pin_set_level(pin_num_t pin, int32_t level)
{
	//  Checks if the pin is in the range of 0-31 (ENABLE)
	//  Else the pin is in the range of 32-39 (ENABLE1)
	if(pin < PIN_RANGE){
		// Checks if level is non-zero
		if(level){
			REG_SET_BIT(GPIO_OUT_W1TS_REG, pin);
		}else{
			REG_SET_BIT(GPIO_OUT_W1TC_REG, pin);
		}
		
	}else{
		// Checks if level is non-zero
		if(level){
			REG_SET_BIT(GPIO_OUT1_W1TS_REG, (pin-PIN_RANGE));
		}else{
			REG_SET_BIT(GPIO_OUT1_W1TC_REG, (pin-PIN_RANGE));
		}
	}
	return 0;
}

// Gets the input signal level if the pin is configured as an input.
int32_t pin_get_level(pin_num_t pin)
{
	//  Checks if the pin is in the range of 0-31 (ENABLE)
	//  Else the pin is in the range of 32-39 (ENABLE1)
	if(pin >= 0 && pin < PIN_RANGE){
		return REG_GET_BIT(GPIO_IN_REG, pin);
	}else{
		return REG_GET_BIT(GPIO_IN1_REG, (pin-PIN_RANGE));
	}
	
	return 0;
}

// Get the value of the input registers, one pin per bit.
// The two 32-bit input registers are concatenated into a uint64_t.
uint64_t pin_get_in_reg(void)
{
	uint64_t concatIn = (uint64_t)REG(GPIO_IN1_REG) << PIN_RANGE | REG(GPIO_IN_REG);

	return concatIn;
}

// Get the value of the output registers, one pin per bit.
// The two 32-bit output registers are concatenated into a uint64_t.
uint64_t pin_get_out_reg(void)
{
	uint64_t concatOut = (uint64_t)REG(GPIO_OUT1_REG) << PIN_RANGE | REG(GPIO_OUT_REG);

	return concatOut;
}
