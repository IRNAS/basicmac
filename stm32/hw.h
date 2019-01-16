// Copyright (C) 2016-2019 Semtech (International) AG. All rights reserved.
// Copyright (C) 2014-2016 IBM Corporation. All rights reserved.
//
// This file is subject to the terms and conditions defined in file 'LICENSE',
// which is part of this source code package.

#ifndef _hw_h_
#define _hw_h_

#include "board.h"
#include "oslmic.h"

#if defined(STM32L0)
#include "stm32l0xx.h"
#elif defined(STM32L1)
#include "stm32l1xx.h"
#endif


//////////////////////////////////////////////////////////////////////
// GPIO
//////////////////////////////////////////////////////////////////////

// GPIO by port number (A=0, B=1, ..)
#define GPIOx(no) ((GPIO_TypeDef*) (GPIOA_BASE + (no)*(GPIOB_BASE-GPIOA_BASE)))

// GPIOCFG macros
#define GPIOCFG_AF_MASK        0x0F

#define GPIOCFG_MODE_SHIFT      4
#define GPIOCFG_MODE_MASK      (3<<GPIOCFG_MODE_SHIFT)
#define GPIOCFG_MODE_INP       (0<<GPIOCFG_MODE_SHIFT)
#define GPIOCFG_MODE_OUT       (1<<GPIOCFG_MODE_SHIFT)
#define GPIOCFG_MODE_ALT       (2<<GPIOCFG_MODE_SHIFT)
#define GPIOCFG_MODE_ANA       (3<<GPIOCFG_MODE_SHIFT)
#define GPIOCFG_OSPEED_SHIFT    6
#define GPIOCFG_OSPEED_MASK    (3<<GPIOCFG_OSPEED_SHIFT)
#define GPIOCFG_OSPEED_400kHz  (0<<GPIOCFG_OSPEED_SHIFT)
#define GPIOCFG_OSPEED_2MHz    (1<<GPIOCFG_OSPEED_SHIFT)
#define GPIOCFG_OSPEED_10MHz   (2<<GPIOCFG_OSPEED_SHIFT)
#define GPIOCFG_OSPEED_40MHz   (3<<GPIOCFG_OSPEED_SHIFT)
#define GPIOCFG_OTYPE_SHIFT     8
#define GPIOCFG_OTYPE_MASK     (1<<GPIOCFG_OTYPE_SHIFT)
#define GPIOCFG_OTYPE_PUPD     (0<<GPIOCFG_OTYPE_SHIFT)
#define GPIOCFG_OTYPE_OPEN     (1<<GPIOCFG_OTYPE_SHIFT)
#define GPIOCFG_PUPD_SHIFT      9
#define GPIOCFG_PUPD_MASK      (3<<GPIOCFG_PUPD_SHIFT)
#define GPIOCFG_PUPD_NONE      (0<<GPIOCFG_PUPD_SHIFT)
#define GPIOCFG_PUPD_PUP       (1<<GPIOCFG_PUPD_SHIFT)
#define GPIOCFG_PUPD_PDN       (2<<GPIOCFG_PUPD_SHIFT)
#define GPIOCFG_PUPD_RFU       (3<<GPIOCFG_PUPD_SHIFT)

// IRQ triggers (same values as in Moterunner!)
#define GPIO_IRQ_MASK          0x38
#define GPIO_IRQ_FALLING       0x20
#define GPIO_IRQ_RISING        0x28
#define GPIO_IRQ_CHANGE        0x30

// configure operation mode of GPIO pin
void gpio_cfg_pin (int port, int pin, int gpiocfg);

// configure operation mode of GPIO pin and set state
void gpio_cfg_set_pin (int port, int pin, int gpiocfg, int state);

// set state of GPIO output pin
void gpio_set_pin (int port, int pin, int state);

// get state of GPIO output pin
int gpio_get_pin (int port, int pin);

// configure given line as external interrupt source (EXTI handler)
void gpio_cfg_extirq (int port, int pin, int irqcfg);
void gpio_cfg_extirq_ex (int port, int pin, bool rising, bool falling);

// clear/enable, or disable interrupt for given line (configure first!)
void gpio_set_extirq (int pin, int on);

