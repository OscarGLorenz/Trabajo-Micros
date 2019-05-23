#include <avr/io.h>

.extern coin_state
.extern t_coin
.extern t2u
.extern t2d
.extern t3u
.extern pin_sensor


.global asmISRSO2

	asmISRSO2:
		PUSH R17
		LDS R17, pin_sensor
		ANDI R17, 0x02
		BREQ SO2_down
	SO2_up:
		LDS R17, coin_state
		CPI R17, 0x00
		BRNE ESPURIO_2
		LDS R17, t_coin
		STS t2u, R17
		LDS R17, t_coin + 1
		STS t2u + 1 , R17
		LDS R17, t_coin + 2
		STS t2u + 2 , R17
		LDS R17, t_coin + 3
		STS t2u + 3 , R17
		LDI R17, 0x01
		STS coin_state, R17
		RJMP END_2
	SO2_down:
		LDS R17, coin_state
		CPI R17, 0x02
		BRNE ESPURIO_2
		LDS R17, t_coin
		STS t2d, R17
		LDS R17, t_coin + 1
		STS t2d + 1 , R17
		LDS R17, t_coin + 2
		STS t2d + 2 , R17
		LDS R17, t_coin + 3
		STS t2d + 3 , R17
		LDI R17, 0x03
		STS coin_state, R17
		RJMP END_2
	ESPURIO_2:
		LDI R17, 0x00
		STS coin_state, R17		
	END_2:
		POP R17
		RET

.global asmISRSO3
		
	asmISRSO3:
		PUSH R17
		LDS R17, pin_sensor
		ANDI R17, 0x04
		BREQ ESPURIO_3
	SO3_up:
		LDS R17, coin_state
		CPI R17, 0x01
		BRNE ESPURIO_3
		LDS R17, t_coin
		STS t3u, R17
		LDS R17, t_coin + 1
		STS t3u + 1 , R17
		LDS R17, t_coin + 2
		STS t3u + 2 , R17
		LDS R17, t_coin + 3
		STS t3u + 3 , R17
		LDI R17, 0x02
		STS coin_state, R17
		RJMP END_3
	ESPURIO_3:
		LDI R17, 0x00
		STS coin_state, R17		
	END_3:
		POP R17
		RET
