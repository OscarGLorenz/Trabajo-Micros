;*************************************************************
;
; FICHERO: monedero_asm.S
;
; DESCRIPCIÓN :
; 	Contiene las funciones en ensamblador que se llaman en las
;	interrucpiones ante cambio de SO2 y SO3
;
; AUTORES:
;
; 	Marta Centeno Tortajada
;
;   Mario Musicò Cortés
;
;
;**************************************************************

#include <avr/io.h>

.extern coin_state
.extern t_coin
.extern t2u
.extern t2d
.extern t3u
.extern pin_sensor

.global asm_ISR_SO2
	asm_ISR_SO2:			; Función llamada por la interrupción una vez leídos PIND y micros()
		PUSH R17				; Guardamos estado de los registros que usaremos
		LDS R17, pin_sensor		; Comprobamos si en SW2 ha habido flanco de subida o bajada
		ANDI R17, 0x02			; Máscara
		BREQ SO2_down			; Si la lectura de SW2 tras el flanco es 0 entonces es flanco de bajada
	SO2_up:						; Si no, es flanco de subida
		LDS R17, coin_state		; Comprobamos que estamos en coin_state = 0 (nada)
		CPI R17, 0x00			; Comparación
		BRNE ESPURIO_2			; Si no estábamos en espera, ha sido ruido!
		LDS R17, t_coin			; Cargamos la lectura t_coin = micros() para asignársela a t2u
		STS t2u, R17			; ...
		LDS R17, t_coin + 1		; t_coin y t2u son de 4 bytes!
		STS t2u + 1 , R17		; ...
		LDS R17, t_coin + 2		; ...
		STS t2u + 2 , R17		; ...
		LDS R17, t_coin + 3		; ...
		STS t2u + 3 , R17		; ...
		LDI R17, 0x01			; Ahora la moneda ha entrado en SO2 por lo que pasamos a coin_state = 1
		STS coin_state, R17		; ...
		RJMP END_2				; Podemos terminar rutina en ensamblador :)
	SO2_down:
		LDS R17, coin_state		; Comprobamos que estamos en coin_state = 2 que corresponde a cuando la moneda sale de SO2
		CPI R17, 0x02			; ...
		BRNE ESPURIO_2			; Si no estábamos en coin_state = 2 es que ese flanco de bajada en SO2 es ruido
		LDS R17, t_coin			; Si se ha confirmado que no es ruido, asignamos t_coin a t2d
		STS t2d, R17			; ...
		LDS R17, t_coin + 1		; ...
		STS t2d + 1 , R17		; ...
		LDS R17, t_coin + 2		; ...
		STS t2d + 2 , R17		; ...
		LDS R17, t_coin + 3		; ...
		STS t2d + 3 , R17		; ...
		LDI R17, 0x03			; Ahora la moneda ha salido de SO2 por lo que pasamos a coin_state = 3
		STS coin_state, R17		; ...
		RJMP END_2				; Volvemos a la ISR de monedero.c para comparar con ratios calibrados
	ESPURIO_2:
		LDI R17, 0x00			; Ha sido ruido! Reseteamos coin_state = 0 e ignoramos lo que ha pasao
		STS coin_state, R17		; ...
	END_2:
		POP R17					; Recuperamos el estado de los registros que hemos usado
		RET						; Volvemos a la ISR de monedero.c

.global asm_ISR_SO3
	asm_ISR_SO3:			; Función análoga a asm_ISR_SO3 pero llamada con los flancos de SO3, consultar comentarios arriba
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
