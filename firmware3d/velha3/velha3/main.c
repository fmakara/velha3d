/*
 * velha3.c
 *
 * Created: 15/05/2016 22:26:38
 * Author : Makara
 */ 

#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include "relogio.h"
#include "display.h"
#include <avr/eeprom.h>


uint8_t CORES[4][3] = {
	{MAXIMO_CONTAGEM, 0, 0},
	{0, MAXIMO_CONTAGEM, 0},
	{0, 0, MAXIMO_CONTAGEM},
	{MAXIMO_CONTAGEM, MAXIMO_CONTAGEM, 0},
};
void salvarCores(){
	for(uint8_t i=0;i<4;i++)
		for(uint8_t j=0;j<3;j++)eeprom_write_byte((uint8_t*)(j+i*3),CORES[i][j]);
}
void carregarCores(){
	for(uint8_t i=0;i<4;i++)
		for(uint8_t j=0;j<3;j++)CORES[i][j] = eeprom_read_byte((uint8_t*)(j+i*3));
}
/* 0  1  2    9  10 11    18 19 20
 * 3  4  5    12 13 14    21 22 23 top
 * 6  7  8    15 16 17    24 25 26  */
void jogarNp(uint8_t n);
void editarCor();
int main(void){
	uint8_t j;
	inicializar_relogio();
	inicializar_display();
	carregarCores();
    while (1){
		apagar();
		LEDS[18][0] = 10;
		LEDS[ 9][0] = 10;
		LEDS[20][1] = 10;
		LEDS[11][1] = 10;
		LEDS[ 2][1] = 10;
		LEDS[26][2] = 10;
		LEDS[25][2] = 10;
		LEDS[16][2] = 10;
		LEDS[17][2] = 10;
		LEDS[4][0] = 10;
		LEDS[4][1] = 10;
		LEDS[4][2] = 6;
		j = esperarJogada(10000);
		switch(j){
			case 18:
			case 9:
				fazerJogada(j,0,0,0);
				jogarNp(2);
				break;
			case 20:
			case 11:
			case 2:
				fazerJogada(j,0,0,0);
				jogarNp(3);
				break;
			case 26:
			case 25:
			case 16:
			case 17:
				fazerJogada(j,0,0,0);
				jogarNp(4);
				break;
			case 4:
				fazerJogada(j,0,0,0);
				editarCor();
				break;
		}
    }
}
/* 0  1  2    9  10 11    18 19 20
 * 3  4  5    12 13 14    21 22 23 top
 * 6  7  8    15 16 17    24 25 26  */
void editarCor(){
	uint8_t corSelecionada = 5;
	uint8_t novaCor[3];
	apagar();
	fazerJogadaV(18,CORES[0][0],CORES[0][1],CORES[0][2],1);
	fazerJogadaV(20,CORES[1][0],CORES[1][1],CORES[1][2],2);
	fazerJogadaV(26,CORES[2][0],CORES[2][1],CORES[2][2],3);
	fazerJogadaV(24,CORES[3][0],CORES[3][1],CORES[3][2],4);
	LEDS[4][0] = 10;
	LEDS[4][1] = 10;
	LEDS[4][2] = 6;
	while(corSelecionada>4){
		switch(esperarJogada(20000)){
			case 18: corSelecionada=0; fazerJogada(18,0,0,0); break;
			case 20: corSelecionada=1; fazerJogada(20,0,0,0); break;
			case 26: corSelecionada=2; fazerJogada(26,0,0,0); break;
			case 24: corSelecionada=3; fazerJogada(24,0,0,0); break;
			case 4:
			case 30: return;
		}
	}
	novaCor[0] = CORES[corSelecionada][0];
	novaCor[1] = CORES[corSelecionada][1];
	novaCor[2] = CORES[corSelecionada][2];
	apagar();
	while(1){
		LEDS[18][0] = 1;
		LEDS[20][0] = 10;
		LEDS[21][1] = 1;
		LEDS[23][1] = 10;
		LEDS[24][2] = 1;
		LEDS[26][2] = 10;
		LEDS[22][0] = novaCor[0];
		LEDS[22][1] = novaCor[1];
		LEDS[22][2] = novaCor[2];
		LEDS[4][0] = 10;
		LEDS[4][1] = 10;
		LEDS[4][2] = 6;
		switch(esperarJogada(20000)){
			case 18:
				if(novaCor[0]>0){
					novaCor[0]--;
					LEDS[18][0] = 10;
					_delay_ms(100);
				}	
				break;
			case 20:
				if(novaCor[0]<10){
					novaCor[0]++;
					LEDS[20][0] = 0;
					_delay_ms(100);
				}
				break;
			case 21:
				if(novaCor[1]>0){
					novaCor[1]--;
					LEDS[21][1] = 10;
					_delay_ms(100);
				}
				break;
			case 23:
				if(novaCor[1]<10){
					novaCor[1]++;
					LEDS[23][1] = 0;
					_delay_ms(100);
				}
				break;
			case 24:
				if(novaCor[2]>0){
					novaCor[2]--;
					LEDS[24][2] = 10;
					_delay_ms(100);
				}
				break;
			case 26:
				if(novaCor[2]<10){
					novaCor[2]++;
					LEDS[26][2] = 0;
					_delay_ms(100);
				}
				break;
			case 22:
				if(novaCor[0]==0 && novaCor[1]==0 && novaCor[2]==0){
					fazerJogada(22,10,10,10);
				}else{
					fazerJogadaV(22,0,0,0,1);
					CORES[corSelecionada][0] = novaCor[0];
					CORES[corSelecionada][1] = novaCor[1];
					CORES[corSelecionada][2] = novaCor[2];
					salvarCores();
					return;
				}
				break;
			case 4:
			case 30:
				fazerJogada(4,0,0,0);
				return;
		}
	}
}








