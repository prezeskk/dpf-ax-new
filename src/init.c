/* Initialization routines that are needed mostly only once */

#include "print.h"
#include "global.h"
#include "utils.h"
#include "dpf.h"
#include "lcd.h"
#include "spiflash.h"

#pragma codeseg INIT
#pragma constseg INIT

void mdelay(BYTE x);

void term_init(void) __banked
{
	_asm
		mov	_g_chartbl_offs, #chartbl4x8_offs
		mov	_g_chartbl_offs + 1, #(chartbl4x8_offs >> 8)
		mov	_g_chartbl_offs + 2, #(chartbl4x8_offs >> 16)
	_endasm;
	term_selfont(FONT_SMALL);
}


void timer1_config(BYTE on) __banked
{
#ifdef LCD_BACKLIGHT_NONE
	// No backlight control at all??
	on;
	return;
#else
	unsigned char con;

#  ifdef LCD_TIMER1_PWM_P23
	// pwn on P2.3 + sys-clock + enable
	con = T1POS1 | T1POEN | T1ON;
#  elif defined(LCD_TIMER1_PWM_P40)
	// pwm on P4.0 + +sys-clock + enable
	con = T1POS2 | T1POEN | T1ON;
#  elif defined(LCD_TIMER1_PWM_NONE)
	// no pwm output, timer stopped
	con = 0;
#  endif

// See if we have a init value for the timer
// If not, just start/stop the timer -- per/pwm set by _custom_backlighttbl/_custom_backlighttbl2
#  ifdef LCD_TIMER1_PERIOD
	// must be table or custom backlight control with fixed frequency
	tmr1perl = ((unsigned int) LCD_TIMER1_PERIOD) & 0xFF;
	tmr1perh = ((unsigned int) LCD_TIMER1_PERIOD) >> 8;
#  endif
	tmr1con = (on) ? con : 0;
	// Call setbrightness for (re-)init of tmr1pwm (or whatever - if LCD_BACKLIGHT_CUSTOM)
	set_brightness(on ? g_lcd.brightness : 0);
#endif
}

void timer0_config()
{
	tmr0cnt = 0; // Reset counter
	tmr0pr = 41; // Period
	tmr0psr = 3; // Prescaler
	tmr0con = T0OS | T0CS | T0ON; // Turn on
}

void adc_config(BYTE enable)
{
	switch (enable) {
		case 0:
			adccon &= ~ADCEN;
			return;
		case 1:
			adcrate = 24 - 1;
			break;
		case 2:
			adcrate = TMREN | 1;
			break;
	}
	pie &= ~(P_VSENSE); // Disable P03
	adccon |= ADCEN | 3; // Select P03 for analog in
}

void sleep(unsigned short ticks) __banked
{
	BYTE reg;
	reg = ie;

	tmr2cntl = 0;
	tmr2cnth = 0;
	tmr2perl = ticks;
	tmr2perh = ticks >> 8;

	_B_T2P = 0; // Make sure we're not pending

	ie = EA | T2IE;
	tmr2con =  T2S | (5 << 1); // Unit: 1 / 1024 second
 	tmr2con |= T2ON;

	wdtcon &= ~WDTEN;

	pcon |= COREEN;    // IDLE

	tmr2con = 0; // Stop timer
	wdtcon |= WDTPND | WDTEN | 7; // Rub Watchdog

	ie = reg;
}

static
void mdelay(BYTE x)
{
	int i;
	while (x--) {
		RUB_WATCHDOG();
		i = 0x200;
		do {
			_asm
				nop
			_endasm;
		} while (--i);
	}
}

// No longer used from version 0.4 up.
//static const BYTE rot_index[4] = {
//	ROTCODE_UP, ROTCODE_LEFT, ROTCODE_DOWN, ROTCODE_RIGHT
//};
//BYTE g_rgborder = PIXELORDER(LCD_ORIENTATION_RGB);

void lcd_orientation(BYTE which) __banked
{
//	BYTE c;
//	c = rot_index[which];
	lcd_custom_setorientation(which);
//	g_rgborder = RGBORDER(c);
}


static
void flash_writecmd(BYTE cmd)
{
	spi_select();
	spibuf = cmd;
	while (spi_busy());
	spi_deselect();
}

static void flash_wakeup(void)
{
	spi_select();
	spibuf = SPM_RES; while (spi_busy());
	spibuf = 0xff; while (spi_busy());
	spibuf = 0xff; while (spi_busy());
	spibuf = 0xff; while (spi_busy());
	sleep(10);
	spi_deselect();
}