enum { GPIO_LOHI = 0, GPIO_HILO = 1 };
// generate a transition, then switch to HiZ
int gpio_transition (int port, int pin, int type, int duration, unsigned int config);

#define GPIO_DEFAULT_CFG	(GPIOCFG_MODE_ANA | GPIOCFG_OSPEED_400kHz | GPIOCFG_OTYPE_OPEN | GPIOCFG_PUPD_NONE)
#define SET_PIN(gpio, val)	gpio_set_pin(BRD_PORT(gpio), BRD_PIN(gpio), (val))
#define GET_PIN(gpio)		gpio_get_pin(BRD_PORT(gpio), BRD_PIN(gpio))
#define CFG_PIN(gpio, opts)	gpio_cfg_pin(BRD_PORT(gpio), BRD_PIN(gpio), (opts))
#define CFG_PIN_VAL(gpio, o, s)	gpio_cfg_set_pin(BRD_PORT(gpio), BRD_PIN(gpio), (o), (s))
#define CFG_PIN_AF(gpio, opts)	gpio_cfg_pin(BRD_PORT(gpio), BRD_PIN(gpio), GPIOCFG_MODE_ALT | BRD_AF(gpio) | (opts))
#define CFG_PIN_DEFAULT(gpio)	gpio_cfg_pin(BRD_PORT(gpio), BRD_PIN(gpio), GPIO_DEFAULT_CFG)
#define IRQ_PIN(gpio, opts)	gpio_cfg_extirq(BRD_PORT(gpio), BRD_PIN(gpio), (opts))
#define IRQ_PIN_SET(gpio, on)	gpio_set_extirq(BRD_PIN(gpio), (on))
#define TXN_PIN(gpio, t, d, c)	gpio_transition(BRD_PORT(gpio), BRD_PIN(gpio), (t), (d), (c))

// Convenience macros to set GPIO configuration registers
#define GPIO_AF_BITS		4		// width of bit field
#define GPIO_AF_MASK		0x0F		// mask in AFR[0/1]
#define GPIO_AFRLR(i)		((i) >> 3)
#define GPIO_AF_PINi(i,af)	((af) << (((i) & 7) * GPIO_AF_BITS))
#define GPIO_AF_set(p,i,af)	do { \
    (p)->AFR[GPIO_AFRLR(i)] = ((p)->AFR[GPIO_AFRLR(i)] \
	    & ~GPIO_AF_PINi(i, GPIO_AF_MASK)) | GPIO_AF_PINi(i, af); \
} while (0)
#define HW_CFG_PIN(p,i,cfg)	do { \
    if (((cfg) & GPIOCFG_MODE_MASK) == GPIOCFG_MODE_ALT) { \
	GPIO_AF_set(p, i, (cfg) & GPIOCFG_AF_MASK); \
	(p)->MODER = ((p)->MODER   & ~(3 << (2*(i)))) | ((((cfg) >> GPIOCFG_MODE_SHIFT  ) & 3) << (2*(i))); \
    } \
    (p)->OSPEEDR   = ((p)->OSPEEDR & ~(3 << (2*(i)))) | ((((cfg) >> GPIOCFG_OSPEED_SHIFT) & 3) << (2*(i))); \
    (p)->OTYPER    = ((p)->OTYPER  & ~(1 << (1*(i)))) | ((((cfg) >> GPIOCFG_OTYPE_SHIFT ) & 1) << (1*(i))); \
    (p)->PUPDR     = ((p)->PUPDR   & ~(3 << (2*(i)))) | ((((cfg) >> GPIOCFG_PUPD_SHIFT  ) & 3) << (2*(i))); \
    if (((cfg) & GPIOCFG_MODE_MASK) != GPIOCFG_MODE_ALT) { \
	(p)->MODER = ((p)->MODER   & ~(3 << (2*(i)))) | ((((cfg) >> GPIOCFG_MODE_SHIFT  ) & 3) << (2*(i))); \
    } \
} while (0)
#define HW_SET_PIN(p,i,state)	do { \
    (p)->BSRR |= (1 << (i + ((state) ? 0 : 16))); \
} while (0)
#define HW_GET_PIN(p,i)		((p)->IDR & (1 << (i)))

