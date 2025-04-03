/******************************************************************
* Area Ingenieria de Sistemas y Automatica
* Sistemas Electronicos Programables
*******************************************************************
* nombre fichero : main.c
*******************************************************************
* descripcion    : Practica PR6 medicion y gestion tiempo
* programador    : JL Villarroel (JLV)
* lenguaje       : ANSI C
* fecha          : 16/02/2022
********************************************************************/

/******************************************************************
*                   Modulos usados
*******************************************************************/

#include <stdlib.h>
#include <msp430.h>
#include "InitSystem.h"
#include "clock.h"
#include "display.h"


/******************************************************************
*                   Variables globales
*******************************************************************/

unsigned int siguiente, periodo ;
unsigned int t_reaccion ;
unsigned char comienzo = 0;
unsigned char pulsado = 0;
const unsigned int DELAY = 250;

Timer_id Ti ;

// unsigned char disp = 0 ;
unsigned char Centesimas, Decimas, Segundos ;


/******************************************************************
*                   Prototipos funciones
*******************************************************************/

void Init_GPIO (void) ;
void mostrar_t_reaccion (unsigned int) ;
void reset_game (void) ;

/******************************************************************
*                   Main y funciones
*******************************************************************/

int main(void)
{
    Stop_Watchdog () ;                  // Stop watchdog timer

    Init_CS () ;                        // MCLK = 8 MHz, SMCLK = 4 MHz
    Init_Clock () ;
    Init_GPIO () ;
    Init_Display () ;

    P1IE |= BIT1 | BIT2 ;
    P1IES &= ~BIT1 ;
    P1IES &= ~BIT2 ; 

    while(1)
    {
        if(comienzo){
            unsigned long dt = (rand() % 4000) + 1000;
            Ti = Set_Timer(dt, ONE_SHOT, 0);
            while(!Time_Out(Ti));
            Remove_Timer(Ti);
            P2OUT |= BIT0;
            unsigned int start = Get_Time();
            while(!pulsado);
            unsigned int end = Get_Time();
            t_reaccion = end - start;
            reset_game();
            mostrar_t_reaccion(t_reaccion);
        }
    }
}

void mostrar_t_reaccion(unsigned int t){
    t = t / 10; // Tiempo en centésimas (Asumimos que t nos lo pasan en milésimas)
    while(!comienzo){
        display(0, t % 10);
        __delay_cycles(DELAY);
        display(1, (t / 10) % 10);
        __delay_cycles(DELAY);
        display(2, (t / 100) % 10);
        __delay_cycles(DELAY);
    }
    display(0, 0);
}

void reset_game(){
    pulsado = 0;
    comienzo = 0;
    P2OUT &= ~BIT0;
}

void Init_GPIO (void) {
    // P2.6,P2.7: XIN, XOUT, reloj externo
    // P1.0 salida, led de la tarjeta
    // P6.6 salida, led de la tarjeta
    // P1.1 .. P1.6 entradas, pulsadores del ascensor
    // P3 control display 7 segmentos
    // P6.0 y P6.1 --> entradas encoder
    // P6.2 y P6.3 --> salidas PWM
    // resto: sin uso

    P1DIR |= BIT0 ;                                          // led de la tarjeta
    // P1.1 - P1.6 entradas para pulsadores
    P1REN |= BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 ;       // pull-down/pull-up
    P1OUT |= BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 ;       // pull-up
    P1DIR |= BIT7 ;                                          // sin uso
    // P2.0 - P2.2 salidas para led
    P2DIR |= BIT0 | BIT1 | BIT2  ;                           // salida led
    P2DIR |= BIT3 | BIT4 | BIT5 ;                            // sin uso
    P2OUT = 0x00 ;                                           // sin uso
    // P3 control display 7 segmentos
    P3DIR |= BIT0 | BIT1 | BIT2 ;                            // D0, D1, D2
    P3DIR |= BIT3 | BIT4 | BIT5 | BIT6 ;                     // A, B, C, D
    P3DIR |= BIT7 ;                                          // sin uso
    P3OUT = 0x00 ;
    // P4.5 - P4.7 salidas para led
    P4DIR |= BIT0 | BIT1 | BIT2 | BIT3 | BIT4 ;              // sin uso
    P4DIR |= BIT5 | BIT6 | BIT7 ;                            // salida led
    P4OUT = 0x00 ;
    // P5.2 - P5.3 ADC
    P5DIR |= BIT0 | BIT1 | BIT4 ;                            // sin uso
    P5OUT = 0x00 ;
    // P6.0 - P6.1 entradas encoder
    // P6.2 - P6.3 salidas PWM
    P6DIR |= BIT2 | BIT3 ;                                   // PWM
    P6DIR |= BIT4 | BIT5 ;                                   // sin uso
    P6DIR |= BIT6 ;                                          // led de la tarjeta
    P6OUT = 0x00 ;


    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings
}

#pragma vector = PORT1_VECTOR
__interrupt void Pulso (void) {
    if (P1IV) {
        if (P1IN & BIT2){
            comienzo = 1;
        }
        if (P1IN & BIT1){
            pulsado = 1;
        }
    }
    return ;
}

