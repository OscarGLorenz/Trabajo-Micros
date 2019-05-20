#include<avr/io.h>

void UART1_Init() {

	UBRR1H = 0x00;	//set baud rate = 9600 bps
	UBRR1L = 103;	//0x67
	//set 1 stop bit, no parity bit and 8 bit charachter size
	UCSR1C = 0x06;         //(1 << UCSZ1)|(1 << UCSZ0)   
	//enable transmission and reception
	UCSR1B = 0x18;         //(1 << RXEN)|(1 << TXEN)
}

//Transmit character through UART
void UART1_Tx_Char(unsigned char data) {
	UDR1 = data; //put the data to be transmitted into the UDR register
	while(!(UCSR1A&(1<<UDRE))); //wait until the transmission is completed
}


//Transmit string through UART
void UART1_Tx_Str(unsigned char * str) {
	while(*str) UDR1 = *str++;
	while(!(UCSR1A&(1<<UDRE);
}

//Receive a character through UART 
unsigned char UART1_Rx_Char() {
	while(!(UCSR1A & (1<<RXC))); //wait for the charater
	return(UDR1); //return the received charater
}

//Receive string through UART
unsigned char * UART1_Rx_Str() {
	unsigned char string[20], x, i = 0;
	while((x = UART1_Rx_Char()) != 13){ //receive the characters until ENTER is pressed (ASCII for ENTER = 13)
		string[i++] = x; //and store the received characters into the array string[] one-by-one
	}
	string[i] = '\0'; //insert NULL to terminate the string
	return(string); //return the received string
}

void main() {
	UART1_Init();	//initialize the UART circuitry 
	UART1_Tx_Str( UART1_Rx_Str());	//Transmit the Received string onto the UART again
}