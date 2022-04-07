   
#define STEPS 4800 
float NUMSTEPS=5;

#include <Stepper.h> 
Stepper stepper(STEPS, 10, 11, 12, 13);
long rango=2; 
long ultimamedicion;   
int ciclo=0;
int cambiodeciclo=0;
int picodetension;
int valledetension=1023;
long contadorvisualizacion,tiempo=0;
long contadorciclo=0;

int in1 = 6, in2 = 7, EnA = 5;
int Ventilador=3;
int i;

////////////////////////Variables///////////////////////
int Read = 0;
//float elapsedTime, time, timePrev;        //Variables for time control
float previous_error, error;
int period = 50;  //Refresh rate period of the loop is 50ms


///////////////////PID constants///////////////////////
float kp=0.8; 
float ki=0.2; 
float kd=0.005; // kd=0.002;
float PID_p=0, PID_i=0, PID_d=0, PID_total=0;

// va de 630 - 1140   
// int E5= 660, F5=698, G5= 784, B5=988, C6= 1046, D6=1175, nota=0;  REAL
//int C5=520,D5=530,E5=355,F5=698,G5=784,nota=0;     A5= 880, 

int E5= 420, F5=420, G5= 450, B5=470, C6= 500, D6=510, E7=700, nota=0;  
int cancion[]={440, 440, 440, 540, 680, 640, 620, 580, 710, 620,  640, 620, 580, 710, 620,   640, 620, 640, 580, 580, 581, 581, 581}; // para micro 1 (450-830)
//             re,  re,   re,  sol, re, do,  si,  la, sol', re,    do,  si,  la, sol', re,   do,  si,  do , la... 


void setup() {

 Serial.begin(9600);
 pinMode(A0, INPUT);
 stepper.setSpeed(5); //600 rpm, antes 5
 void P();

 pinMode(Ventilador, OUTPUT); 
analogWrite(Ventilador, 160);
//digitalWrite(Ventilador, HIGH);
 
 }
 

void loop() {
   
 long sensorValue = analogRead(A0); 

   if (micros()-tiempo>500000) 
   
   {
        contadorciclo=2*contadorciclo;
        Serial.print("-->");
        Serial.print(cancion[nota]);
        Serial.print("\n"); 
        Serial.println(contadorciclo);
        //Serial.print("t: "); 
        //Serial.println(micros()*0.000001);  
        //Serial.println(micros());            
        rango=(2+((picodetension-valledetension)/5)); // RANGO para REDUCIR ERRORES EN SEÑALES CON RUIDO DE FONDO
        contadorvisualizacion=micros();
        picodetension=sensorValue; 
        valledetension=sensorValue;

        if ( cancion[nota] >= 620){
          if ( cancion[nota] >= 700){
            analogWrite(Ventilador, 230);
          }
          else {analogWrite(Ventilador, 200); }
          }
        else{analogWrite(Ventilador, 160); }

        if ( cancion[nota] == 581){ // Fin de la melodía
          analogWrite(Ventilador, 0);
          stepper.step(0);
          delay(100000);//delay de 100s
          }

        if(micros()>4000000){

          if ( contadorciclo > 380){

              error = cancion[nota] - contadorciclo;   
              PID_p = kp * error;   
              PID_d = kd*((error - previous_error)/1);
                
              if(-4 < error && error < 4) {  PID_i = PID_i + (ki * error); }
              else{  PID_i = 0;  }
            
              PID_total = PID_p + PID_i + PID_d; 
              
              if(PID_total > 100) {PID_total = 100; } 
              
              if( error >= -1 && error <= 1 ){
              Serial.print("Afinado!!\n");
              analogWrite(Ventilador, 255);
              delay(800);
              analogWrite(Ventilador, 150);
              nota++;  // poner un valor de u para al final para que no siga imprimiendo us(=0)
              }
          }//2º if
          else{PID_total = 0;}
          
           stepper.step(-PID_total);
           //Serial.print("error: "); 
           //Serial.println(error );
           //Serial.print("PID: ");
           //Serial.print(PID_total);
           //Serial.println("\n");  
           
           previous_error = error;
          
        }
        
        tiempo=micros();

        contadorciclo=0;   

   }  
   
 if (sensorValue >= ( ultimamedicion+rango) ) // La salida 2 pasa a 1 logico si la tensión ´´ es mayor ´´ + latensión de RANGO

 {
   //Serial.print(" ultimamedicion+rango:\n");
   //Serial.println(ultimamedicion+rango);
   
   ultimamedicion = sensorValue;
   ciclo=1;
   if (sensorValue>picodetension) {
    picodetension=sensorValue;
    //Serial.print("pico:");
    //Serial.println(picodetension);
    }   
 }
 if (sensorValue <= ( ultimamedicion-rango))  
 {
   //Serial.print(" ultimamedicion-rango:\n");
   //Serial.println(ultimamedicion-rango);
   
   ultimamedicion = sensorValue; // <-------------
   ciclo=0; //  EL VOLTAJE EMPIEZA A BAJAR DESDE EL PICO DE TENSIÓN MÁS ALTA
   if (sensorValue<valledetension) {
    valledetension=sensorValue; 
    //Serial.print("valle:");
    //Serial.println(valledetension);    
    
    }
 }
 
if (ciclo==1 && cambiodeciclo==0)
   
   {
     cambiodeciclo=1;
     contadorciclo++;
 
   }
   
if (ciclo==0 && cambiodeciclo==1)
   
   {
     //Serial.print(" ciclo==0 && cambiodeciclo==1 \n");     
     cambiodeciclo=0;
   }


}//loop
