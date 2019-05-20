#include "macros.h"

.extern coin_state
.extern t
.extern t2u
.extern t2d
.extern t3u
.extern millis_cnt

.global asm_ISR_SO2

	asm_ISR_SO2:
		PUSH R17
		IN R17, PIND
		ANDI R17, 0x02
		BRNE SO2_down		// SI FLANCO BAJADA
	SO2_up:
		LDS R17, coin_state
		CPI R17, 0x00
		BRNE ESPURIO
		LDS R17, t
		STS t2u, R17
		LDS R17, t + 1
		STS t2u + 1 , R17
		LDS R17, t + 2
		STS t2u + 2 , R17
		LDS R17, t + 3
		STS t2u + 3 , R17
		LDI R17, 0x01
		STS coin_state, R17
		RJMP END
	SO2_down:
		LDS R17, coin_state
		CPI R17, 0x02
		BRNE ESPURIO
		LDS R17, t
		STS t2d, R17
		LDS R17, t + 1
		STS t2d + 1 , R17
		LDS R17, t + 2
		STS t2d + 2 , R17
		LDS R17, t + 3
		STS t2d + 3 , R17
		LDI R17, 0x03
		STS coin_state, R17
		RJMP END
		
	asm_ISR_SO3:
		PUSH R17
		IN R17, PIND
		ANDI R17, 0x03
		BRNE SO3_down		// SI FLANCO BAJADA
	SO3_up:
		LDS R17, coin_state
		CPI R17, 0x01
		BRNE ESPURIO
		LDS R17, t
		STS t3u, R17
		LDS R17, t + 1
		STS t3u + 1 , R17
		LDS R17, t + 2
		STS t3u + 2 , R17
		LDS R17, t + 3
		STS t3u + 3 , R17
		LDI R17, 0x02
		STS coin_state, R17
		RJMP END
	SO3_down:
		LDS R17, coin_state
		CPI R17, 0x03
		BRNE ESPURIO
		LDI R17, 0x04
		STS coin_state, R17
		RJMP END
		
	ESPURIO:
		LDI R17, 0x00
		STS coin_state, R17		
	END:
		POP R17
		RET