void config_ports(BYTE enable)
{
	if (enable) {
		// Configure Inputs:
		p0 = ~(FLA_PWR | P_VCOMP); // Enable flash power, all others HIGH
		p0up = BUT_MENU; // Pullup enable
		p0dir = ~(P_VCOMP | FLA_PWR | LCD_PWR); // Outputs

		p1 = 0xff;
		p1up = 0x00;
		p1dir = FLA_MISO | MIC; // Inputs

		p2 = ~BUZZER;
#ifndef LCD_P23_ACTIVE_HIGH
		p2 |= 0x08;	// backlight off --> P23 = 1
#else
		p2 &= 0xf7;	// backlight off --> P23 = 0
#endif
		p2up = LCD_CS | FLA_CS; // PU enable
		p2dir = ~(BUZZER | LCD_LED | LCD_CS | FLA_CS); // Those are outputs

		p3up = 0xff;
		p3dir = ALL_OUTPUTS;
	} else {
		p0up |= (BUT_MENU | BUT_IRQ | TEMP_NTC | FLA_PWR | LCD_PWR);
		p0dir = ALL_INPUTS & ~LCD_PWR;
		p0 = FLA_PWR;

		p1up = 0x00;
		p1dir = FLA_MISO; // only input
		p1 = MIC;

#ifndef LCD_P23_ACTIVE_HIGH
		p2 |= 0x08;	// backlight off --> P23 = 1
#else
		p2 &= 0xf7;	// backlight off --> P23 = 0
#endif
		p2dir = ~(P27 | BUZZER | LCD_LED | LCD_CS | FLA_CS ); // Outputs
//#if DPFMODEL == pink
//		p2up  = ~(      BUZZER | LCD_LED | LCD_CS | FLA_CS ); // PU disable
//		p2   &= ~(      BUZZER           | LCD_CS | FLA_CS ); // ????
//		// Pulling FLA_CS active does not make sense
//#else
		p2up  = ~(      BUZZER | LCD_LED | LCD_CS | FLA_CS ); // PU disable
//#endif
		p3dir = ALL_OUTPUTS;
		p3 = 0;
#if !defined(LCD_TIMER1_PWM_P40)
		p4dir = ALL_INPUTS & ~AUDIO_EN;
		p4 = ~AUDIO_EN;
#endif
	}

#ifdef LCD_TIMER1_PWM_P40
	p4dir = 0xFE;
	p4 = 0x01;
#endif
}

#ifdef BUILD_DEVEL

__pdata BYTE g_databuf[0x40];
unsigned BYTE g_datacount = 0;

void logger(void)
{
	BYTE n;
	unsigned char v;

	// Turn on oscillator for ADC:
	pcon &= ~OSCCEN;
	_asm nop nop nop _endasm;

	pcon &= ~SELRTC;  // use oscillator as system clock
	_asm nop nop nop _endasm; // Important: Wait to settle..

	config_ports(1);
	adc_config(1);
	adccon |= ADCGO; // kick on conversion

	wdtcon |= WDTPND | WDTEN | 7; // Turn on WDOG
	while ((adccon & ADCGO)); // Wait til ADC value ready
	
	v = (adcbufl >> 6) | (adcbufh << 2);

	g_databuf[g_datacount++] = v;
	g_count[LOG]++; // total count
	if (g_datacount == sizeof(g_databuf) / 2 /* || v < 0 */) {
		n = flash_write(g_databuf, g_datacount);
		g_datacount = 0;
	}

	wdtcon &= ~WDTEN; // Turn off watchdog
	pcon |= SELRTC;  // use RTC as system clock
	_asm nop nop nop _endasm; // Important: Wait to settle..
	pcon |= OSCCEN;
	config_ports(0);
	adc_config(0);
}
#endif

void init(BYTE mode)
{
	if (mode == PWR_DOWN) {
		pcon = 0x10;
	} else {
		pcon &= ~OSCCEN;
	}
	_asm nop nop nop _endasm;

	// Activate watchdog to reset after ~2 seconds:
	wdtcon = WDTPND | WDTEN | 7; // Turn on WDOG

	switch (mode) {
		case PWR_DOWN:
			tmr3con = T3FB | T3IE | T3LP | T3ON;
		// no break!
		case PWR_DEEPSLEEP:
			pcon &= ~SELRTC;  // use 24 mhz sysclk
			_asm nop nop nop _endasm;
			ckcon = 0;   // Turn on ROM clock
			config_ports(1);
#ifdef DANGEROUS_SLEEP
			flash_wakeup();
#endif
			adc_config(1);
			lcd_init(); // Initialize LCD
			adccon |= ADCGO; // kick on conversion
		// no break!
		case PWR_SLEEP:
			//do not turn on backlight here!
			//lcd_backlight_on();
	}

	sleep(100);

	timer0_config();

	ie |= WAUIE | T0IE | RAWIE;

}


