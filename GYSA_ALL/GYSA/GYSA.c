#include<16F887.h>
#fuses  PROTECT, MCLR
#use delay(internal=4M)
//////funciones//////////////////////////////
void INTERRUPCION();
void arranque();//para buscar lo inicios de carrera del eje z
void eje_z();// para buscar hacia arriba y abajo en el eje z
void eje_x();// para avanzar el robot
void cosechar_der();//para cosechar e fruto del lado derecho
void cosechar_izq();// para cortar el fruto del lado izquierdo
void final(); // para esperar el reinicio del equipo luego de terminar de cosechar
#byte PORTA=5//dedicado a controles y entradas
#byte PORTB=6//dedicado a control de motores y entrada de boton de paro en RB0
#byte PORTC=7//dedicado a indicadores, entrada de los limit switch y encoder
#byte PORTD=8//dedicado a salidas y entrada de los limit switch
#byte PORTE=9//dedicado a entrada de sensores
/////////motor eje z////////////
// Motor 1
#define paso_z PORTB,1//para dar un paso en el motor debemos activar y desactivar esta salida.
#define dir_z PORTB,2// dependiendo del valor de esta salida tendremos una direccionu otra. 0-abajo 1-arriba
#define en_z PORTB,3//Habilitacion de driver, funciona con logica negativa
//motor para posicionar el sistema de corte, lado derecho, Motor 2
#define paso_z1 PORTB,4
#define dir_z1 PORTB,5 //0-derecha 1-izquierda
#define en_z1 PORTB,6
//motor para posicionar el sistema de corte, lado izquierdo, Motor 3
#define paso_z2 PORTB,7
#define dir_z2 PORTD,7 //0-derecha 1-izquierda
#define en_z2 PORTD,6

/////////botones de arranque y paro//////
#define inicio PORTE,0//boton de inicio **** el boton de paro se conecta al RB0****
// el boton de paro se maneja con la interrupcion del pin RB0

///////////sensores de color y encoder////////////
#define sensor1 PORTA,0
#define sensor2 PORTA,1
#define sensor3 PORTA,2
#define sensor4 PORTA,3
#define sensor5 PORTA,4
#define sensor6 PORTA,5
#define sensor7 PORTA,6
#define sensor8 PORTA,7
////////limit switch's////////
#define ls1 PORTD,3
#define ls2 PORTD,2
#define ls3 PORTD,1
#define ls4 PORTD,0
#define ls5 PORTC,7
#define ls6 PORTC,8

/////////indicadores////////////
#define led_inicio PORTC,0// indicador de inicio
#define led_paro PORTC,1//indicador de paro

////////señales de control y comunicación//////
#define cos_der PORTC,2//señal para cosechar en la derecha
#define cos_izq PORTC,3// señal para cosechar en la izquierda
#define avanzar_x PORTC,4// señal para avanzar en x
#define terminado PORTC,5// señal que indica que el otro micro ha terminado su tarea
#define final_x PORTD, 4// señal que indica que se llego al final de x, o sea, al final del programa

////////variables globales//////////////

int velocidad=5; //variable para la velocidad del motor, es una pausa
int8 sensores=0;// almacena la concatenacion de todos los sensores
///////////interrupcion por el boton de paro//////////////////////////

#INT_EXT             //Funcion de la interrupcion
VOID INTERRUPCION()
{
 portb=0b01001000;
 while(!bit_test(inicio)){
 bit_clear(led_inicio);
 bit_set(led_paro);
 delay_ms(250);
 bit_clear(led_paro);
 delay_ms(250);
 }
}
//////////////////////////////////////////////////////////////////////
void main(){
set_tris_a(0b11111111); 
set_tris_b(0b00000001);
set_tris_c(0b11100000);
set_tris_d(0b00111111);
set_tris_e(0b1111);
portb=0b01001000;
portc=0;
portd=0;
ENABLE_INTERRUPTS(GLOBAL);    //Habilita todas las interrupciones
ENABLE_INTERRUPTS(INT_EXT);   //Habilita la interrupción externa, entra en el pin RB0
/////////////espera a presionar el botón de inicio/////
while(!bit_test(inicio)){
   bit_set(led_inicio);
   delay_ms(200);
   bit_clear(led_inicio);
   delay_ms(200);
}
bit_set(led_inicio);
arranque();//vamos a la funcion arranque
/////////////bucle de trabajo///////////////////
while(true){
bit_set(led_inicio);
/////////////busqueda en el eje////////
eje_z();


   }
}
void arranque(){
//aqui movemos los motores del eje z hasta el inicio de carrera
   while(!bit_test(ls1)){// buscamos en el eje del Motor1
      bit_clear(en_z);
      bit_clear(dir_z); //vamos abajo
      bit_set(paso_z);
      delay_ms(velocidad);
      bit_clear(paso_z);
      delay_ms(velocidad);
   }
   bit_set(en_z);
   while(!bit_test(ls3)){//buscamos en el eje del Motor 2 
      bit_clear(en_z1);
      bit_set(dir_z1); //vamos a la izquierda
      bit_set(paso_z1);
      delay_ms(velocidad);
      bit_clear(paso_z1);
      delay_ms(velocidad);
   }
   bit_set(en_z1);
   while(!bit_test(ls5)){// buscamos en el Motor 3
      bit_clear(en_z2);
      bit_set(dir_z2); //vamos a la izquierda
      bit_set(paso_z2);
      delay_ms(velocidad);
      bit_clear(paso_z2);
      delay_ms(velocidad);
   }
   bit_set(en_z2);

}

