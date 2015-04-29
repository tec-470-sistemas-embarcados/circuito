#include <p18f4550.h>
#include <stdio.h>
#include <adc.h>

#pragma CONFIG WDT=OFF

unsigned int temp_externa;
unsigned int lumi_esterna;

void main(){
  
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