void shutdown(BYTE mode) __banked
{
#ifdef BUILD_DEVEL
	if (g_log) {
		flash_erasesector(LOG_SECTOR);
		g_datacount = 0;
		g_databuf[0] = 0xbe;
		g_databuf[1] = 0xef;
		flash_write(g_databuf, 2);
	}
#endif
	switch (mode) {
		case PWR_DOWN:
			tmr3con &= ~T3ON; // Disable RTC timer
		// no break!
		case PWR_DEEPSLEEP:
			ckcon = IROMCEN;   // Turn off ROM clock
#ifdef DANGEROUS_SLEEP
			// This is dangerous, because some flashes don't wake up
			// from power down...
			if (!g_log) 
				flash_writecmd(SPM_DP); // Power down flash
#endif
			config_ports(0);
			adc_config(0);
		// no break!
		case PWR_SLEEP:
			tmr2con &= ~T2ON; // Disable timer
			tmr1con = 0;
			tmr0con &= ~T0ON; // Disable timer
			lcd_backlight_off(); // turn off backlight
	}

	wdtcon &= ~WDTEN; // Turn off watchdog

	wkedg = WEP06; // falling edge menu button
	wkpnd = 0; // Clear pending wakeup events
	wken = ~(WEP06 | WEP07); // Allow only MENU button wakeup

	ie = EA | WAUIE;  // allow only port interrupts

	// _B_T3IE = 0; This resets the chip when pending

	pcon |= SELRTC;  // use RTC as system clock
	_asm nop nop nop _endasm; // Important: Wait to settle..

	// ..before we disable the main clock

	pcon |= OSCCEN;
	pcon |= STOPC;    // Stop clock
	_asm nop nop nop _endasm;

	// Now we wait for an interrupt:
	if (mode != PWR_DOWN) {
		_B_EA = 0; g_wkup = 0; g_alarm = 0; _B_EA = 1;
		ie |= RAWIE; // Enable RTC IRQ
		// We loop while we have only RTC interrupts:
		while (!g_wkup && !g_alarm) {
			// Log every x seconds:
			// if (g_log && (g_count[SECONDS] & 0x1f) == 0) logger();
			pcon |= STOPC;    // Stop clock again
			_asm nop nop nop _endasm;
		}
	}
	init(mode); // Initialize all
}

void init_all() __banked
{
	init(PWR_DOWN);
}

void turn_off(void) __banked
{
	sleep(500);
	// If this is enabled, log ADC values to flash
	shutdown(PWR_DEEPSLEEP);
	sleep(200);
	g_button.evt = 0; // clear events so we don't right repeat this
	//clrscreen(RGB565(0, 0, 70));
	disp_home(); print_splash(); sleep(100); lcd_backlight_on();
	// g_refresh = 1; // force menu refresh
}

#ifndef BUILD_DEVEL
extern BYTE _load_config_from_flash(unsigned long addr);

void init_config() __banked
{
	BYTE status;
	// iSerial String is in code memory, so we have to do write-access via xdata
        __xdata unsigned char *p = ((__xdata unsigned char *) &g_usbserial[4]) - 0x800;

	status = _load_config_from_flash(CONFIG_SECTOR);
	if (status == 0)
	{
		g_config.splash = DEFAULT_SPLASH;
#if !defined(LCD_BACKLIGHT_NONE)
		g_config.brightness = LCD_DEFAULT_BRIGHTNESS_VALUE;
#else
		g_config.brightness = 1;
#endif
#if !defined(LCD_CONTRAST_NONE)
		g_config.contrast = LCD_DEFAULT_CONTRAST_VALUE;
#else
		g_config.contrast = 1;
#endif		
		g_config.usbserial = 1;
	}
	g_lcd.brightness = g_config.brightness;
	// don't do this here - else backlight will be turned on!
	//set_brightness(g_lcd.brightness);
	set_contrast(g_config.contrast);
	*p = (g_config.usbserial / 10) + '0';
	*(p+2) = (g_config.usbserial % 10) + '0';
}
#endif
