/******************************************************************
* Area Ingenieria de Sistemas y 
* Automatica y Arquitectura y tecnologia de Computadores
* Sistemas Empotrados 1
*******************************************************************
* nombre fichero : main.c
*******************************************************************
* descripcion    : Practica PR1 comprobación placa
* programador    : JL Villarroel (JLV) & Enrique Torres
* lenguaje       : ANSI C
* fecha          : 8/03/2022
********************************************************************/

/******************************************************************
*                   Modulos usados
*******************************************************************/
#include <stdlib.h>
#include <msp430.h>
#include "InitSystem.h"
#include "display.h"

/******************************************************************
*                   Variables compartidas
*******************************************************************/

static volatile unsigned char pulso_flag = 0;
const unsigned long DELAY = 300000;
const unsigned int NUM_LOOPS = 5;

static unsigned char antes_pulsado_D1_sum = 0, antes_pulsado_D1_res = 0, 
                         antes_pulsado_D2_sum = 0, antes_pulsado_D2_res = 0, 
                         antes_pulsado_D3_sum = 0, antes_pulsado_D3_res = 0, 
                         value_D1 = 0, value_D2 = 0, value_D3 = 0;

/******************************************************************
*                   Prototipos funciones
*******************************************************************/
void Init_GPIO (void) ;
void Leds_start_centro (void) ;
void Leds_start_extremo (void) ;
void Pulsadores_check (void) ;
void Segmentos_check (void) ;

/******************************************************************
*                   Main y funciones
*******************************************************************/
int main(void)
{
    // static unsigned char antes_pulsado = 0, j = 0 ;
    

    Stop_Watchdog () ;                  // Stop watchdog timer

    Init_CS () ;                        // MCLK = 8 MHz, SMCLK = 4 MHz
    Init_GPIO () ;
    Init_Display () ;
    Init_Clock ();

//    P1IE |= BIT1 ;                    // P1.1 Int enabled
//    P1IES &= ~BIT1 ;                  // P1.1 Int raising edge

// PASO 3 ----------------------------------------------------------------

    unsigned int i = 0;
    while(i < NUM_LOOPS){
        Leds_start_centro();
        __delay_cycles(DELAY);
        Leds_start_extremo();
        i++;
    }

// -----------------------------------------------------------------------

    while(1)
    {
        // PASO 4 -------------------------------------------------
        Pulsadores_check();
        // --------------------------------------------------------

        // PASO 5 -------------------------------------------------
        Segmentos_check();
        // --------------------------------------------------------

        if (pulso_flag > 0){
            pulso_flag = 0;     // OjO, secion critica? 
            //...
        } 
    }
}

void Segmentos_check (void) {
    if ((P1IN & BIT1) && !antes_pulsado_D1_sum) {//detector de flancos
            antes_pulsado_D1_sum = 1 ;
            value_D1 = (value_D1 + 1)%10 ;
    }
    else if (!(P1IN & BIT1)) antes_pulsado_D1_sum = 0 ;

    if ((P1IN & BIT2) && !antes_pulsado_D1_res) {//detector de flancos
        antes_pulsado_D1_res = 1 ;
        if (value_D1 == 0) value_D1 = 9;
        else value_D1 = value_D1 - 1;
    }
    else if (!(P1IN & BIT2)) antes_pulsado_D1_res = 0 ;


    if ((P1IN & BIT3) && !antes_pulsado_D2_sum) {//detector de flancos
        antes_pulsado_D2_sum = 1 ;
        value_D2 = (value_D2 + 1)%10 ;
    }
    else if (!(P1IN & BIT3)) antes_pulsado_D2_sum = 0 ;

    if ((P1IN & BIT4) && !antes_pulsado_D2_res) {//detector de flancos
        antes_pulsado_D2_res = 1 ;
        if (value_D2 == 0) value_D2 = 9;
        else value_D2 = value_D2 - 1;
    }
    else if (!(P1IN & BIT4)) antes_pulsado_D2_res = 0 ;


    if ((P1IN & BIT5) && !antes_pulsado_D3_sum) {//detector de flancos
        antes_pulsado_D3_sum = 1 ;
        value_D3 = (value_D3 + 1)%10 ;
    }
    else if (!(P1IN & BIT5)) antes_pulsado_D3_sum = 0 ;

    if ((P1IN & BIT6) && !antes_pulsado_D3_res) {//detector de flancos
        antes_pulsado_D3_res = 1 ;
        if (value_D3 == 0) value_D3 = 9;
        else value_D3 = value_D3 - 1;
    }
    else if (!(P1IN & BIT6)) antes_pulsado_D3_res = 0 ;

    display (0, value_D3) ;
    display (1, value_D2) ;
    display (2, value_D1) ;
}

void Pulsadores_check (void) {
    if (P1IN & BIT1){
        P2OUT |= BIT0;
    }
    else P2OUT &= ~BIT0 ;
    
    if (P1IN & BIT2){
        P2OUT |= BIT1;
    }
    else P2OUT &= ~BIT1 ;

    if (P1IN & BIT3){
        P2OUT |= BIT2;
    }
    else P2OUT &= ~BIT2 ;

    if (P1IN & BIT4){
        P4OUT |= BIT4;
    }
    else P4OUT &= ~BIT4 ;

    if (P1IN & BIT5){
        P4OUT |= BIT5;
    }
    else P4OUT &= ~BIT5 ;

    if (P1IN & BIT6){
        P4OUT |= BIT6;
    }
    else P4OUT &= ~BIT6 ;
}

void Leds_start_centro (void) {
    P4OUT |= BIT4;
    __delay_cycles(DELAY);
    P4OUT &= ~BIT4;
    P2OUT |= BIT2;
    P4OUT |= BIT5;
    __delay_cycles(DELAY);
    P2OUT &= ~BIT2;
    P4OUT &= ~BIT5;
    P2OUT |= BIT1;
    P4OUT |= BIT6;
    __delay_cycles(DELAY);
    P2OUT &= ~BIT1;
    P4OUT &= ~BIT6;
    P2OUT |= BIT0;
    P4OUT |= BIT7;
    __delay_cycles(DELAY);
    P2OUT &= ~BIT0;
    P4OUT &= ~BIT7;
}

void Leds_start_extremo (void) {
    P2OUT |= BIT0;
    P4OUT |= BIT7;
    __delay_cycles(DELAY);
    P2OUT &= ~BIT0;
    P4OUT &= ~BIT7;
    P2OUT |= BIT1;
    P4OUT |= BIT6;
    __delay_cycles(DELAY);
    P2OUT &= ~BIT1;
    P4OUT &= ~BIT6;
    P2OUT |= BIT2;
    P4OUT |= BIT5;
    __delay_cycles(DELAY);
    P2OUT &= ~BIT2;
    P4OUT &= ~BIT5;
    P4OUT |= BIT4;
    __delay_cycles(DELAY);
    P4OUT &= ~BIT4;
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
__interrupt void RSI_Pulso (void) {
    pulso_flag++;
    if (P1IV) {
//...
      P2OUT |= BIT0;
    }
    return ;
}

