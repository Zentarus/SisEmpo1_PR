/******************************************************************************/
/*                                                                            */
/*                 Area Ingenieria de Sistemas y Automatica                   */
/*           Departamento de Informatica e Ingenieria de Sistemas             */
/*                         Universidad de Zaragoza                            */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* proyecto  : PRACTICAS SEP - SEI UNIZAR                                     */
/* fichero   : main.c                                                          */
/* version   : 1                                                              */
/* fecha     : 22/02/2022                                                     */
/* autor     : Jose Luis Villarroel                                           */
/* descripcion : Practica PR5-6 (lavadora) (MSP430)                             */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                        Modulos usados                                      */
/******************************************************************************/

#include <msp430.h>
#include "InitSystem.h"
#include "display.h"
#include "clock.h"
#include "pwm.h"
#include "encoder.h"
#include "servos.h"
#include "ad.h"


/******************************************************************************/
/*                        Variables globales                                  */
/******************************************************************************/

enum {INICIAL, PROG1, PROG15, PROG14, PROG13, PROG12, PROG11,
    PROG2, PROG24, PROG23, PROG22, PROG21,
   } estado ;

unsigned int siguiente, periodo = 10;
unsigned int marco_index = 0;
unsigned int programa = 0;
unsigned int paso = 0;
unsigned int iniciar = 0;
unsigned int pulsador1 = 0;
unsigned int pulsador5 = 0;
unsigned int pulsador6 = 0;

unsigned int periodo_motor = 100;
float velocidad_real;
unsigned int velocidad_periodica = 2048;

unsigned int pulso_automata = 0;
unsigned int pulso_motor = 0;
unsigned int pulso_visualizacion = 0;


// Variables globales de la practica

/******************************************************************************/
/*                        Prototipos funciones                                */
/******************************************************************************/

void Init_GPIO (void) ;
void Init_Pulso(void);
float convertir_a_rad_s(unsigned int);
void automata (void) ;
void pulsadores_control(void);
void motores_control(void);
void visualizacion_pulso_reg(void);
Timer_id T;


/******************************************************************************/
/*                        Main y funciones                                    */
/******************************************************************************/

int main(void)
{
    Stop_Watchdog () ;                  // Stop watchdog timer

    Init_CS () ;                        // MCLK = 8 MHz, SMCLK = 4 MHz
    Init_Clock () ;
    Init_GPIO () ;

    Init_PWM () ;
    Init_Encoder () ;
    Init_AD () ;

    Init_Pulso ();
    Init_Servos (periodo_motor) ;

    P1IE |= BIT1 | BIT5 | BIT6;
    P1IES &= ~BIT1 ;
    P1IES &= ~BIT5 ;
    P1IES &= ~BIT6 ;

    siguiente = Get_Time () ;


    while(1){

        visualizacion_pulso_reg();

        switch(marco_index){
        case 0:
            // Automata
            automata();

            // Visualizaci�n de segmentos para programa y paso de ejecuci�n
            display(2, programa);

            // Gesti�n de pulsadores (control discreto) y Resetear pulsadores
            pulsadores_control();

            // Gestión de los motores
            motores_control();

            marco_index++;

            break;
        case 1:
            // Visualizaci�n de segmentos para programa y paso de ejecuci�n
            display(0, paso);

            marco_index++;
            break;
        case 2:
            // Visualizaci�n de segmentos para programa y paso de ejecuci�n
            display(2, programa);

            marco_index++;
            break;
        case 3:
            // Visualizaci�n de segmentos para programa y paso de ejecuci�n
            display(0, paso);

            marco_index++;
            break;
        case 4:
            // Visualizaci�n de segmentos para programa y paso de ejecuci�n
            display(2, programa);

            marco_index++;
            break;
        case 5:
            automata();

            // Visualizaci�n de segmentos para programa y paso de ejecuci�n
            display(0, paso);

            // Gesti�n de pulsadores (control discreto) y Resetear pulsadores
            pulsadores_control();

            marco_index++;
            break;
        case 6:
            // Visualizaci�n de segmentos para programa y paso de ejecuci�n
            display(2, programa);
            marco_index++;
            break;
        case 7:
            // Visualizaci�n de segmentos para programa y paso de ejecuci�n
            display(0, paso);
            marco_index++;
            break;
        case 8:
            // Visualizaci�n de segmentos para programa y paso de ejecuci�n
            display(2, programa);
            marco_index++;
            break;
        case 9:
            // Visualizaci�n de segmentos para programa y paso de ejecuci�n
            display(0, paso);
            marco_index = 0;
            break;
        }
        siguiente += periodo;
        delay_until (siguiente);
    }

}

void motores_control(void){

    if (pulso_motor) {
        pulso_motor = 0;
        P4OUT &= ~BIT7;
    }
    else {
        pulso_motor = 1;
        P4OUT |= BIT7;
    }

    Set_Value_10b(velocidad_periodica >> 2);
}

void visualizacion_pulso_reg(void) {
    if (pulso_visualizacion) {
        pulso_visualizacion = 0;
        P4OUT &= ~BIT5;
    }
    else {
        pulso_visualizacion = 1;
        P4OUT |= BIT5;
    }
}