void eje_z(){
// aqui buscamos frutos maduros en el eje z, en caso de encontarlo posicionamos
// el mecanismo para cortarlo y enviamos el programa a la funcion para cortar el fruto.
while(!bit_test(ls2)){// mientras no lleguemos al final de carrera
      sensores=sensor8,sensor7,sensor6, sensor5, sensor4,sensor3,sensor2,sensor1;
   while(sensores==0 && !bit_test(ls2)){
      sensores=sensor8,sensor7,sensor6, sensor5, sensor4,sensor3,sensor2,sensor1;   
      bit_clear(en_z);
      bit_set(dir_z);
      bit_set(paso_z);
      delay_ms(velocidad);
      bit_clear(paso_z);
      delay_ms(velocidad);
   }
/* para avanzar 1 mm son necesarios 25 pasos del motor*/   
///////sensores del lado derecho///////////////////////   
if(bit_test(sensor1)){//vamos a la posicion del sensor
   for(int16 i=0; i<1;i++){
              for(int i=0; i<25;i++){
               bit_clear(en_z1);
               bit_clear(dir_z1);
               bit_set(paso_z1);
               delay_ms(velocidad);
               bit_clear(paso_z1);
               delay_ms(velocidad);
             }
            }
            bit_set(en_z1);
      cosechar_der();
   }
if(bit_test(sensor2)){//vamos a la posicion del sensor
   for(int16 i=0; i<1;i++){
              for(int i=0; i<25;i++){
               bit_clear(en_z1);
               bit_clear(dir_z1);
               bit_set(paso_z1);
               delay_ms(velocidad);
               bit_clear(paso_z1);
               delay_ms(velocidad);
             }
            }
            bit_set(en_z1);
      cosechar_der();
   }
if(bit_test(sensor3)){//vamos a la posicion del sensor
   for(int16 i=0; i<1;i++){
              for(int i=0; i<25;i++){
               bit_clear(en_z1);
               bit_clear(dir_z1);
               bit_set(paso_z1);
               delay_ms(velocidad);
               bit_clear(paso_z1);
               delay_ms(velocidad);
             }
            }
            bit_set(en_z1);
      cosechar_der();
   }


if(bit_test(sensor4)){//vamos a la posicion del sensor
   for(int16 i=0; i<1;i++){
              for(int i=0; i<25;i++){
               bit_clear(en_z1);
               bit_clear(dir_z1);
               bit_set(paso_z1);
               delay_ms(velocidad);
               bit_clear(paso_z1);
               delay_ms(velocidad);
             }
            }
            bit_set(en_z1);
      cosechar_der();
   } 
 /////////sensores izquierda///////////////////////  
if(bit_test(sensor5)){//vamos a la posicion del sensor
   for(int16 i=0; i<1;i++){
              for(int i=0; i<25;i++){
               bit_clear(en_z2);
               bit_clear(dir_z2);
               bit_set(paso_z2);
               delay_ms(velocidad);
               bit_clear(paso_z2);
               delay_ms(velocidad);
             }
            }
            bit_set(en_z2);
      cosechar_izq();
   }    
if(bit_test(sensor6)){//vamos a la posicion del sensor
   for(int16 i=0; i<1;i++){
              for(int i=0; i<25;i++){
               bit_clear(en_z2);
               bit_clear(dir_z2);
               bit_set(paso_z2);
               delay_ms(velocidad);
               bit_clear(paso_z2);
               delay_ms(velocidad);
             }
            }
            bit_set(en_z2);
      cosechar_izq();
   }
if(bit_test(sensor7)){//vamos a la posicion del sensor
   for(int16 i=0; i<1;i++){
              for(int i=0; i<25;i++){
               bit_clear(en_z2);
               bit_clear(dir_z2);
               bit_set(paso_z2);
               delay_ms(velocidad);
               bit_clear(paso_z2);
               delay_ms(velocidad);
             }
            }
            bit_set(en_z2);
      cosechar_izq();
   }
if(bit_test(sensor8)){//vamos a la posicion del sensor
   for(int16 i=0; i<1;i++){
              for(int i=0; i<25;i++){
               bit_clear(en_z2);
               bit_clear(dir_z2);
               bit_set(paso_z2);
               delay_ms(velocidad);
               bit_clear(paso_z2);
               delay_ms(velocidad);
             }
            }
            bit_set(en_z2);
      cosechar_izq();
   }   
  
//////////////////////////////////////////////////
 
}
// en caso de que lleguemos al final de carrera avanzamos en el eje x
eje_x();
// aqui buscamos frutos maduros en el eje z hacia abajo, en caso de encontarlo posicionamos
// el mecanismo para cortarlo y enviamos el programa a la funcion para cortar el fruto.
while(!bit_test(ls1)){// mientras no lleguemos al final de carrera
      sensores=sensor6, sensor5, sensor4,sensor3,sensor2,sensor1;
   while(sensores==0 && !bit_test(ls1)){
      sensores=sensor6, sensor5, sensor4,sensor3,sensor2,sensor1;   
      bit_clear(en_z);
      bit_clear(dir_z);
      bit_set(paso_z);
      delay_ms(velocidad);
      bit_clear(paso_z);
      delay_ms(velocidad);
   }
/* para avanzar 1 mm son necesarios 25 pasos del motor*/   
///////sensores del lado derecho///////////////////////   
if(bit_test(sensor1)){//vamos a la posicion del sensor
   for(int16 i=0; i<1;i++){
              for(int i=0; i<25;i++){
               bit_clear(en_z1);
               bit_clear(dir_z1);
               bit_set(paso_z1);
               delay_ms(velocidad);
               bit_clear(paso_z1);
               delay_ms(velocidad);
             }
            }
            bit_set(en_z1);
      cosechar_der();
   }
if(bit_test(sensor2)){//vamos a la posicion del sensor
   for(int16 i=0; i<1;i++){
              for(int i=0; i<25;i++){
               bit_clear(en_z1);
               bit_clear(dir_z1);
               bit_set(paso_z1);
               delay_ms(velocidad);
               bit_clear(paso_z1);
               delay_ms(velocidad);
             }
            }
            bit_set(en_z1);
      cosechar_der();
   }
if(bit_test(sensor3)){//vamos a la posicion del sensor
   for(int16 i=0; i<1;i++){
              for(int i=0; i<25;i++){
               bit_clear(en_z1);
               bit_clear(dir_z1);
               bit_set(paso_z1);
               delay_ms(velocidad);
               bit_clear(paso_z1);
               delay_ms(velocidad);
             }
            }
            bit_set(en_z1);
      cosechar_der();
   }

/////////sensores izquierda///////////////////////
if(bit_test(sensor4)){//vamos a la posicion del sensor
   for(int16 i=0; i<1;i++){
              for(int i=0; i<25;i++){
               bit_clear(en_z2);
               bit_clear(dir_z2);
               bit_set(paso_z2);
               delay_ms(velocidad);
               bit_clear(paso_z2);
               delay_ms(velocidad);
             }
            }
            bit_set(en_z2);
      cosechar_izq();
   } 
if(bit_test(sensor5)){//vamos a la posicion del sensor
   for(int16 i=0; i<1;i++){
              for(int i=0; i<25;i++){
               bit_clear(en_z2);
               bit_clear(dir_z2);
               bit_set(paso_z2);
               delay_ms(velocidad);
               bit_clear(paso_z2);
               delay_ms(velocidad);
             }
            }
            bit_set(en_z2);
      cosechar_izq();
   }    
if(bit_test(sensor6)){//vamos a la posicion del sensor
   for(int16 i=0; i<1;i++){
              for(int i=0; i<25;i++){
               bit_clear(en_z2);
               bit_clear(dir_z2);
               bit_set(paso_z2);
               delay_ms(velocidad);
               bit_clear(paso_z2);
               delay_ms(velocidad);
             }
            }
            bit_set(en_z2);
      cosechar_izq();
   }    
//////////////////////////////////////////////////
 
}
// en caso de que lleguemos al final de carrera avanzamos en el eje x
eje_x();
}

