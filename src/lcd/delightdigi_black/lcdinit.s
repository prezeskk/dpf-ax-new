	.include 'dpf.inc'

	.area LCDAUX (CODE)

_lcd_custom_init::
	mov	dptr,#_custom_initseq
	ljmp	_lcd_init_by_table

	.area LCDAUX (CODE)

_custom_initseq::
  .db	#0x00, #0xb0, #0x64, #0x11, #0xb0, #0x0a, #0x10, #0xb0, #0x64, #0x11, #0xb0, #0x64
  .db	#0x71, #0x2c, #0xb0, #0x0a, #0x71, #0xeb, #0xb0, #0x14, #0x71, #0x02, #0x71, #0x01
  .db   #0xb0, #0x0a, #0x71, #0x26, #0x71, #0x01, #0xb0, #0x0a, #0x71, #0x26, #0x71, #0x09
  .db   #0xb0, #0x0a, #0x71, #0x26, #0x71, #0x0b, #0xb0, #0x0a, #0x71, #0x26, #0x71, #0x0f
  .db   #0xb0, #0x0a, #0x71, #0x40, #0x71, #0x80, #0xb0, #0x0a, #0x71, #0x18, #0x71, #0x00 
  .db   #0xb0, #0x0a, #0x71, #0x2c, #0xb0, #0x0a, #0x71, #0x02, #0x71, #0x00, #0xb0, #0x0a  
  .db   #0x71, #0x2a, #0x71, #0x5e, #0xb0, #0x0a, #0x71, #0x10, #0x71, #0x0a, #0xb0, #0x0a
  .db   #0x71, #0x28, #0x71, #0x00, #0xb0, #0x0a, #0x71, #0x30, #0x71, #0x0b, #0xb0, #0x0a
  .db   #0x71, #0x32, #0x71, #0x06, #0xb0, #0x0a, #0x71, #0x34, #0x71, #0x91, #0xb0, #0x0a
  .db   #0x71, #0x36, #0x71, #0x00, #0xb0, #0x0a, #0x71, #0x45, #0x71, #0x00, #0xb0, #0x0a
  .db   #0x71, #0x53, #0x71, #0x00, #0xb0, #0x0a, #0x71, #0x56, #0x71, #0x00, #0xb0, #0x0a 
  .db   #0x71, #0x57, #0x71, #0x83, #0xb0, #0x0a, #0x71, #0x70, #0x71, #0x1c, #0xb0, #0x0a
  .db   #0x71, #0x7f, #0x71, #0x50, #0xb0, #0x0a, #0x71, #0x51
  .db #0xFF



