//******************************************************************************
/* 
 * File:   Master Main.c
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
#include "LCD header.h"
#include <xc.h>
#include <stdio.h>
//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000

int envio = 0xFF;
char StringConversion[8];
int ValorContador;
char ValorADC;
int temp;

int contadorNum = 0;
int contadorDec = 0;
int contadorDec2 = 0;

int Temperatura;
//*****************************************************************************
// Definición de funciones para que se puedan colocar después del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);

//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
//***********************Interfaz grafica para la LCD**************************      
    Lcd_Init();
  
        Lcd_Clear(); 
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("S1");
        Lcd_Set_Cursor(1,8);
        Lcd_Write_String("S2");
        Lcd_Set_Cursor(1,13);
        Lcd_Write_String("S3");
        Lcd_Set_Cursor(2,5);
        Lcd_Write_String("V");
        Lcd_Set_Cursor(2,16);
        Lcd_Write_String("C");
//*************************Inicialización Sensor I2C****************************
   I2C_Master_Start();
   I2C_Master_Write(0x90|(0x03<<1)); //Genera primer byte (1001A2A1A0-W)
   I2C_Master_Write(0xee);        //Lee temperatura
   I2C_Master_Stop();
   __delay_ms(100);
        
        
    while(1){
//**********************Escritura y Lectura Sensor I2C**************************         
        I2C_Master_Start();
        I2C_Master_Write(0x90|(0x03<<1)); //Genera primer byte (1001A2A1A0-W)
        I2C_Master_Write(0xee);        //Lee temperatura
        I2C_Master_Stop();
        __delay_ms(100);

        I2C_Master_Start();
        I2C_Master_Write(0x90|(0x03<<1)); //Genera primer byte (1001A2A1A0-W)
        I2C_Master_Write(0xaa);        //Lee temperatura
        I2C_Master_Stop();
        __delay_ms(100);
        I2C_Master_Start();
        I2C_Master_Write(0x91|(0x03<<1)); //Genera primer byte (1001A2A1A0-R)
        Temperatura = I2C_Master_Read(0); 
        I2C_Master_Stop();
         __delay_ms(100);
   
        PORTB = Temperatura;
        
//************************Escritura y Lectura Slave 1***************************
        I2C_Master_Start();
        I2C_Master_Write(0x50);
        I2C_Master_Write(envio);
        I2C_Master_Stop();
        __delay_ms(100);
       
        I2C_Master_Start();
        I2C_Master_Write(0x51);
        ValorADC = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(100);
//*************************Escritura y Lectura Slave 2**************************       
        I2C_Master_Start();
        I2C_Master_Write(0x60);
        I2C_Master_Write(envio);
        I2C_Master_Stop();
        __delay_ms(200);
        
        I2C_Master_Start();
        I2C_Master_Write(0x61);
        ValorContador = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(200); 
        
//******************************************************************************
//  CONTADOR LCD    
//******************************************************************************
        Lcd_Set_Cursor(2,8);
        sprintf(StringConversion, "%d", ValorContador);    //convierte contador a string
        Lcd_Write_String(StringConversion);               //Imprime contador o sensor 2
        
        if (ValorContador >= 0 ){                //Condiciones de seguridad para borrar ceros indeseados al incrementar y decrementar contador
            if (ValorContador< 10){
                Lcd_Set_Cursor(2,9);
                Lcd_Write_String(" ");
                Lcd_Set_Cursor(2,10);
                Lcd_Write_String(" ");
            }
        }
        if (ValorContador >= 10 ){
            if (ValorContador < 100){
                Lcd_Set_Cursor(2,10);
                Lcd_Write_String(" ");
            }
        }
        if (ValorContador <= 0 ){
            if (ValorContador > -10){
                Lcd_Set_Cursor(2,10);
                Lcd_Write_String(" ");
            }
        }
        if (ValorContador <= -10 ){
            if (ValorContador > -100){
                Lcd_Set_Cursor(2,11);
                Lcd_Write_String(" ");
            }
        }
        __delay_ms(5); 
//******************************************************************************
//  Sensor Temperatura LCD     
//******************************************************************************
        Lcd_Set_Cursor(2,13);
        sprintf(StringConversion, "%d", Temperatura);    //convierte contador a string
        Lcd_Write_String(StringConversion);               //Imprime contador o sensor 2 

        if (Temperatura >= 0 ){                //Condiciones de seguridad para borrar ceros indeseados al incrementar y decrementar contador
            if (Temperatura< 10){
                Lcd_Set_Cursor(2,14);
                Lcd_Write_String(" ");
                Lcd_Set_Cursor(2,15);
                Lcd_Write_String(" ");
            }
        }
        if (Temperatura >= 10 ){
            if (Temperatura < 100){
                Lcd_Set_Cursor(2,15);
                Lcd_Write_String(" ");
            }
        }
        if (Temperatura <= 0 ){
            if (Temperatura > -10){
                Lcd_Set_Cursor(2,15);
                Lcd_Write_String(" ");
            }
        }
        if (Temperatura <= -10 ){
            if (Temperatura > -100){
                Lcd_Set_Cursor(2,16);
                Lcd_Write_String(" ");
            }
        }
        __delay_ms(5);         
//******************************************************************************
//  ADC LCD      
//******************************************************************************     
        Lcd_Set_Cursor(2,1);                    
        temp = ValorADC;     //Conversion completa ADC
        
        while (temp > 0){                       //Ciclos de divisiones para obtener un valor de 5V
            temp = temp - 51;
            contadorNum = contadorNum + 1; 
            if (temp <= 0){
                break;
            }
        }
        if (temp == 0){
            sprintf(StringConversion, "%d", contadorNum);
            Lcd_Write_String(StringConversion);
            contadorNum = 0;
            __delay_ms(5); //Imprimir en LCD la parte entera de la division:
            Lcd_Set_Cursor(2,2);
            Lcd_Write_Char('.'); 
            __delay_ms(5);  //Imprimir punto decimal
            Lcd_Set_Cursor(2,3);
            Lcd_Write_Char('0'); 
            __delay_ms(5);  //Imprimir punto decimal
            Lcd_Set_Cursor(2,4);
            Lcd_Write_Char('0'); 
            __delay_ms(5);  //Imprimir punto decimal
        }
        if (temp < 0){
            temp = temp + 51;
            contadorNum = contadorNum - 1;
            sprintf(StringConversion, "%d", contadorNum);
            Lcd_Write_String(StringConversion); 
            contadorNum = 0;
            __delay_ms(5); //Imprimir en LCD la parte entera de la division:
            Lcd_Set_Cursor(2,2);
            Lcd_Write_Char('.'); 
            __delay_ms(5);  //Imprimir punto decimal
            temp = temp * 10;
//*******************************************************************************            
            while (temp > 0){
            temp = temp - 51;
            contadorDec = contadorDec + 1;
                if (temp <= 0){
                    break;
                }
            }
            if (temp == 0){ 
            Lcd_Set_Cursor(2,3);
            sprintf(StringConversion, "%d", contadorDec);
            Lcd_Write_String(StringConversion); 
            contadorDec = 0;
            __delay_ms(5);  //Imprimir en LCD unico decimal de la division:
            Lcd_Set_Cursor(2,4);
            Lcd_Write_Char('0'); 
            __delay_ms(5);  //Imprimir punto decimal
            }
            if (temp < 0){
            temp = temp + 51;
            contadorDec = contadorDec - 1;
            Lcd_Set_Cursor(2,3);
            sprintf(StringConversion, "%d", contadorDec);
            Lcd_Write_String(StringConversion); 
            contadorDec = 0;
            __delay_ms(5);  //Imprimir en LCD el primer decimal de la division:
            temp = temp * 10;
//*******************************************************************************              
                while (temp > 0){
                temp = temp - 51;
                contadorDec2 = contadorDec2 + 1;
                    if (temp <= 0){
                        break;
                    }
                }
                if (temp == 0){ 
                Lcd_Set_Cursor(2,4);
                sprintf(StringConversion, "%d", contadorDec2);
                Lcd_Write_String(StringConversion); 
                contadorDec2 = 0;
                __delay_ms(5); //Imprimir en LCD unicos dos decimales de la division: 
                }
                if (temp < 0){
                temp = temp + 51;
                contadorDec2 = contadorDec2 - 1;
                Lcd_Set_Cursor(2,4);
                sprintf(StringConversion, "%d", contadorDec2);
                Lcd_Write_String(StringConversion); 
                contadorDec2 = 0;
                 __delay_ms(5);  //Imprimir en LCD el segundo decimal de la division:
                }
            }
        }       
//******************************************************************************
    }
    return;
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    TRISB = 0;
    TRISD = 0;
    TRISE = 0;
    
    PORTB = 0;
    PORTD = 0;
    PORTE = 0;
    
    I2C_Master_Init(100000);        // Inicializar Comuncación I2C
    
    IRCF0 = 1; //Oscilador a 8MHz
    IRCF1 = 1;
    IRCF2 = 1;
    SCS = 1;
}