#include "i8042.h"

.intel_syntax

.global _kbc_asm_handler
.global _g_keycode

.text
	_kbc_asm_handler:
				pushad
	bufferNotFull:	xor eax, eax
				in al, STATUS_REGISTER
				test al, OUT_BUFFER_READY
				jz bufferNotFull
				xor eax, eax
				in al, OUT_BUFF
				mov _g_keycode, al

				//mov al, EOI
				//out PIC1_CMD, al
				popad
				ret




