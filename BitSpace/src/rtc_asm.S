#include "RTC_Macros.h"

.intel_syntax

.global _rtc_asm_handler
.globl _getRtc


.text
	_rtc_asm_handler:
		pushad

		xor eax, eax
		mov al, RTC_REG_C
		out RTC_ADDR_REG, al


		xor eax, eax
		in 	al, RTC_DATA_REG

		test al, RTC_AF
		jz cont

		call  _getRtc
		mov esi, eax
		mov al, [esi]
		or al, 1
		mov [esi], al

		cont:

		popad
		ret
