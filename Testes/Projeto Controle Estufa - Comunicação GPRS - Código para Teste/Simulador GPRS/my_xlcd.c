/******************************************************************************
 *
 *            Curso PICMINAS de Microcontroladores (PIC18F4550)
 *
 ******************************************************************************
 * Nome do Arquivo:     my_xlcd.c
 * Dependencias:    	Veja arquivo "my_xlcd.h"
 * Processador:       	PIC18F4550
 * Op��o de Clock:		HS 20MHz (externo) - CPU: 48MHz	
 * Compilador:        	C18 v3.20 ou superior
 * Empresa:         	PICMINAS - Axoon - UFMG
 *
 * Plataforma:			Kit PICMINAS v3.0
 *
 * Autor:				M.Sc. Henrique Resende Martins
 * 						Mauricio Ferrari Santos Corr�a
 *						Bruno Silveira Avelar
 *
 * Data:				v1 - 15/Fev/2009 - M.Sc. Henrique Resende Martins
 *						v2 - 28/Abr/2009 - Mauricio Ferrari Santos Corr�a
 *						v3 - 16/Jun/2009 - Mauricio Ferrari Santos Corr�a
 *						v4 - 18/Nov/2009 - Mauricio Ferrari Santos Corr�a
 *						v5 - 05/Dez/2009 - Mauricio Ferrari Santos Corr�a
 *						v6 - 28/fev/2010 - Bruno Silveira Avelar
 *						v7 - 10/Mar/2010 - Mauricio Ferrari Santos Corr�a
 *						v8 - 15/Out/2010 - Mauricio Ferrari Santos Corr�a
 *
 * Descri��o:			Modifica��o da biblioteca xlcd.h forncedida com o compilador
 *						C18 da Microchip. A biblioteca xlcd.h trata da comunica��o
 *						com display de LCD por meio de 4-bits (length), utilizando o 
 *						PORTB tanto para os 4 bits de dados quanto para os 3 bits de
 *						controle (RS, RW e E).
 *						No projeto do Kit PICMINAS o display LCD � ligado a portas
 *						digitais diversas do PIC 18F4550. O Kit PICMINAS v3.0 utiliza
 *						os seguintes pinos para comunica��o com o display LCD:
 *							PIC18F4550(pino):: Display LCD
 *								RD4 (27)  	::	  D4
 *								RD5 (28)  	::	  D5
 *								RD6 (29)  	::	  D6
 *								RD7 (30)  	::	  D7
 *								RD2 (21)  	:: 	  RS
 *								RD3 (22)  	::	  E
 *								 GND		::	  R/W
 *						OBS: o pino R/W do display � ligado no  Kit PICMINAS ao GND
 *						atrav�s de um resistor de pull-down, assimo LCD opera no 
 *						modo escrite(Write) o tempo todo. No entando o c�digo C foi
 *						escrito mostrando a imlementa��o onde um pino do PIC � 
 *						utilizado para a fun��o R/W, assim comentando o inicio das 
 *						linhas correspondentes com "//n�o usado //".
 *   Notas:
 *		- Essa bliblioteca de rotinas foi escrita pensando em um LCD
 *        16x2 FDCC1602E implementado com o controlador SPLC780D, mas pode
 * 		  ser utilizado com qualquer LCD 16x2 com controladores equivalentes
 *        como, por exemplo, o controlador Hitachi HD44780 LCD controller.
 *
 *		- O usu�rio deve definir os seguintes itens:
 *      	-- O tipo de interface de comunica��o do LDC com o PIC (4 ou 8-bits):
 *             Se for o modo 4-bit, utilizar o upper nibble (nibble � metade de um Byte)
 *			   , ou seja, enviar primeiro os 4 bits mais significativos (upper nibble)
 *			   e depois os 4 bits menos sgnificativos (lower nibble).
 *
 *          -- Cursor e/ou Blink:
 *             O usu�rio poder� definir se quer visualizar cursor, blink 
 *			   ou nenhum dos dois.
 *          
 *			-- O sincronismo da comunica��o com o LCD:
 *             As temporiza��es entre o envio de comandos, n�o foi utilizado
 *			   o bit de Busy do LCD, mas sim, atrav�s de fun��es de atraso.
 *			   A fun��o utilizada foi a Delay10KTCYx() da bilioteca padr�o C18
 *			   delays.h que aparece na se��o INCLUDE deste arquivo.
 * 			   Os tempos de delay escolhidos est�o colocados nos defines:
 *			   DELAY_SINC e DELAY_INICIO, que foram ecolhidos bem acima do 
 *			   tempo m�nimo requerido pelo display de LCD (5ms o de sincronismo e
 *			   15ms o inicial).
 *****************************************************************************/


