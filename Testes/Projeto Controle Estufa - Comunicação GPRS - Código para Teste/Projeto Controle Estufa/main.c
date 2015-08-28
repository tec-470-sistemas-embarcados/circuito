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
#define rec PORTBbits.RB2  // for receiving
/*****************************************************************************************************/

#pragma config PLLDIV = 3
#pragma config CPUDIV = OSC1_PLL2
#pragma config FOSC = HS
#pragma config MCLRE = ON
#pragma config WDT = OFF , DEBUG = OFF
#pragma config LVP = OFF
#pragma config PWRT = OFF 
#pragma config PBADEN = OFF

/*#pragma config FOSC = HS    // Usando crystal 20 MHz
#pragma config WDT = OFF   // Desabilitando Watchdog timer
#pragma config MCLRE = ON  // Habilitando MCLR
*/

//Formato padrão do sms

unsigned char ch = 0;
int i = 0, k;

unsigned char welcome[]=" Welcome ";
unsigned char choice[]="Send or receive? "; 
unsigned char sending[]=" Sending .. ";
unsigned char waiting[]=" Waiting for SMS";
unsigned char sms_received[]=" SMS Received ";
unsigned char character_set[]="AT+CSCS=\"GSM\"\r";

unsigned char sms_format[]="AT+CMGF=1\r";   // TEXT mode
unsigned char sms_write[]="AT+CMGS=\"XXXXXXXXXX\"\r";  // 10-Digit Mobile Number
unsigned char sms[]="Hello\r";
unsigned char sms_report[]="SMS Sent...";
unsigned char sms_indication[]="AT+CNMI=1,2,0,0,0\r";

unsigned char stringArray[6]; //to store the received sms information
unsigned char sms_terminate=0x1A;  //Ctrl+z
unsigned char enter=0x0D;  // Enter Key

unsigned char j;
unsigned char data[90];
unsigned char timer = 0;

/*********************************************PROTÓTIPOS**********************************************/
//Funções responsáveis pela configuração do PIC e periféricos

void ConfiguraSistema(void); 
void ConfiguraUSART(void);
void Transmitir(unsigned char msg[], int tam); // to send any string to TX pin
void Tx_Mensagem(unsigned char msg_serial); // Transmit data function
void Rx_Mensagem(void); //receiving data function

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
	unsigned char pass_flag = 0;
	
	ConfiguraSistema(); 
	ConfiguraUSART();

	OpenXLCD();
	SetDDRamAddr(0x00);   
	putrsXLCD(" Welcome ");
	
	while(1)		
	{
		OpenXLCD();
		SetDDRamAddr(0x00);   
		putrsXLCD("Send or receive? "); // display " Send or Receive ? "

		if(send == 1) // if sending switch is pressed 
		{
			OpenXLCD();
			SetDDRamAddr(0x00);   
			putrsXLCD(" Sending .. "); // display "sending.."
			
			pass_flag = 0;
			while (!pass_flag)
			{
				for(j = 89; j > 0; j--){
					data[j] = ' ';
				}
				Transmitir(sms_format, sizeof(sms_format)); // send to GSM ( set Text mode)
				Delay10KTCYx(250); //Tempo determinado pelo datasheet SIM300 pg 24
    			Delay10KTCYx(250);
				if(data[0] == ~'O' && data[1] == ~'K'){
					pass_flag = 1;
				}
			}
			
			pass_flag = 0;
			while (!pass_flag)
			{
				for(j = 89; j > 0; j--) data[j] = ' ';
				Transmitir(character_set, sizeof(character_set)); // send to GSM 
    			Delay10KTCYx(250);
    			Delay10KTCYx(250);
				if(data[0] == ~'O' && data[1] == ~'K'){
					pass_flag = 1;
				}
			}
			
			Transmitir(sms_write, sizeof(sms_write)); // send to GSM ( to write phone number)
    		Delay10KTCYx( 250);
            Delay10KTCYx( 250);
            Delay10KTCYx( 250);
            Delay10KTCYx( 250);
            Delay10KTCYx( 250);
			
			
    		Transmitir(sms, sizeof(sms)); // send to GSM ( to write the sms)
			Delay10KTCYx(250); 
			Delay10KTCYx(250);

			Tx_Mensagem(sms_terminate);  // ctrl+z
			Delay10KTCYx(250);
    		Delay10KTCYx(250);	
			
			Tx_Mensagem(enter); // enter Key
    		Delay10KTCYx(250);

			OpenXLCD();
			SetDDRamAddr(0x00);   
			putrsXLCD("SMS Sent..."); // display "SMS Sent .."
		}	
	
		if(rec == 1) //if Receiving switch is pressed 
        {
			OpenXLCD();
			SetDDRamAddr(0x00);   
			putrsXLCD(" Waiting for SMS"); // display " waiting for sms"

			Transmitir(sms_format, sizeof(sms_format)); // to set the mode           
			Delay10KTCYx(250);
    		Delay10KTCYx(250);
			Delay10KTCYx(250);

			Transmitir(sms_indication, sizeof(sms_indication)); // to choose how sms arrive
			Delay10KTCYx(250);
    		Delay10KTCYx(250);
			Delay10KTCYx(250);
			Rx_Mensagem();
			for(k = 0; k < 46; k++)
            {
				stringArray[k] = ch;
            }
			// --- By this point the SMS is Received and stored in Array --

			OpenXLCD();
			SetDDRamAddr(0x00);   
			putrsXLCD(" SMS Received "); // display ( sms received )
			Transmitir(stringArray, sizeof(stringArray)); // to set the mode           
			Delay10KTCYx(250);
    		Delay10KTCYx(250);
			Delay10KTCYx(250);
			/*--- display SMS on LCD ---*/
			OpenXLCD();
			SetDDRamAddr(0x00);   
			putrsXLCD(stringArray); // display the text that received
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
	TRISBbits.RB2 = 1; // To Receive an SMS

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

	/*// serial communicatin configuration 

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
	data[j++] = RCREG;*/
	
     if(PIE1bits.RCIE && PIR1bits.RCIF)
     {
		timer = 0;
        while(!DataRdyUSART());
		data[j++] = getcUSART();
		
		PIR1bits.RCIF = 0;
    }
}

#pragma code // return to the default code section
