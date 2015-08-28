/**********************************************INCLUDES***********************************************/
#include <p18cxxx.h>   					// Biblioteca com as definições do PIC 18F4550

#include <timers.h>    					// Biblioteca para uso dos módulos TIMERs

#include <delays.h>    				 	// Biblioteca para gerar atrasos 

#include <usart.h>

#include <stdio.h>

#include "my_xlcd.h"  					// Biblioteca desenvolvida pela PICMinas para escrever no display LCD.
/*****************************************************************************************************/

/************************************************DEFINES**********************************************/
#define TX	PORTCbits.RC6			//Define o pino C6 como transmissor
#define RX	PORTCbits.RC7			//Define o pino C7 como receptor
#define send PORTBbits.RB1  //for sending 

/*****************************************************************************************************/

#pragma config FOSC = HS    // Usando crystal 20 MHz
#pragma config WDT = OFF   // Desabilitando Watchdog timer
#pragma config MCLRE = ON  // Habilitando MCLR

//Formato padrão do sms
unsigned char sms_format[]="AT+CMGF=1\r";   // Definindo como modo de texto
unsigned char sms_write[]="AT+CMGS=\"XXXXXXXXXX\"\r";  // Digitando o número do telefone
unsigned char sms_indication[]="AT+CNMI=1,2,0,0,0\r"; // Configurando para receber msg na porta serial
unsigned char character_set[]="AT+CSCS=\"GSM\"\r"; // 

unsigned char stringArray[6]; //Armazenamento das informações dos sms recebido
unsigned char sms_terminate=0x1A;  //Ctrl+z
unsigned char enter=0x0D;  // Enter Key


unsigned char msg[] = "OK\r"; //Mensagem de Texto
int i = 0;

unsigned char j;
unsigned char data[90];
unsigned char timer = 0;


/*********************************************PROTÓTIPOS**********************************************/
//Funções responsáveis pela configuração do PIC e periféricos

void ConfiguraSistema(void); 
void ConfiguraUSART(void);
void Transmitir(unsigned char msg[], int tam); //Transmissão de Dados
void Tx_Mensagem(unsigned char msg_serial); //Transmitir alguma String para o pino TX


/*****************************************************************************************************/

/***********************************************FUNCOES***********************************************/

/******************************************************************************************************
* Funcao:    void main(void) 														  				  *
* Entrada:   Nenhuma 		 														  				  *
* Saída:     Nenhuma 		 														  				  *
* Descrição: Função principal do programa.											  				  *
******************************************************************************************************/
void main(void)
{
	unsigned char flag = 0;
	int cont = 0;	

	ConfiguraSistema(); 
	ConfiguraUSART();
	if(send == 1) // if sending switch is pressed 
	{
		Transmitir(msg, sizeof(msg)); // Enviando para GSM formato de texto
		Delay10KTCYx(250); //Tempo determinado pelo datasheet SIM300 pg 24
    	Delay10KTCYx(250);	
	}
	
/*	Transmitir(sms_format, sizeof(sms_format)); // Enviando para GSM formato de texto
	Delay10KTCYx(250); //Tempo determinado pelo datasheet SIM300 pg 24
    Delay10KTCYx(250);

    Transmitir(character_set, sizeof(character_set)); // Envia para GSM 
    Delay10KTCYx(250);
    Delay10KTCYx(250);

    Transmitir(sms_write, sizeof(sms_write)); // Envia para GSM: Escrevendo o numero do telefone
    Delay10KTCYx(250);
    Delay10KTCYx(250);
    Delay10KTCYx(250);
    Delay10KTCYx(250);
    Delay10KTCYx(250);

    Transmitir(msg, sizeof(msg)); // Enviando Mensagem
	Delay10KTCYx(250); //Tempo determinado pelo datasheet SIM300 pg 24
	Delay10KTCYx(250);	
	
	//Transmitir(sms, sizeof(sms));  // Envia para GSM: Escrevendo o sms  
    Tx_Mensagem(sms_terminate);  // Fim da transmissão. ctrl+z. So serve de teste

    Delay10KTCYx(250);
    Delay10KTCYx(250);

    Tx_Mensagem(enter);  // Envia a chave
    Delay10KTCYx(250);
*/	
	
	while(1)		
	{
		while(!flag && cont == 0){
			if(data[0] == ~'A' && data[1] == ~'T' && data[2] == ~'+' && data[3] == ~'C' && data[4] == ~'M' && data[5] == ~'G' && data[6] == ~'F' && data[7] == ~'=' && data[8] == ~'1' && data[9] == ~'\r')
			{
				cont++;
				Transmitir(msg, sizeof(msg));
				Delay10KTCYx(250); //Tempo determinado pelo datasheet SIM300 pg 24
    			Delay10KTCYx(250);
				if(cont == 1) flag = 1;
			}			
		}
		flag = 0;
		while(!flag && cont == 1){
			if(data[10] == ~'A' && data[11] == ~'T' && data[12] == ~'+' && data[13] == ~'C' && data[14] == ~'S' && data[15] == ~'C' && data[16] == ~'S' && data[17] == ~'=' && data[18] == ~'"' && data[19] == ~'G' && data[20] == ~'S' && data[21] == ~'M' && data[22] == ~'"' && data[23] == ~'\r')
			{
				cont++;
				Transmitir(msg, sizeof(msg));
			
				if(cont == 2) flag = 1;
			}			
		}

	}

}//fim main
/*****************************************************************************************************/
/******************************************************************************************************
* Funcao:		ConfiguraSistema 										  						  	  *
* Entrada:		Nenhuma											  			   						  *
* Saída:		Nenhuma														  						  *
* Descrição:	Configuração do oscilador e das portas do microcontrolador    						  *
******************************************************************************************************/
void ConfiguraSistema(void)
{	   
	//Configura como oscilador externo - 20Mhz
	OSCCONbits.SCS0 = 0;
	OSCCONbits.SCS1 = 0;     
	
	//Define TX como transmissor (saída)
	TRISCbits.TRISC6 = 0; 
	
	//Define RX como receptor (entrada)
	TRISCbits.TRISC7 = 1; 

	TRISBbits.RB1 = 1; // To send an SMS
	
	// USART configurações de interrupção
    RCONbits.IPEN   = 1; // desabilita interrupções de alta prioridade 
    INTCONbits.GIE  = 1; // habilita interrupções
    INTCONbits.PEIE = 1; // habilita perifericos
    PIE1bits.RCIE   = 1; // habilita interrupção de recepção serial do USART
	
}//fim ConfiguraSistema
/*****************************************************************************************************/