// Theses macros manipulate the GPIO registers directly, without generating a function call
#define SET_PIN_DIRECT(gpio, val)	HW_SET_PIN(GPIOx(BRD_PORT(gpio)), BRD_PIN(gpio), (val))
#define GET_PIN_DIRECT(gpio)		HW_GET_PIN(GPIOx(BRD_PORT(gpio)), BRD_PIN(gpio))
#define CFG_PIN_DIRECT(gpio, opts)	HW_CFG_PIN(GPIOx(BRD_PORT(gpio)), BRD_PIN(gpio), (opts))

// Determine RCC enable bit for GPIO port
#if defined(STM32L0)
#define GPIO_RCC_ENR	(RCC->IOPENR)
#define GPIO_EN(gpio)	(((gpio) == PORT_A) ? RCC_IOPENR_GPIOAEN \
	: ((gpio) == PORT_B) ? RCC_IOPENR_GPIOBEN \
	: ((gpio) == PORT_C) ? RCC_IOPENR_GPIOCEN \
	: 0)
#elif defined(STM32L1)
#define GPIO_RCC_ENR	(RCC->AHBENR)
#define GPIO_EN(gpio)	(((gpio) == PORT_A) ? RCC_AHBENR_GPIOAEN \
	: ((gpio) == PORT_B) ? RCC_AHBENR_GPIOBEN \
	: ((gpio) == PORT_C) ? RCC_AHBENR_GPIOCEN \
	: 0)
#endif


#define PERIPH_PIO
#define PIO_IRQ_LINE(gpio)   BRD_PIN(gpio)


//////////////////////////////////////////////////////////////////////
// EEPROM
//////////////////////////////////////////////////////////////////////

// Unique device ID registers (96 bits)
#define UNIQUE_ID_BASE 0x1FF80050
#define UNIQUE_ID0 (UNIQUE_ID_BASE+0x00)
#define UNIQUE_ID1 (UNIQUE_ID_BASE+0x04)
#define UNIQUE_ID2 (UNIQUE_ID_BASE+0x14)

// EEPROM (these macros are addresses, type uint32_t)
#define EEPROM_BASE	DATA_EEPROM_BASE
#ifdef DATA_EEPROM_BANK2_END
#define EEPROM_END	(DATA_EEPROM_BANK2_END + 1)
#else
#define EEPROM_END	DATA_EEPROM_END
#endif
#define EEPROM_SZ	(EEPROM_END - EEPROM_BASE)

// EEPROM layout for STM32

// 0x0000-0x003f   64 B : reserved for bootloader
// 0x0040-0x005f   32 B : reserved for persistent stack data
// 0x0060-0x00ff  160 B : reserved for personalization data
// 0x0100-......        : reserved for application

#define STACKDATA_BASE          (EEPROM_BASE + 0x0040)
#define PERSODATA_BASE          (EEPROM_BASE + 0x0060)
#define APPDATA_BASE            (EEPROM_BASE + 0x0100)

#define STACKDATA_SZ            (PERSODATA_BASE - STACKDATA_BASE)
#define PERSODATA_SZ            (APPDATA_BASE - PERSODATA_BASE)
#define APPDATA_SZ              (EEPROM_END - APPDATA_BASE)

#define PERIPH_EEPROM

//////////////////////////////////////////////////////////////////////
// ADC
//////////////////////////////////////////////////////////////////////

#define VREFINT_CAL_ADDR ((u2_t*) (0x1FF80078U))
#define VREFINT_ADC_CH       17
u2_t adc_read (u1_t chnl);


//////////////////////////////////////////////////////////////////////
// CRC engine (32bit aligned words only!)
//////////////////////////////////////////////////////////////////////

#define PERIPH_CRC


//////////////////////////////////////////////////////////////////////
// I2C
//////////////////////////////////////////////////////////////////////

#ifdef BRD_I2C