void cosechar_der(){
// aqui cosechamos del lado derecho, enviamos la señal al otro micro y esperamos la indicación de terminado, luego continuamos
   bit_set(cos_der);
   while(!bit_test(terminado)){// esperamos la señal de terminado
   bit_set(cos_der);
   }
   bit_clear(cos_der);
   // ahora solo continuamos con el código
}

void cosechar_izq(){
// aqui cosechamos del lado izquierdo, enviamos la señal al otro micro y esperamos la indicación de terminado, luego continuamos
   bit_set(cos_izq);
   while(!bit_test(terminado)){// esperamos la señal de terminado
   bit_set(cos_izq);
   }
   bit_clear(cos_izq);// continuamos con el programa
}

void eje_x(){
// aqui le decimos al otro micro que avance en x, por defecto avanzará 18 cm
   bit_set(avanzar_x);
   while(!bit_test(terminado) && !bit_test(final_x)){
   bit_set(avanzar_x);
   }
  if(bit_test(final_x)){// si llegamos al final de carrera mandamos al void final
      final();
   }else{
      bit_clear(avanzar_x);
   }
}

void final(){
// en esta funcion solo esperamos a que el usuario apague y vuelva a encender el equipo luego de posicionarlo para hacer una nueva cosecha 
   while(true){
      bit_set(led_inicio);
      bit_set(led_paro);
      delay_ms(500);
      bit_clear(led_inicio);
      bit_clear(led_paro);
      delay_ms(500);
   }
}