const uint8_t VENCE[50][3] = {
	{0,0,0},
	{ 0, 1, 2},{ 3, 4, 5},{ 6, 7, 8},{ 0, 3, 6},{ 1, 4, 7},{ 2, 5, 8},{ 0, 4, 8},{ 2, 4, 6},//Camada H 0
	{ 9,10,11},{12,13,14},{15,16,17},{ 9,12,15},{10,13,16},{11,14,17},{ 9,13,17},{11,13,15},//Camada H 1
	{18,19,20},{21,22,23},{24,25,26},{18,21,24},{19,22,25},{20,23,26},{18,22,26},{20,22,24},//Camada H 2
	{ 0, 9,18},{ 1,10,19},{ 2,11,20},{ 3,12,21},{ 4,13,22},{ 5,14,23},{ 6,15,24},{ 7,16,25},{ 8,17,26},//Camada V
	{ 0,10,20},{ 2,10,18},{ 3,13,23},{ 5,13,21},{ 8,16,24},{ 6,16,26},//Camada D a
	{ 2,14,26},{ 8,14,20},{ 1,13,16},{ 7,13,19},{ 0,12,24},{ 6,12,18},//Camada D b
	
	{ 0,13,17},{18,13, 8},{ 2,13,24},{ 6,13,20}//Camada D Int
};
/* 0  1  2    9  10 11    18 19 20
 * 3  4  5    12 13 14    21 22 23 top
 * 6  7  8    15 16 17    24 25 26  */

uint8_t checa_vencedor(){
	for(uint8_t i=1;i<50;i++)
	if(	LEDS[VENCE[i][0]][0]==LEDS[VENCE[i][1]][0] && LEDS[VENCE[i][0]][0]==LEDS[VENCE[i][2]][0] &&
		LEDS[VENCE[i][0]][1]==LEDS[VENCE[i][1]][1] && LEDS[VENCE[i][0]][1]==LEDS[VENCE[i][2]][1] &&
		LEDS[VENCE[i][0]][2]==LEDS[VENCE[i][1]][2] && LEDS[VENCE[i][0]][2]==LEDS[VENCE[i][2]][2] &&
		!(LEDS[VENCE[i][0]][0]==0 && LEDS[VENCE[i][0]][1]==0 && LEDS[VENCE[i][0]][2]==0)
	)return i;
	return 0;
}
void pisca_vencedor(){
	uint8_t original[27][3],sobreposta[27][3], i, p, j;
	for(i=0;i<27;i++){
		for(j=0;j<3;j++){
			original[i][j]=LEDS[i][j];
			sobreposta[i][j]=LEDS[i][j];
		}
	}
	p = checa_vencedor();
	if(p==0)
		for(i=0;i<27;i++)
			for(j=0;j<3;j++)sobreposta[i][j]=0;
	sobreposta[VENCE[p][0]][0]=0;
	sobreposta[VENCE[p][1]][0]=0;
	sobreposta[VENCE[p][2]][0]=0;
	sobreposta[VENCE[p][0]][1]=0;
	sobreposta[VENCE[p][1]][1]=0;
	sobreposta[VENCE[p][2]][1]=0;
	sobreposta[VENCE[p][0]][2]=0;
	sobreposta[VENCE[p][1]][2]=0;
	sobreposta[VENCE[p][2]][2]=0;
	for(j=0;j<5;j++){
		for(i=0;i<27;i++){
			LEDS[i][0]=sobreposta[i][0];
			LEDS[i][1]=sobreposta[i][1];
			LEDS[i][2]=sobreposta[i][2];
		}
		_delay_ms(200);
		for(i=0;i<27;i++){
			LEDS[i][0]=original[i][0];
			LEDS[i][1]=original[i][1];
			LEDS[i][2]=original[i][2];
		}
		_delay_ms(200);
	}
}

void jogarNp(uint8_t n){
	uint8_t jogador, jogada, atual;
	apagar();
	for(jogador=0;jogador<27;jogador++){
		jogada = esperarJogada(120000);
		if(jogada>27){
			pisca_vencedor();
			return;
		}
		if(!(LEDS[jogada][0]==0 && LEDS[jogada][1]==0 && LEDS[jogada][2]==0)){
			jogador--;
		}else{
			atual = jogador%n;
			fazerJogada(jogada,CORES[atual][0],CORES[atual][1],CORES[atual][2]);
			if(checa_vencedor()){
				pisca_vencedor();
				return;
			}
		}
	}
	pisca_vencedor();
	return;
}