/** I N C L U D E S **********************************************************/
#include "my_xlcd.h"


/** F U N C O E S ************************************************************/

/******************************************************************************
 * Funcao:		void OpenXLCD(void)
 * Entrada:		Nenhuma (void)
 * Sa�da:		Nenhuma (void)
 * Descri��o: 	Configura portas do PIC 18F4550 que est�o conectadas ao 
 *				display LCD como saida. Essa rotina � baseada no controlador 
 *				de LCD Hitachi HD44780. Configura os parametros de comunica��o
 *				com o display, tais como:
 *					- modo de opera��o (4 ou 8 data bits)
 *					- tipo de cursor
 *					- tamanho da fonte
 *				Ao final da configura��o limpa o display.
 *
 *****************************************************************************/
void OpenXLCD(void)
{
	// Desabilita todas as 13 portas anal�gicas ANx (portas do conversor A/D),
	// assim as mesmas fucionam como pinos digitais
	ADCON1 |= 0x0F;

	// Configura os pinos de controle (RW,RS,E) como sa�da.
	//n�o usado // 	TRIS_RW = 0;
	TRIS_RS = 0;
	TRIS_E = 0;

	// Configura os pinos de dados (D4,D5,D6,D7) como sa�da.
	TRIS_D4=0;
	TRIS_D5=0;
	TRIS_D6=0;
	TRIS_D7=0;

// Inicio da rotina de inicializa��o do display LCD
// Aguardar tempos e enviar 3 vezes o comando 0b0011XXXX como descrito no datasheet
// do controlador HD44780 ou SPLC780D

	//Seleciona pinos de Controle para escrita do comando
	//n�o usado // 	RW_PIN = 0;	//escrita=0, leitura=1
	RS_PIN = 0;		// comando=0, dados=1
	E_PIN =  0; 	// desabilita=0, habilita=1

	//selecioan pinos de dados com comando de inicializa��o 0b0011 no Nibble-High
	//(os 4 bits de dados mais significativos), os bits do Nibble-Low s�o Don't Care 
	D7=0;
	D6=0;
	D5=1;
	D4=1;

//Passo 1: Envio do comando de inicializa��o 0b0011

	// Delay inicial de 15ms ap�s alimentar o LCD: Vdd > 4,5V
	Delay10KTCYx(DELAY_INICIO);

	//Habilita leitura do comando pelo LCD, atrav�s do pino de enable, para tanto,
	//um pulso de 5ms com borda de descida � efetuado
	E_PIN = 1;      			
    Delay10KTCYx(DELAY_SINC);
	E_PIN = 0;
	Delay10KTCYx(DELAY_SINC);

//Passo 2: Envio do comando de inicializa��o 0b0011

	// Delay de pelo menos 4.1ms
	Delay10KTCYx(DELAY_SINC);

	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 1;      			
    Delay10KTCYx(DELAY_SINC);
	E_PIN = 0;
	Delay10KTCYx(DELAY_SINC);

//Passo 3: Envio do comando de inicializa��o 0b0011

	// Delay de pelo menos 100us
	Delay10KTCYx(DELAY_SINC);

	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 1;      			
    Delay10KTCYx(DELAY_SINC);
	E_PIN = 0;
	Delay10KTCYx(DELAY_SINC);

//Passo 4: Envio da Configura��o 0b0010 no Nibble-High
	D7=0;
	D6=0;
	D5=1;
	D4=0;


	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 1;      			
    Delay10KTCYx(DELAY_SINC);
	E_PIN = 0;
	Delay10KTCYx(DELAY_SINC);


//Envio dos comandos de Configura��o do LCD
	WriteCmdXLCD(0b00101000);	// Function Set: 4bit interface, 2 lines, Font 5x7  
	WriteCmdXLCD(0b00000110);	// Entry mode set: Increment, Shift OFF
	WriteCmdXLCD(0b00001110);	// Display Control: Display ON, Cursor ON, Blink OFF
	WriteCmdXLCD(0b00000001);	// Clear display
}

