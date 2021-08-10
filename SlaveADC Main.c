//******************************************************************************
/* 
 * File:   SlaveADC Main.c
 * Author: Daniel
 *
 * Created on August 09, 2021, 11:34 AM
 */
//******************************************************************************
//*****************************************************************************
// Palabra de configuración
//*****************************************************************************
// CONFIG1
#pragma config FOSC = XT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//*****************************************************************************
// Definición e importación de librerías
//*****************************************************************************
#include <stdint.h>
#include <pic16f887.h>
#include "I2C.h"
#include <xc.h>
#include "ADC header.h"
//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
uint8_t z;
uint8_t dato;


char Low_ADC1;
char Low_ADC2;
char High_ADC1;
char High_ADC2;
char temp;
char temp1;
char DisplayLow_ADC;
char DisplayHigh_ADC;
char contador_ADC;
char estado_ADC;
//*****************************************************************************
// Definición de funciones para que se puedan colocar después del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);
//*****************************************************************************
// Código de Interrupción 
//*****************************************************************************
void __interrupt() isr(void){
//****************************************************************
//      INTERRUPCION ADC
//****************************************************************    
    InterruptADC(&DisplayLow_ADC, &DisplayHigh_ADC);        //Funcion para saber que fue lo que se convirtio del ADC
    
    if (contador_ADC == 1) {                                //Escogemos puerto 
        High_ADC1 = DisplayHigh_ADC;
        Low_ADC1 = DisplayLow_ADC;
    }
    
    if (contador_ADC == 0) {
        High_ADC2 = DisplayHigh_ADC;
        Low_ADC2 = DisplayLow_ADC;
    }

//****************************************************************    
   if(PIR1bits.SSPIF == 1){ 

        SSPCONbits.CKP = 0;
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            z = SSPBUF;                 // Read the previous value to clear the buffer
            SSPCONbits.SSPOV = 0;       // Clear the overflow flag
            SSPCONbits.WCOL = 0;        // Clear the collision bit
            SSPCONbits.CKP = 1;         // Enables SCL (Clock)
        }

        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) {
            //__delay_us(7);
            z = SSPBUF;                 // Lectura del SSBUF para limpiar el buffer y la bandera BF
            //__delay_us(2);
            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupción recepción/transmisión SSP
            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);     // Esperar a que la recepción se complete
            PORTD = SSPBUF;             // Guardar en el PORTD el valor del buffer de recepción
            __delay_us(250);
            
        }else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
            z = SSPBUF;
            BF = 0;
            SSPBUF = (High_ADC1 << 4) + Low_ADC1; //Envio de datos al master
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
    }
}
//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    //*************************************************************************
    // Loop infinito
    //*************************************************************************
    while(1){

//******************************************************************************
//  CONVERSION ADC       
//******************************************************************************
        contador_ADC = 1;   //Indicamos en que puerto mostraremos nuestra conversion
        Read_ADC(2);        //Hacemos una conversion usando canal 13
        
        __delay_ms(5);
        
        contador_ADC = 0;   
        Read_ADC(3);       //Hacemos una conversion usando canal 11
        
        PORTB = ~PORTB;
       __delay_ms(500);
    }
    return;
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){
    TRISB = 0x00;
    TRISD = 0x00;
       
    PORTA = 0;
    PORTB = 0;     
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    
    ANSEL = 0;
    ANSELH = 0;
    ANS2 = 1; //Indicamos que pines son analogicos y cuales digitales del puerto A
    ANS3 = 1;

    ADCON0bits.ADCS = 2;
    ADCON0bits.GO = 0;
    ADCON0bits.ADON = 1;
    ADCON1 = 0x80;
    ADIF = 0;   //Habilita interrupcion del adc
    ADIE = 1;
    
    INTCONbits.GIE = 1;         // Habilitamos interrupciones
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones perifericas
    
    
    PIR1bits.SSPIF = 0;         // Borramos bandera interrupción MSSP
    PIE1bits.SSPIE = 1;         // Habilitamos interrupción MSSP
    TRISAbits.TRISA5 = 1;       // Slave Select
    
    IRCF0 = 1; //Oscilador a 8MHz
    IRCF1 = 1;
    IRCF2 = 1;
    SCS = 1;
    I2C_Slave_Init(0x50);   
}