#define I2C_BUSY	1
#define I2C_OK		0
#define I2C_NAK		-1
#define I2C_ABORT	-2
typedef void (*i2c_cb) (int status);
void i2c_xfer (unsigned int addr, unsigned char* buf, unsigned int wlen, unsigned int rlen, i2c_cb cb, ostime_t timeout);
void i2c_xfer_ex (unsigned int addr, unsigned char* buf, unsigned int wlen, unsigned int rlen, ostime_t timeout,
	osjob_t* job, osjobcb_t cb, int* pstatus);
void i2c_irq (void);
void i2c_abort (void);

#endif


// ------------------------------------------------
// USART

#define PERIPH_USART

#if (BRD_USART & BRD_LPUART(0)) == 0
#define USART_BR_9600	0xd05
#define USART_BR_115200	0x116
#else
#define USART_BR_9600	0xd0555
#define USART_BR_115200	0x115c7
#endif


//////////////////////////////////////////////////////////////////////
// SPI
//////////////////////////////////////////////////////////////////////

#define SPI_MODE_CPHA  0x01
#define SPI_MODE_CPOL  0x02

// initialize SPI (modes 0-3, bit0=clock phase, bit1=clock polarity)
void spi_init (u1_t mode);

// transfer byte to and from SPI (no chip-select)
u1_t spi_xfer (u1_t out);


//////////////////////////////////////////////////////////////////////
// FLASH
//////////////////////////////////////////////////////////////////////

#define FLASH_PAGE_SZ		128
#define FLASH_PAGE_NW		(FLASH_PAGE_SZ >> 2)

#define FLASH_SIZE      (*((unsigned short*)0x1FF8007C) << 10) // flash size register (RM0377 28.1.1)

// NOTE: continue using static flash size for firmwares since old boot loaders cannot handle more than 128K
// #define FLASH_END	((uint32_t*)(FLASH_BASE + FLASH_SIZE))
#undef FLASH_END         // already defined by some STM32L0XXXxx header files
extern uint32_t _eflash; // provided by linker script
#define FLASH_END	(&_eflash)

typedef struct {
    uint32_t* base;
    uint32_t off;
    uint32_t buf[FLASH_PAGE_NW];
} flash_bw_state;

void flash_buffered_write (flash_bw_state* state, uint32_t* src, uint32_t nwords);
void flash_wword (uint32_t* dst, uint32_t word);


//////////////////////////////////////////////////////////////////////
// LEDs
//////////////////////////////////////////////////////////////////////

void leds_init (void);
void leds_set (unsigned int gpio, int state);
void leds_pwm (unsigned int gpio, int dc);
void leds_pwm_irq (void);
void leds_pulse (unsigned int gpio, unsigned int min, unsigned int max, int step, unsigned int delay);


//////////////////////////////////////////////////////////////////////
// Vibe
//////////////////////////////////////////////////////////////////////

#ifdef BRD_VIBE_TIM
void vibe_init (void);
void vibe_set (int dc);
#endif


//////////////////////////////////////////////////////////////////////
// PWM
//////////////////////////////////////////////////////////////////////

#ifdef BRD_PWM_TIM
void pwm_ramp (unsigned int gpio, unsigned int period_us, unsigned int max, osjob_t* job, osjobcb_t cb);
void pwm_irq (void);
#endif


//////////////////////////////////////////////////////////////////////
// PIR
//////////////////////////////////////////////////////////////////////

#ifdef BRD_PIR_TIM
void pir_init (void);
void pir_config (unsigned int config, osjob_t* job, osjobcb_t cb);
void pir_readout (osjob_t* job, osjobcb_t cb);
void pir_get (unsigned int* pconfig, unsigned int* pstatus);
void pir_tim_irq (void);
#endif


//////////////////////////////////////////////////////////////////////
// Infrared Blaster
//////////////////////////////////////////////////////////////////////

#ifdef BRD_IR_TIM

typedef struct {
    uint16_t on;
    uint16_t off;
} ir_bp;

typedef struct {
    uint16_t rpt;
    uint16_t n;
    ir_bp* pairs;
} ir_bpseq;

void ir_burst (unsigned int psc, ir_bpseq* seqs, int nseqs, osjobcb_t cb);

void ir_tim_irq (void);
#endif


#endif