/******************************************************************************
 * Funcao:		void WriteCmdXLCD(unsigned char cmd)
 * Entrada:		cmd: comando que ser� enviado para o display LCD.
 * Sa�da:		Nenhuma (void)
 * Descri��o: 	(Write a command to the LCD)
 *				Escreve um comando no display de LCD.
 *****************************************************************************/
void WriteCmdXLCD(unsigned char cmd)
{
	//Seleciona pinos de Controle para escrita do comando
	//n�o usado // 	RW_PIN = 0;	//escrita=0, leitura=1
	RS_PIN = 0;		// comando=0, dados=1
	E_PIN =  0; 	// desabilita=0, habilita=1

	//Seleciona pinos de Dados: Nibble-High
	if(cmd&0b00010000){D4=1;}
	else {D4=0;}
	if(cmd&0b00100000){D5=1;}
	else {D5=0;}
	if(cmd&0b01000000){D6=1;}
	else {D6=0;}
	if(cmd&0b10000000){D7=1;}
	else {D7=0;}

	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 1;
    Delay10KTCYx(DELAY_SINC);
	E_PIN = 0;
	Delay10KTCYx(DELAY_SINC);

	//Seleciona pinos de Dados: Nibble-Low
	if(cmd&0b00000001){D4=1;}
	else {D4=0;}
	if(cmd&0b00000010){D5=1;}
	else {D5=0;}
	if(cmd&0b00000100){D6=1;}
	else {D6=0;}
	if(cmd&0b00001000){D7=1;}
	else {D7=0;}    

	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 1;
    Delay10KTCYx(DELAY_SINC);
	E_PIN = 0;
	Delay10KTCYx(DELAY_SINC);
}
/******************************************************************************
 * Funcao:		void WriteDataXLCD(unsigned char data)
 * Entrada:		data: Byte de dados para ser escrito na memoria RAM do
 *				controlador do display LCD.
 * Sa�da:		Nenhuma (void)
 * Descri��o: 	(Write a Data byte to the LCD)
 *				Essa rotina escreve um byte de dados na posi��o da memoria RAM setada
 * 				anteriormente com a fun��o SetDDRamAddr().
 *****************************************************************************/
void WriteDataXLCD(char data)
{
	//Seleciona pinos de Controle para escrita do comando
	//n�o usado // 	RW_PIN = 0;	//escrita=0, leitura=1
	RS_PIN = 1;		// comando=0, dados=1
	E_PIN =  0; 	// desabilita=0, habilita=1

	//Seleciona pinos de Dados: Nibble-High
	if(data&0b00010000){D4=1;}
	else {D4=0;}
	if(data&0b00100000){D5=1;}
	else {D5=0;}
	if(data&0b01000000){D6=1;}
	else {D6=0;}
	if(data&0b10000000){D7=1;}
	else {D7=0;}

	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 1;
    Delay10KTCYx(DELAY_SINC);
	E_PIN = 0;
	Delay10KTCYx(DELAY_SINC);


	//Seleciona pinos de Dados: Nibble-Low
	if(data&0b00000001){D4=1;}
	else {D4=0;}
	if(data&0b00000010){D5=1;}
	else {D5=0;}
	if(data&0b00000100){D6=1;}
	else {D6=0;}
	if(data&0b00001000){D7=1;}
	else {D7=0;}    

	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 1;
    Delay10KTCYx(DELAY_SINC);
	E_PIN = 0;
	Delay10KTCYx(DELAY_SINC);
}

