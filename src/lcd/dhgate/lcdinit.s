	.include 'dpf.inc'

	.area LCDAUX (CODE)

_lcd_custom_init::
	mov	p3dir,#0x0
	anl	p1dir,#0xe8
	anl	p2dir,#0xf6
	orl	p2,#0x8
	anl	p2,#0xfd
	lcall	X1502
	lcall	X1502
	lcall	X1502
	lcall	X1502
	lcall	X1502
	anl	p1,#0xfe
	lcall	X1502
	lcall	X1502
	lcall	X1502
	lcall	X1502
	lcall	X1502
	lcall	X1502
	lcall	X1502
	lcall	X1502
	orl	p1,#0x1
;
	mov	dptr,#_custom_initseq
	ljmp	_lcd_init_by_table
;
X1502:	mov	r7,#0x0
X1504:	nop	
	djnz	r7,X1504
	ret	
;
; If custom backlight handling is needed, uncomment the following label
; and set LCD_BACKLIGHT_CUSTOM in dpfmodel.h
; r3 contains brightness value (0 .. LCD_MAX_BRIGHTNESS_VALUE)
;
;_lcd_custom_setbacklight::

;
; If custom contrast handling is needed, uncomment the following label
; and set LCD_CONTRAST_CUSTOM in dpfmodel.h
; r3 contains contrast value (1 .. LCD_MAX_CONTRAST_VALUE)
;
;_lcd_custom_setcontrast::

	.area LCDAUX (CODE)


;backlight_table::
;	.db	If needed, put data for custom backlight handling here
;_custom_backlighttbl_len::  .db  . - backlight_table


;contrast_table::
;	.db	If needed, put data for custom contrast handling here
;_custom_contrasttbl_len::  .db  . - contrast_table

_custom_contrasttbl::
	.db	0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10
_custom_contrasttbl_len::  .db  7

_custom_contrasttbl2::
	.db	0x71, 0x00, 0x71, 0x00, 0x91, 0x00, 0x91, 0x00, 0xff
_custom_contrasttbl2_len::  .db  9

_custom_contrasttbl2_offsets::
	.db	0x07
_custom_contrasttbl2_offsets_len::  .db  1

_custom_backlighttbl::
	.db	0x00, 0x05, 0x0a, 0x0f, 0x14, 0x19, 0x1e, 0x23, 0x28, 0x2d
	.db	0x32, 0x37, 0x3c, 0x41, 0x46, 0x4b, 0x50, 0x55, 0x5a, 0x5f
	.db	0x64, 0x69
_custom_backlighttbl_len::  .db  22

_custom_backlighttbl2::
_custom_backlighttbl2_len::  .db  0

_custom_initseq::
	.db	0x00, 0xb0, 0x64, 0x11, 0xb0, 0x0a, 0x10, 0xb0, 0x64, 0x11
	.db	0xb0, 0xff, 0x71, 0x11, 0xb0, 0x78, 0x71, 0x06, 0x71, 0x07
	.db	0x71, 0xb1, 0x91, 0x00, 0x91, 0x12, 0x71, 0xb4, 0x91, 0x02
	.db	0x71, 0xb6, 0x91, 0x00, 0x91, 0xa0, 0x91, 0x27, 0x91, 0x00
	.db	0x71, 0xc0, 0x91, 0x13, 0x71, 0xc1, 0x91, 0x51, 0x91, 0xdd
	.db	0x91, 0x75, 0x71, 0xca, 0x91, 0x21, 0x71, 0xcb, 0x91, 0x03
	.db	0x71, 0xc4, 0x91, 0x00, 0x71, 0xc5, 0x91, 0xc3, 0x71, 0xc6
	.db	0x91, 0x00, 0x71, 0xc7, 0x91, 0x06, 0x71, 0xc8, 0x91, 0x34
	.db	0x71, 0xcc, 0x91, 0x0a, 0x71, 0xcd, 0x91, 0x1d, 0x71, 0xce
	.db	0x91, 0x00, 0x71, 0xcf, 0x91, 0x00, 0x71, 0xf9, 0x91, 0x15
	.db	0x91, 0x15, 0x71, 0xf3, 0x91, 0x06, 0x91, 0x02, 0x91, 0x06
	.db	0x71, 0xf2, 0x91, 0x00, 0x71, 0xe0, 0x91, 0x70, 0x71, 0xe1
	.db	0x91, 0x15, 0x71, 0xe2, 0x91, 0x77, 0x71, 0xe3, 0x91, 0x70
	.db	0x71, 0xe4, 0x91, 0x80, 0x71, 0xe5, 0x91, 0xe0, 0x71, 0xe6
	.db	0x91, 0x00, 0x71, 0xe7, 0x91, 0x36, 0x71, 0xe8, 0x91, 0x70
	.db	0x71, 0xe9, 0x91, 0x07, 0x71, 0xea, 0x91, 0xc0, 0x71, 0xeb
	.db	0x91, 0x80, 0x71, 0xfa, 0x71, 0xfb, 0xb0, 0x78, 0x71, 0x36
	.db	0x91, 0x68, 0x71, 0x3a, 0x91, 0x05, 0x71, 0x29, 0xb0, 0x64
	.db	0x71, 0x2c, 0xff
_custom_initseq_len::  .db  183

