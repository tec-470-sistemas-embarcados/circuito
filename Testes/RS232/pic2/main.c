#include <p18f4550.h>   					// Biblioteca com as defini��es do PIC 18F4550

#include <delays.h>    				 	// Biblioteca para gerar atrasos 

#include <usart.h>					//Biblioteca da USART

#include <stdio.h>			//Biblioteca padr�o de entrada e sa�da.						

#define LED PORTAbits.RA0		//Define o pino RA0 como LED
#define TX	PORTCbits.RC6			//Define o pino C6 como transmissor
#define RX	PORTCbits.RC7			//Define o pino C7 como receptor

#pragma config FOSC = HS    // Usando crystal 20 MHz
#pragma config WDT = OFF   // Desabilitando Watchdog timer
#pragma config MCLRE = ON  // Habilitando MCLR

void ConfiguraSistema(void); 
void ConfiguraUSART(void);
void TransmitirMensagem(char *); 

char msg[];

void main(void)
{
	//char msg[24] = {'E', 'C', 'O', 'M', 'P', 'U', 'E', 'F', 'S', 'I', 'N', 'F', 'O', 'R', 'M', 'A', 'C', 'O', 'E', 'S', '2', '0', '1', '5'};	
	char msg[24] = {'E', 'C', 'O', 'M', 'P', 'U', 'E', 'F', 'S', 'T', 'E', 'M', 'P', 'E', 'R', 'A', 'T', 'U', 'R', 'A', '2', '0', '1', '5'};	
		
	//Configura os registradores do PIC e receptor USART
	ConfiguraSistema();
	ConfiguraUSART();
	//TransmitirMensagem(msg);
	
	
	LED = 0;

	while(1)
	{	 			  
			
 	}

}
void ConfiguraSistema(void)
{	   
	//Configura como oscilador externo - 20Mhz
	OSCCONbits.SCS0 = 0;
	OSCCONbits.SCS1 = 0;

	
	//Define TX como transmissor (sa�da)
	TRISCbits.TRISC6 = 0; 
	
	//Define RX como receptor (entrada)
	TRISCbits.TRISC7 = 1; 

	//Configura teclas como entrada = 1 ou saida = 0
	TRISAbits.TRISA0 = 0;  //LED

	 // USART configura��es de interrup��o
    RCONbits.IPEN   = 1; // desabilita interrup��es de alta prioridade 
    INTCONbits.GIE  = 1; // habilita interrup��es
    INTCONbits.PEIE = 1; // habilita perifericos
    PIE1bits.RCIE   = 1; // habilita interrup��o de recep��o serial do USART
	
	
}

void ConfiguraUSART(void)
{	   
	OpenUSART(USART_TX_INT_OFF & //desabilita interrup��o de transmiss�o
			  USART_RX_INT_ON & //habilita interrup��o de recep��o
			  USART_ASYNCH_MODE & //modo ass�ncrono
			  USART_EIGHT_BIT & //transmiss�o e recep��o em 8 bits
			  USART_BRGH_LOW, //Baud Rate em baixa velocidade
			  31);//SPBRG = Gerador de taxa de transmiss�o.

  
			  
	baudUSART(BAUD_8_BIT_RATE & //Gerador de 8bits
			  BAUD_AUTO_OFF & //Auto baud rate desabilitado
			  BAUD_WAKEUP_OFF);  //Desabilita o auto-wake-up

}
void TransmitirMensagem(char *msg){
	putsUSART(msg);
	while (BusyUSART());     //Aguarda o final da transmiss�o
         Delay10KTCYx(50);
		
}

#pragma code receberMensagem = 0x08 // armazena o c�digo abaixo no endere�o 0x08
#pragma interrupt receberMensagem   // permite que o compilador saiba que a fun��o  receberMensagem � uma interrup��o

void receberMensagem(void)
{

     if(PIE1bits.RCIE && PIR1bits.RCIF)
     {
        while(!DataRdyUSART());
		
		getsUSART(msg, 5);	
		
		if (msg[0] == ~'H' && msg[1] == ~'E' && msg[2] == ~'L' && msg[3] == ~'L' && msg[4] == ~'O'){
				
				LED = 1;
				Delay10KTCYx(1000);
				LED = 0;
		}
        PIR1bits.RCIF = 0;
    }    
	
}

#pragma code // return to the default code section