void pulsadores_control (void){
    // Gesti�n de pulsadores (control discreto) y Resetear pulsadores
            if (pulsador1){
                if (programa != 0){
                    iniciar = 1;
                    P2OUT |= BIT0;
                }

                pulsador1 = 0;
            }

            if (pulsador5){
                if (iniciar == 0){

                    // Encender led correspondiente
                    P2OUT |= BIT1;
                    P2OUT &= ~BIT2;

                    programa = 1;
                }
                pulsador5 = 0;
            }

            if (pulsador6){
                if (iniciar == 0){

                    // Encender led correspondiente
                    P2OUT |= BIT2;
                    P2OUT &= ~BIT1;

                    programa = 2;
                }
                pulsador6 = 0;
            }
}

float convertir_a_rad_s(unsigned int valor_entero) {
    return (valor_entero / 4096.0) * 8.0 - 4.0;
}


void automata (void) {

    if (pulso_automata) {
        pulso_automata = 0;
        P4OUT &= ~BIT6;
    }
    else {
        pulso_automata = 1;
        P4OUT |= BIT6;
    }

    switch (estado)
    {
    case INICIAL:
    paso = 0;
    velocidad_periodica = 2048; // 0 rad/s;
        if (programa == 1) {
            estado = PROG1;
        }
        else if (programa == 2) {
            estado = PROG2;
        }
        break;

    // ------------------------------------------------ PROGRAMA 1

    case PROG1:
        if (iniciar) {
            estado = PROG15;
            T = Set_Timer(4000, ONE_SHOT, 0); //Timer de 4 segundos
        }
        if (programa == 2) {
            estado = PROG2;
        }
        break;

    case PROG15:
        paso = 5;
        velocidad_periodica = 1536 + 1024; // 1 rad/s
        if (Time_Out(T)) {
            Remove_Timer(T);
            T = Set_Timer(4000, ONE_SHOT, 0); //Timer de 4 segundos
            estado = PROG14;
        }
        break;

    case PROG14:
        paso = 4;
        velocidad_periodica = 1536; // -1 rad/s
        if (Time_Out(T)) {
            Remove_Timer(T);
            T = Set_Timer(4000, ONE_SHOT, 0); //Timer de 4 segundos
            estado = PROG13;
        }
        break;

    case PROG13:
        paso = 3;
        velocidad_periodica = 1536 + 1024; // 1 rad/s
        if (Time_Out(T)) {
            Remove_Timer(T);
            T = Set_Timer(4000, ONE_SHOT, 0); //Timer de 4 segundos
            estado = PROG12;
        }
        break;

    case PROG12:
        paso = 2;
        velocidad_periodica = 2048; // 0 rad/s
        if (Time_Out(T)) {
            Remove_Timer(T);
            T = Set_Timer(10000, ONE_SHOT, 0); //Timer de 10 segundos
            estado = PROG11;
        }
        break;

    case PROG11:
        paso = 1;
        velocidad_periodica = 3584; // 3 rad/s
        if (Time_Out(T)) {
            programa = 0;
            iniciar = 0;
            P2OUT &= ~BIT1;
            P2OUT &= ~BIT0;
            estado = INICIAL;
        }
        break;


    // ------------------------------------------------ PROGRAMA 2

    case PROG2:
        if (iniciar) {
            estado = PROG24;
            T = Set_Timer(2000, ONE_SHOT, 0); //Timer de 2 segundos
        }
        if (programa == 1) {
            estado = PROG1;
        }
        break;
    case PROG24:
        velocidad_periodica = 1536 + 256; // -0.5 rad/s
        paso = 4;
        if (Time_Out(T)) {
            Remove_Timer(T);
            T = Set_Timer(4000, ONE_SHOT, 0); //Timer de 4 segundos
            estado = PROG23;
        }
        break;

    case PROG23:
        paso = 3;
        velocidad_periodica = 1536 + 1024; // 1 rad/s
        if (Time_Out(T)) {
            Remove_Timer(T);
            T = Set_Timer(2000, ONE_SHOT, 0); //Timer de 2 segundos
            estado = PROG22;
        }
        break;

    case PROG22:
        paso = 2;
        velocidad_periodica = 2048; // 2 rad/s
        if (Time_Out(T)) {
            Remove_Timer(T);
            T = Set_Timer(6000, ONE_SHOT, 0); //Timer de 6 segundos
            estado = PROG21;
        }
        break;

    case PROG21:
        paso = 1;
        velocidad_periodica = 3584; // 3 rad/s
        if (Time_Out(T)) {
            programa = 0;
            iniciar = 0;
            P2OUT &= ~BIT2;
            P2OUT &= ~BIT0;
            estado = INICIAL;
        }
        break;
    }
}


 void Init_Pulso(void) {
    
    P4DIR |= BIT5 | BIT6 | BIT7;          // P4.0, P4.1, P4.2 output (configurar como salidas digitales)
    P4OUT &= ~(BIT5 | BIT6 | BIT7);         //inicializar en bajo

 }


 //para ponerlo a alto: P4OUT |= BIT0;


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
    if (P1IV){
        if (P1IN & BIT1){
            pulsador1 = 1;
        }
        if (P1IN & BIT5){
            pulsador5 = 1;
        }
        if (P1IN & BIT6){
            pulsador6 = 1;
        }
    }
    return;
}
