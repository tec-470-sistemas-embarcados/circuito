#include <p18F4520.h>

// Frequencia do oscilador = 8MHz
// Ciclo de máquina  = 1/(Fosc/4) = 0,5us

#pragma CONFIG OSC = HS        	//Configura o oscilador a cristal
#pragma CONFIG WDT = OFF  		//Desabilita o Watchdog Timer (WDT).
#pragma CONFIG PWRT = ON   	//Habilita o Power-up Timer (PWRT).
#pragma CONFIG BOREN = ON		//Habilita Brown-out reset
#pragma CONFIG BORV = 1        	//Tensão do BOR é 4,33V.
#pragma CONFIG PBADEN = OFF 	//RB0,1,2,3 e 4 CONFIGurado como I/O digital.
#pragma CONFIG LVP = OFF       	//Desabilita o Low Voltage Program.

char conta=0;   					//Variável de controle de tempo

#pragma code int_pr = 0x000008		//vetor de interrupção de alta prioridade
#pragma interrupt trata_TIMER0		/*/Define a função trata_TIMER0 p/tratamento da
									interrupção do Timer 0*/
void trata_TIMER0(void)			//Função trata_TIMER0
{
conta++;							//A cada 5s a variável conta é incrementada
//Inverte a situação dos pinos 0, 1, 2 e 3 da porta D
PORTDbits.RD0 = ~PORTDbits.RD0;
PORTDbits.RD1 = ~PORTDbits.RD1;
PORTDbits.RD2 = ~PORTDbits.RD2;
PORTDbits.RD3 = ~PORTDbits.RD3;
if (conta>=2)						//Se esta for >= 2, segue o código abaixo
   {
      //Inverte a situação dos pinos 4, 5, 6 e 7 da porta D
     	PORTDbits.RD4 = ~PORTDbits.RD4;
	PORTDbits.RD5 = ~PORTDbits.RD5;
	PORTDbits.RD6 = ~PORTDbits.RD6;
	PORTDbits.RD7 = ~PORTDbits.RD7;
	conta = 0;						//Zera a variável conta
   }
INTCONbits.TMR0IF = 0;	//Zera o flag de controle da interrupção
TMR0H = 0x67;            //Valor inicial para a parte alta do TIMER 0
TMR0L = 0x6A;            //Valor Inicial para a parte baixa do TIMER 0
}

void main()
{
TRISD = 0x00;			//Faz toda a porta D como saída
PORTD = 0x0F;			//Zera toda a porta D, apagando os LEDS

T0CON = 0b10000111;      /*Configura o Registrador T0CON
                                       TMR0ON = 1 -> Habilita o TIMER 0
                                       T08BIT = 0 -> TIMER 0 no modo 16 bits
                                       T0CS = 0 -> Incremento pelo ciclo de máquina
                                        ...0,5us para o cristal de 8MHz.
                                       T0SE = 0 -> Incremento na orda de subida.
                                       PSA = 0 -> Prescale aplicado ao Timer 0
                                       PS2, PS1 e PS0 = 1 -> Prescale = 1:256*/

INTCON = 0b10100000;     /*Configura o registrador INTCON
                                     GIE = 1 (bit7) -> Habilita a interrupção global
                                     TMR0IE = 1 (bit 5)-> Habilita a interrupção do Timer 0
                                     TMR0IF = 0 (bit 2)-> Flag de interrupção do Timer 0 desligado */

TMR0H = 0x67;            //Valor inicial para a parte alta do TIMER 0
TMR0L = 0x6A;            //Valor Inicial para a parte baixa do TIMER 0

while(1)
	{
	}
}