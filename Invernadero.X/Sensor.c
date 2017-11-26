#include <xc.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>




bool t_change=false; //Variable que comprueba si se ha efectuado un cambio de 
                    // consigna o no en el intervalo de tiempo de 5 seg

bool t_change_user=false; /*
                           * Se activa en caso de que el usuario haya introducido
                           * cambios en el dial de temperatura 
                           */
unsigned int t_actual = 0; //Valor de la temperatura actual
unsigned int t_ant = 0;

int adc_val; //Variable auxiliar para calcular el voltaje de entrada
int millibolts;  //Variable que indica el voltaje de entrada
int millibolts_aux;

bool t_sys_update = false; //Variable que indica si el sistema tiene o no tiene que
                       // actualizar la consigna de temperatura
unsigned int t_sys_cont = 0;    /* Contador auxiliar que indica si se ha llegado a 5 seg
                        * Como sabemos que cada 0.5 seg el sistema revisa la
                        * consigna del dial para comprobar si se han producido 
                        * cambios, entonces, cuando haga 10 veces esta operacion
                        * habran pasado 5 seg ya que 0.5 * 10 = 5 seg, luego, 
                        * cuando el contador llegue a 10 el sistema actualizara
                        * la consigna de temperatura interior.
                        */

bool t_sys_switch = true; /*
                           * True: el sistema de temperatura esta encendido
                           * False: el sistema de temperatura esta apagado (V > 3.8)
                           */ 

//unsigned char vectorB[sizeof(int)];

/*void sys_display(void)
{
    
}
*/

/*Funcion que se encarga de comprobar si el voltage esta dentro del intervalo
* 0V..3V. Si el voltage es mayor estricto de 3V, entonces se apagara el sistema
*/
void check_t_voltage(void)
{
    if(millibolts >= 0 && millibolts <= 3) t_sys_switch = true;
    else t_sys_switch = false;
        
}

//Funcion que chequea el valor introducido por el usuario
void check_t_changes(void)
{   
    if(millibolts != millibolts_aux) t_change_user = true;
    else t_change_user = false;
}

//Funcion que mide el voltaje 
void t_bolt_value(void)
{
    ADCON0bits.GO = 1;
    
    while (!ADCON0bits.nDONE);
    adc_val = (ADRESH << 8);
    adc_val |= ADRESL;
    
    millibolts_aux = millibolts_aux;
    
    millibolts = (8192 / adc_val) * 1024;
    millibolts /= 8;
}

//Funcion que se encarga de asignar el valor de la temperatura
void t_set(unsigned int  t_next)
{       
        t_ant = t_actual;
        t_actual = t_actual + t_next;
}


//Funcion que se encarga de conseguir el valor de la temperatura
/*unsigned int t_get(void)
{
    
    return t_actual;
}
*/


//Funcion que se activa cuando se produce una interrupcion
void interrupt init_timer(void)
{
    
    /*
     * TBD:
     * Calculo del valor que tiene que tener el timer para 0.5 seg
     * En mi caso como no se hacerlo usare el de la practica 1
     */
    TMR0=60;
    check_t_changes();
    if(t_change_user) t_set(t_actual);
    else t_sys_cont++;
    if(t_sys_cont == 10) t_sys_update = true;
}

//Funcion que configura el registro Ps del timer y que habilita sus interrupciones
void timer_value_set(void)
{
    
    //Configuracion del registro de opciones PS
    OPTION_REGbits.PS = 0b110;
    OPTION_REGbits.nRBPU = 1;
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.T0SE = 0;
    OPTION_REGbits.PSA = 0;
    
    //Habilitar interrupciones
    INTCONbits.GIE = 1;
    INTCONbits.T0IE = 1;
    
    
    
}


void main(void) {

    T0SE = 0;
    
    //LATA = 0x00;
    
    TRISA = 1; //Puerto de entrada de la temperatura
    TRISB = 0; //Puerto de salida de la temperatura
    
    ANSEL = 0;
    ANSELH= 0;
    
    // PORTAbits.RA0
    timer_value_set();
    t_bolt_value();
    check_t_voltage();
    while(t_sys_switch);
}