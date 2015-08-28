/******************************************************************************
 *
 *            Curso PICMINAS de Microcontroladores (PIC18F4550)
 *
 ******************************************************************************
 * Nome do Arquivo:     my_xlcd.h
 * Dependencias:    	Veja se��o INCLUDES
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
#ifndef __MY_XLCD_H
#define __MY_XLCD_H

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>	// Biblioteca com as defini��es do PIC selecionado 
						//no projeto, neste caso, o modelo 18F4550.

#include <delays.h>		// Biblioteca padr�o C18 Microchip para uso de rotinas de atraso (delay)
						// mais informa��es nos arquivos de ajuda da biblioteca C18
						// presentes na propria pasta de instala��o, por exemplo,
						// C:\MCC18\doc\hlpC18Lib.chm
						// Procure pelo item: "Delay Functions"

#include <stdlib.h>		// Biblioteca padr�o C18 Microchip para uso de rotinas de convers�o
						// de tipos como itoa(). Mais informa��es nos arquivos de ajuda da
						// biblioteca C18 presentes na propria pasta de instala��o, por exemplo,
						// C:\MCC18\doc\hlpC18Lib.chm
						// Procure pelo item: "Data Conversion Functions"

/** D E F I N E S ************************************************************/
// Define dos pinos de DADOS do LCD para o KIT PICMINAS v3.0
#define D4       PORTDbits.RD4
#define D5       PORTDbits.RD5
#define D6		 PORTDbits.RD6
#define D7       PORTDbits.RD7

// Define do TRIS dos pinos de DADOS do LCD para o KIT PICMINAS v3.0
#define TRIS_D4  TRISDbits.TRISD4
#define TRIS_D5  TRISDbits.TRISD5
#define TRIS_D6  TRISDbits.TRISD6
#define TRIS_D7  TRISDbits.TRISD7

// Define dos pinos de CONTROLE do LCD para o KIT PICMINAS v3.0
#define RS_PIN   PORTDbits.RD2
#define E_PIN    PORTDbits.RD3
//n�o usado // #define RW_PIN   PORTBbits.RB2

// Define os TRIS dos pinos de CONTROLE do LCD para o KIT PICMINAS v3.0
#define TRIS_RS  TRISDbits.TRISD2
#define TRIS_E   TRISDbits.TRISD3
//n�o usado // #define TRIS_RW  TRISBbits.TRISB2

// Define o valor dos delays baseado no clock da CPU de 48 MHz
#define DELAY_SINC		6	//(5ms)
#define DELAY_INICIO	18	//(15ms)

/**  P R O T O T I P O  D E  F U N C O E S  **********************************/
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
 *****************************************************************************/
 extern void OpenXLCD(void);

/******************************************************************************
 * Funcao:		void WriteCmdXLCD(unsigned char cmd)
 * Entrada:		cmd: comando que ser� enviado para o LCD
 * Sa�da:		Nenhuma (void)
 * Descri��o: 	(Write a command to the LCD)
 *				Escreve um comando no display de LCD.
 *****************************************************************************/
 extern void WriteCmdXLCD(unsigned char cmd);

/******************************************************************************
 * Funcao:		void WriteDataXLCD(unsigned char data)
 * Entrada:		data: Byte de dados para ser escrito na memoria RAM do LCD
 * Sa�da:		Nenhuma (void)
 * Descri��o: 	(Write a Data byte to the LCD)
 *				Essa rotina escreve um byte de dados na posi��o da memoria RAM setada
 * 				anteriormente com a fun��o SetDDRamAddr().
 *****************************************************************************/
 extern void WriteDataXLCD(char data);

/******************************************************************************
 * Funcao:		void SetDDRamAddr(unsigned char DDaddr)
 * Entrada:		DDaddr: endere�o da memoria de dados do display que se deseja apontar
 * Sa�da:		Nenhuma (void)
 * Descri��o: 	(Set the Display Data RAM Address)
 *				Essa rotina seta o endere�o da memoria RAM de dados do 
 *				display LCD para o endere�o	que se deseja escrever um caracter
 *				usando a fun��o WriteDataXLCD().
 *****************************************************************************/
 extern void SetDDRamAddr(unsigned char DDAddr);

/******************************************************************************
 * Funcao:		void putsXLCD(char *buffer)
 * Entrada:		*buffer: apontador para string na memoria de dados(RAM)
 * Sa�da:		Nenhuma (void)
 * Descri��o: 	Essa rotina copia uma string terminada com caracter nulo
 *				da memoria de dados do microcontrolador para o display de LCD.
 *				A string apontada por *buffer � copiada no display a partir da
 *				posi��o da memoria RAM do display escolhida com a fun��o
 *				SetDDRamAddr().
 *****************************************************************************/
 extern void putsXLCD(char *buffer);

/******************************************************************************
 * Funcao:		void putrsXLCD(const rom char *buffer)
 * Entrada:		*buffer: apontador para string na memoria de programa(ROM)
 * Sa�da:		Nenhuma (void)
 * Descri��o: 	Essa rotina copia uma string terminada com caracter nulo
 *				da memoria de programa do microcontrolador para o display de LCD.
 *				A string apontada por *buffer � copiada no display a partir da
 *				posi��o da memoria RAM do display escolhida com a fun��o
 *				SetDDRamAddr().
 *****************************************************************************/
 extern void putrsXLCD(const rom char *buffer);

/******************************************************************************
 * Funcao:		void putIntXLCD(int valor)
 * Entrada:		valor: inteiro com sinal (de -32768 a 32767) a ser escrito no
 *				display.
 * Sa�da:		Nenhuma (void)
 * Descri��o: 	Essa rotina escreve o numero inteiro passado como parametro
 *				no display de LCD a partir da posi��o atual do cursor.
 *****************************************************************************/
void putIntXLCD(int valor);

#endif
