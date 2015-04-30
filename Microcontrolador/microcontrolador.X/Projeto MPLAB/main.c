#include <p18f4550.h>
#include <stdio.h>
#include <adc.h>
#include <delays.h>    				 	// Biblioteca para gerar atrasos 
#include <usart.h>					//Biblioteca da USART


#pragma config FOSC = HS    // Usando crystal 20 MHz
#pragma config WDT = OFF
#pragma config MCLRE = ON  // Habilitando MCLR

#define LED PORTAbits.RA0		//Define o pino RA0 como LED
#define TX	PORTCbits.RC6			//Define o pino C6 como transmissor
#define RX	PORTCbits.RC7			//Define o pino C7 como receptor

void ConfiguraSistema(void); 
void ConfiguraUSART(void);
void TransmitirMensagem(char *);

unsigned int temp_externa;
unsigned int lumi_esterna;
char msg[];


void main(){
  	
	char dado[] = {'H', 'E', 'L', 'L', 'O'};
	ConfiguraSistema(); 
	ConfiguraUSART();
	TransmitirMensagem(dado);
	LED = 0;

    OpenADC(ADC_FOSC_RC & ADC_RIGHT_JUST & ADC_2_TAD, ADC_CH0 & ADC_VREFPLUS_VDD & ADC_VREFMINUS_VSS  & ADC_VREFPLUS_EXT & ADC_VREFMINUS_EXT, ADC_3ANA);

    /* BITS DE ENTRADA ANALÓGICA DO AMBIENTE EXTERNO*/
    
    TRISAbits.TRISA1= 1; //ENTRADA DA LUMINOSIDADE
    TRISAbits.TRISA2 = 1; //ENTRADA DA BATERIA

    while(1){
       // ADCON0bits.GO_DONE = 1;

        PORTDbits.RD0 = 1;
        PORTDbits.RD1 = 1;
        PORTDbits.RD2 = 1;
        SetChanADC(ADC_CH0);
        ConvertADC();
        while(BusyADC());
        
        temp_externa = ReadADC();

        SetChanADC(ADC_CH1);
        ConvertADC();
        while(BusyADC());
        lumi_esterna = ReadADC();


       // while(ADCON0bits.GO_DONE != 0);

        //temperatura_externa = ADRES;
        PORTDbits.RD0 = 0;
    }
}

void ConfiguraSistema(void)
{	   
	//Configura como oscilador externo - 20Mhz
	OSCCONbits.SCS0 = 0;
	OSCCONbits.SCS1 = 0;

	
	//Define TX como transmissor (saída)
	TRISCbits.TRISC6 = 0; 
	
	//Define RX como receptor (entrada)
	TRISCbits.TRISC7 = 1; 

	//Configura teclas como entrada = 1 ou saida = 0
	TRISAbits.TRISA0 = 0;  //LED

	 // USART configurações de interrupção
    RCONbits.IPEN   = 1; // desabilita interrupções de alta prioridade 
    INTCONbits.GIE  = 1; // habilita interrupções
    INTCONbits.PEIE = 1; // habilita perifericos
    PIE1bits.RCIE   = 1; // habilita interrupção de recepção serial do USART
	
	
}

void ConfiguraUSART(void)
{	   
	OpenUSART(USART_TX_INT_OFF & //desabilita interrupção de transmissão
			  USART_RX_INT_ON & //habilita interrupção de recepção
			  USART_ASYNCH_MODE & //modo assíncrono
			  USART_EIGHT_BIT & //transmissão e recepção em 8 bits
			  USART_BRGH_LOW, //Baud Rate em baixa velocidade
			  31);//SPBRG = Gerador de taxa de transmissão.

  
			  
	baudUSART(BAUD_8_BIT_RATE & //Gerador de 8bits
			  BAUD_AUTO_OFF & //Auto baud rate desabilitado
			  BAUD_WAKEUP_OFF);  //Desabilita o auto-wake-up

}

void TransmitirMensagem(char *msg){
	putsUSART(msg);
	while (BusyUSART());     //Aguarda o final da transmissão
         Delay10KTCYx(50);
		
}

#pragma code receberMensagem = 0x08 // armazena o código abaixo no endereço 0x08
#pragma interrupt receberMensagem   // permite que o compilador saiba que a função  receberMensagem é uma interrupção

void receberMensagem(void)
{

     if(PIE1bits.RCIE && PIR1bits.RCIF)
     {
        while(!DataRdyUSART());
		
		getsUSART(msg, 30);	
		
	/*	if(msg[0] == ~'E' && msg[1] == ~'C' && msg[2] == ~'O' && msg[3] == ~'M' && msg[4] == ~'P' && 
			msg[5] == ~'U' && msg[6] == ~'E' && msg[7] == ~'F' && msg[8] == ~'S' &&
		    msg[9] == ~'I' && msg[10] == ~'N' && msg[11] == ~'F' && msg[12] == ~'O' && msg[13] == ~'R' && msg[14] == ~'M' && msg[15] == ~'A' && msg[16] == ~'C' && msg[17] == ~'O' && msg[18] == ~'E' && msg[19] == ~'S' &&
			msg[20] == ~'2' && msg[21] == ~'0' && msg[22] == ~'1' && msg[23] == ~'5')
		{
			LED = 1;
			Delay10KTCYx(1000);
			LED = 0;
			
		}
		else*/ if (msg[0] == ~'E' && msg[1] == ~'C' && msg[2] == ~'O' && msg[3] == ~'M' && msg[4] == ~'P' && 
			msg[5] == ~'U' && msg[6] == ~'E' && msg[7] == ~'F' && msg[8] == ~'S' &&
		    msg[9] == ~'T' && msg[10] == ~'E' && msg[11] == ~'M' && msg[12] == ~'P' && msg[13] == ~'E' && msg[14] == ~'R' && msg[15] == ~'A' && msg[16] == ~'T' && msg[17] == ~'U' && msg[18] == ~'R' && msg[19] == ~'A' &&
			msg[20] == ~'2' && msg[21] == ~'0' && msg[22] == ~'1' && msg[23] == ~'5'){
				
				LED = 1;
				Delay10KTCYx(1000);
				LED = 0;
		}
		PIR1bits.RCIF = 0;
    }    
}

#pragma code // return to the default code section
