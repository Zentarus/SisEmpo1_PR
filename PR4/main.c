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
/* descripcion : Practica PR4 (motor) (MSP430)                             */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                        Modulos usados                                      */
/******************************************************************************/

#include <msp430.h>
#include "InitSystem.h"
#include "clock.h"
#include "pwm.h"
#include "encoder.h"
#include "servos.h"
#include "ad.h"


/******************************************************************************/
/*                        Variables globales                                  */
/******************************************************************************/

unsigned int siguiente, periodo = 100;
float velocidad_real;
unsigned int velocidad_periodica = 1536;

Timer_id Ti, Ti_buffer ;


float buffer[15];
int indice_buffer = 0;

// Variables globales de la practica

/******************************************************************************/
/*                        Prototipos funciones                                */
/******************************************************************************/

void Init_GPIO (void) ;
float convertir_a_rad_s(unsigned int);


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

    Init_Servos (periodo) ;

    P1IE |= BIT1 | BIT2 ;
    P1IES &= ~BIT1 ;
    P1IES &= ~BIT2 ; 

    siguiente = Get_Time () ;

    unsigned int valor_potenciometro;

    
    float velocidad_referencia;


    Ti = Set_Timer(5000, ONE_SHOT, 0);
    Ti_buffer = Set_Timer(1000, ONE_SHOT, 0);




    while(1)
    {

// Codigo de la practica




        //////////PARTE 1

        // //valor entre 0 y 4096
        // valor_potenciometro = Read_Value_Int_1();
        // Set_Value_10b ((valor_potenciometro >> 2));

        //////////PARTE 2

        // //valor entre 0 y 4096
        // valor_potenciometro = Read_Value_Int_1();


        // // valor entre -4.0 y 4.0 rad/s
        // velocidad_referencia = convertir_a_rad_s(valor_potenciometro);
        // // velocidad real del motor en rad/s
        // velocidad_real = velocity();

        // voltaje_feedback = R(velocidad_referencia, velocidad_real);
        // action(voltaje_feedback);

        //////////PARTE 3
        

        Set_Value_10b(velocidad_periodica >> 2);

        if (Time_Out(Ti)) {
            Remove_Timer(Ti);
            Ti = Set_Timer(5000, ONE_SHOT, 0);
            velocidad_periodica += 1024;
            if (velocidad_periodica == (3584+1024)) {
                velocidad_periodica = 1536;
            }
        }

        if (Time_Out(Ti_buffer)) {
            Remove_Timer(Ti_buffer);
            Ti_buffer = Set_Timer(1000, ONE_SHOT, 0);
            buffer[indice_buffer] = velocity();
            indice_buffer = (indice_buffer + 1) % 14 ;
        }


        siguiente += periodo ;
        delay_until (siguiente) ;    
    }
}

float convertir_a_rad_s(unsigned int valor_entero) {
    return (valor_entero / 4096.0) * 8.0 - 4.0;
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




