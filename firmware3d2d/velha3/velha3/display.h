#ifndef DISPLAY_H
#define DISPLAY_H

#include "relogio.h"

uint8_t LEDS[27][3];
uint8_t BOTOES[27];

#define MAXIMO_CONTAGEM 10 //*9/16000 => 17Hz
uint8_t CONTAGEM, COLUNA;
/*
0  1  2    9  10 11    18 19 20
3  4  5    12 13 14    21 22 23 top
6  7  8    15 16 17    24 25 26
*/
#define DDR_SERIAL	DDRB
#define PORT_SERIAL	PORTB
#define PINO_DATA	PORTB0
#define PINO_CLK	PORTB1
#define DDR_BOTAO	DDRD
#define PIN_BOTAO	PIND
#define PINO_BOTAO0	PORTD4
#define PINO_BOTAO1	PORTD3
#define PINO_BOTAO2	PORTD2

void inicializar_display(){
	//Serial
	DDR_SERIAL  |=  (1<<PINO_CLK)|(1<<PINO_DATA);
	PORT_SERIAL &=~((1<<PINO_CLK)|(1<<PINO_DATA));
	//Botoes
	DDR_BOTAO &=~((1<<PINO_BOTAO0)|(1<<PINO_BOTAO1)|(1<<PINO_BOTAO2));
	PORTD |= (1<<PINO_BOTAO0)|(1<<PINO_BOTAO1)|(1<<PINO_BOTAO2);
	MCUCR &=~(1<<PUD);
	//LEDS
	DDRD |= (1<<5)|(1<<6)|(1<<7);
	DDRC |= 0b00111111;
	
	CONTAGEM = 0;
	COLUNA = 0;
	for(uint8_t i=0;i<27;i++){
		LEDS[i][0]=0;
		LEDS[i][1]=0;
		LEDS[i][2]=0;
		BOTOES[i]=0;
	}
}
#define NOP() __asm__ __volatile__ ("nop")
/*inline void serial_out(uint16_t shift){
	for(uint8_t i=0; i<9; i++){
		if(shift&1)PORTD|= (1<<PINO_DATA);
		else       PORTD&=~(1<<PINO_DATA);
		shift=(shift>>1)&0x7FFF;
		PORTD|= (1<<PINO_CLK);
		NOP();NOP();
		PORTD&=~(1<<PINO_CLK);
		NOP();NOP();
	}
	PORTD|= (1<<PINO_STROBE);
	NOP();NOP();NOP();NOP();
	PORTD&=~(1<<PINO_STROBE);
}*/
void interrupt_tick(){
	//uint16_t shift = 0;
	CONTAGEM++;
	if(CONTAGEM>=MAXIMO_CONTAGEM){
		if(PIN_BOTAO&(1<<PINO_BOTAO0))BOTOES[COLUNA+0 ]=0;
		else                          BOTOES[COLUNA+0 ]=1;
		if(PIN_BOTAO&(1<<PINO_BOTAO1))BOTOES[COLUNA+9 ]=0;
		else                          BOTOES[COLUNA+9 ]=1;
		if(PIN_BOTAO&(1<<PINO_BOTAO2))BOTOES[COLUNA+18]=0;
		else                          BOTOES[COLUNA+18]=1;
		CONTAGEM = 0;
		COLUNA++;
		if(COLUNA>=9){
			COLUNA=0;
		}
		
		PORT_SERIAL|= (1<<PINO_CLK);
		NOP();NOP();
		PORT_SERIAL&=~(1<<PINO_CLK);
		NOP();NOP();
		if(COLUNA==8)PORT_SERIAL|= (1<<PINO_DATA);
		else         PORT_SERIAL&=~(1<<PINO_DATA);
	}
	if(CONTAGEM<LEDS[COLUNA+0 ][0])PORTD |= (1<<7);
	else                           PORTD &=~(1<<7);
	if(CONTAGEM<LEDS[COLUNA+0 ][1])PORTD |= (1<<6);
	else                           PORTD &=~(1<<6);
	if(CONTAGEM<LEDS[COLUNA+0 ][2])PORTD |= (1<<5);
	else                           PORTD &=~(1<<5);
	if(CONTAGEM<LEDS[COLUNA+9 ][0])PORTC |= (1<<5);
	else                           PORTC &=~(1<<5);
	if(CONTAGEM<LEDS[COLUNA+9 ][1])PORTC |= (1<<4);
	else                           PORTC &=~(1<<4);
	if(CONTAGEM<LEDS[COLUNA+9 ][2])PORTC |= (1<<3);
	else                           PORTC &=~(1<<3);
	if(CONTAGEM<LEDS[COLUNA+18][0])PORTC |= (1<<2);
	else                           PORTC &=~(1<<2);
	if(CONTAGEM<LEDS[COLUNA+18][1])PORTC |= (1<<1);
	else                           PORTC &=~(1<<1);
	if(CONTAGEM<LEDS[COLUNA+18][2])PORTC |= (1<<0);
	else                           PORTC &=~(1<<0);
	//shift |= (1<<(9+COLUNA));
	//serial_out(shift);
}
void apagar(){
	for(uint8_t i=0;i<27;i++){
		LEDS[i][0]=0;
		LEDS[i][1]=0;
		LEDS[i][2]=0;
	}
}
uint8_t esperarJogada(int32_t timeout){
	uint8_t ultimos_estados[27], i;
	for(i=0;i<27;i++)ultimos_estados[i]=BOTOES[i];
	while(timeout>0){
		for(i=0;i<27;i++){
			if(BOTOES[i]&&!ultimos_estados[i])return i;
		}
		for(uint8_t i=0;i<27;i++)ultimos_estados[i]=BOTOES[i];
		_delay_ms(50);
		timeout -= 51;
	}
	return 30;
}
void fazerJogada(uint8_t posicao, uint8_t jogadaR, uint8_t jogadaG, uint8_t jogadaB){
	uint8_t ultima[3];
	ultima[0] = LEDS[posicao][0];
	ultima[1] = LEDS[posicao][1];
	ultima[2] = LEDS[posicao][2];
	for(int i=0;i<3;i++){
		LEDS[posicao][0] = jogadaR;
		LEDS[posicao][1] = jogadaG;
		LEDS[posicao][2] = jogadaB;
		_delay_ms(100);
		LEDS[posicao][0] = ultima[0];
		LEDS[posicao][1] = ultima[1];
		LEDS[posicao][2] = ultima[2];
		_delay_ms(100);
	}
	LEDS[posicao][0] = jogadaR;
	LEDS[posicao][1] = jogadaG;
	LEDS[posicao][2] = jogadaB;
}
void fazerJogadaV(uint8_t posicao, uint8_t jogadaR, uint8_t jogadaG, uint8_t jogadaB, uint8_t vezes){
	uint8_t ultima[3];
	ultima[0] = LEDS[posicao][0];
	ultima[1] = LEDS[posicao][1];
	ultima[2] = LEDS[posicao][2];
	for(int i=0;i<vezes;i++){
		LEDS[posicao][0] = jogadaR;
		LEDS[posicao][1] = jogadaG;
		LEDS[posicao][2] = jogadaB;
		_delay_ms(100);
		LEDS[posicao][0] = ultima[0];
		LEDS[posicao][1] = ultima[1];
		LEDS[posicao][2] = ultima[2];
		_delay_ms(100);
	}
	LEDS[posicao][0] = jogadaR;
	LEDS[posicao][1] = jogadaG;
	LEDS[posicao][2] = jogadaB;
}

#endif