/******************************************************************************
 * Funcao:		void SetDDRamAddr(unsigned char DDaddr)
 * Entrada:		DDaddr: endere�o da memoria de dados do display que se deseja
 *				posicionar o cursor.
 * Sa�da:		Nenhuma (void)
 * Descri��o: 	(Set the Display Data RAM Address)
 *				Essa rotina posiciona o cursor no endere�o da memoria RAM do
 *				controlador do display LCD passada como parametro.
 *				Assim, a pr�xima escrita no display ser� iniciada a partir desta
 *				posi��o definida para o cursor.
 *****************************************************************************/
void SetDDRamAddr(unsigned char DDaddr)
{
	//Seleciona pinos de Controle para escrita do comando
	//n�o usado // 	RW_PIN = 0;	//escrita=0, leitura=1
	RS_PIN = 0;		// comando=0, dados=1
	E_PIN =  0; 	// desabilita=0, habilita=1

	//Seleciona pinos de Dados: Nibble-High
	D7 = 1;	
	if(DDaddr&0b00010000){D4=1;}
	else {D4=0;}
	if(DDaddr&0b00100000){D5=1;}
	else {D5=0;}
	if(DDaddr&0b01000000){D6=1;}
	else {D6=0;}
	
	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 1;
    Delay10KTCYx(DELAY_SINC);
	E_PIN = 0;
	Delay10KTCYx(DELAY_SINC);

	//Seleciona pinos de Dados: Nibble-Low
	if(DDaddr&0b00000001){D4=1;}
	else {D4=0;}
	if(DDaddr&0b00000010){D5=1;}
	else {D5=0;}
	if(DDaddr&0b00000100){D6=1;}
	else {D6=0;}
	if(DDaddr&0b00001000){D7=1;}
	else {D7=0;}

	//Pulso no pino de Enable para habilitar leitura do LCD na borda de descida
	E_PIN = 1;
    Delay10KTCYx(DELAY_SINC);
	E_PIN = 0;
	Delay10KTCYx(DELAY_SINC);
}

/******************************************************************************
 * Funcao:		void putsXLCD(char *buffer)
 * Entrada:		*buffer: apontador para string na memoria de dados(RAM) a ser
 *				escrita no display.
 * Sa�da:		Nenhuma (void)
 * Descri��o: 	Essa rotina copia uma string terminada com caracter nulo
 *				da memoria de dados do microcontrolador para o display de LCD.
 *				A string apontada por *buffer � copiada no display a partir da
 *				posi��o da memoria RAM do display escolhida com a fun��o
 *				SetDDRamAddr().
 *****************************************************************************/
void putsXLCD(char *buffer)
{
	while(*buffer)                  // escreve dados no LCD ate achar caracter nulo
	{
	        WriteDataXLCD(*buffer); // escreve caracter no LCD
	        buffer++;               // incrementa apontador
	}
}

/******************************************************************************
 * Funcao:		void putrsXLCD(const rom char *buffer)
 * Entrada:		*buffer: apontador para string na memoria de programa(ROM) a ser
 *				escrita no display.
 * Sa�da:		Nenhuma (void)
 * Descri��o: 	Essa rotina copia uma string terminada com caracter nulo
 *				da memoria de programa do microcontrolador para o display de LCD.
 *				A string apontada por *buffer � copiada no display a partir da
 *				posi��o da memoria RAM do display escolhida com a fun��o
 *				SetDDRamAddr().
 *****************************************************************************/
void putrsXLCD(const rom char *buffer)
{
	while(*buffer)                  // escreve dados no LCD ate achar caracter nulo
	{
	        WriteDataXLCD(*buffer); // escreve caracter no LCD
	        buffer++;               // incrementa apontador
	}
}

/******************************************************************************
 * Funcao:		void putIntXLCD(int valor)
 * Entrada:		valor: inteiro com sinal (de -32768 a 32767) a ser escrito no
 *				display.
 * Sa�da:		Nenhuma (void)
 * Descri��o: 	Essa rotina escreve o numero inteiro passado como parametro
 *				no display de LCD a partir da posi��o atual do cursor.
 *****************************************************************************/
void putIntXLCD(int valor)
{
	char texto[7] = " ";
	itoa( valor, texto);
	putsXLCD(texto);
}