/******************************************************************************************************
* Funcao:		ConfiguraTransmissor									  						  	  *
* Entrada:		Nenhuma											  			   						  *
* Saída:		Nenhuma														  						  *
* Descrição:	Configuração do transmissor								    						  *
******************************************************************************************************/
void ConfiguraUSART(void)
{	   

/*	// serial communicatin configuration 

SPBRG= 18; //Fill SPBRG register to set the baud rate
RCSTAbits.SPEN=1;     // To activate Serial port (TX and RX pins)
TXSTAbits.TXEN=1;      // To enable transmission
RCSTAbits.CREN=1;  // To enable continuous reception
PIR1bits.RCIF =0; // clear rcif interrupt flag
PIE1bits.RCIE=1;
INTCONbits.PEIE =1; // enable peripheral interrupt
INTCONbits.GIE =1; // enable global interrupt
*/

	OpenUSART(USART_TX_INT_OFF & 
			  USART_RX_INT_ON & 
			  USART_ASYNCH_MODE &
			  USART_EIGHT_BIT &
			  USART_BRGH_LOW,31);
			  
	baudUSART(BAUD_8_BIT_RATE &
			  BAUD_AUTO_OFF &
			  BAUD_WAKEUP_OFF);

}//fim ConfiguraTransmissor

void Transmitir(unsigned char msg[], int tam){
	for(i = 0; i < tam; i++){
		Tx_Mensagem(msg[i]);
		Delay10KTCYx(250);
	}		
}

void Tx_Mensagem(unsigned char msg_serial){
	/*TXREG=msg_serial;
   	while(PIR1bits.TXIF==0);*/
	while(BusyUSART()); //Aguarda o final da transmissão 
	putcUSART(msg_serial);
	stdout = _H_USART;	
}

#pragma code Rx_Mensagem = 0x08 // armazena o código abaixo no endereço 0x08
#pragma interrupt Rx_Mensagem   // permite que o compilador saiba que a função  receberMensagem é uma interrupção

void Rx_Mensagem(void)
{
	/*while(PIR1bits.RCIF==0); // Wait until RCIF gets low
	data[j++] = RCREG;
	printf(data[j]);
	*/
     if(PIE1bits.RCIE && PIR1bits.RCIF)
     {
		timer = 0;
        while(!DataRdyUSART());
		data[j++] = getcUSART();
		
		PIR1bits.RCIF = 0;
    }
}

#pragma code // return to the default code section
