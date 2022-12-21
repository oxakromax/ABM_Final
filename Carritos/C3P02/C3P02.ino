#include <Adafruit_MotorShield.h>
#include <SoftwareSerial.h>   

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotor1 = AFMS.getMotor(1);
Adafruit_DCMotor *myMotor2 = AFMS.getMotor(2);
// You can also make another motor on port M2
//Adafruit_DCMotor *myOtherMotor = AFMS.getMotor(2);

SoftwareSerial BT(10,11); //(TX-10,RX-11) -> cables en arduino
//SENSORES ULTRASONIDO
//SENSOR 1
#define echoPin 2 
#define trigPin 4
//SENSOR 2
#define echoPin2 7 
#define trigPin2 6

// Módulo Bluetooth
// lectura
#define LEYENDO 1
#define LEIDO 2
#define ESPERANDO 0

// Mensaje grua 1
#define inGrua1 'a' //AVANZAR (MENSAJE RECIBIDO)
#define outGrua1 'c' //CARGADO (MENSAJE ENVIADO)
//mensaje grua2
#define inGrua2 'r' //RETROCEDER (MENSAJE RECIBIDO)
#define outGrua2 'd' //DESCARGAR (MENSAJE ENVIADO)

//ESTADOS
#define INICIO 0
#define G1 1 // avanza hacia grua 1
#define E1 2 // espera la carga de grua 1
#define G2 3 // avanza hacia grua 2
#define E2 4 // espera la descarga de grua 2
#define D 8 // DESVIADO

#define activado 'g'
#define desactivado 's' 
int temporal;

// ESTADOS INTERNOS
#define cargado 6
#define descargado 7
#define salida 13

//variables para las funciones
int st_read = ESPERANDO;
int size = 0;

//otras variables
String mensaje;
unsigned char *buffer;

int st_carrito = descargado;
int ESTADO = INICIO;
int DISTANCIA;
int DISTANCIA2;
int buttonState;

/*---------------------------------------------------------------------------------------------------------*/
//FUNCIONES GENERALES

//CALCULO DISTANCIAS CON LOS SENSORES
int sensor1(){
  long duracion;
  int distancia;  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duracion = pulseIn(echoPin, HIGH); //microsegundos
  distancia = duracion * 0.034 / 2;
  return distancia;
}

int sensor2(){
  long duracion;
  int distancia;  
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duracion = pulseIn(echoPin2, HIGH); //microsegundos
  distancia = duracion * 0.034 / 2;
  return distancia;
}

//entrada y salida de mensajes en el BT

void recibir(){
  unsigned char c;
  if(BT.available())   
  { 

    //Serial.write(BT.read());
    c = BT.read();
    Serial.println(char(c));
    if (st_read == LEYENDO && c == '}')
    {
      size = size + 1;
      buffer[size] = c;      
      for (int i=0; i<size+1;i++){
          mensaje =  mensaje + String(char(buffer[i]));
          
      }
      st_read = LEIDO;
    }
    if (st_read == ESPERANDO && c == '{'){
      size = 0;
      st_read = LEYENDO;
      buffer[size] = c;
    }
    if (st_read == LEYENDO && c != '{' && c != '}'){
      size = size + 1;
      buffer[size] = c;
    }
  }  
}

int mensaje_disp(){
    if (st_read == LEIDO){
      return 1;
    }
    return 0;
}

String leerMensaje(){
  String aux = mensaje;
    mensaje = "";
    st_read = ESPERANDO;
    return aux;
}

/* SETUP */

void setup() {
  BT.begin(9600);  //AT1:9600 AT2:38400    
  buffer = malloc(1000);
  Serial.begin(9600);           // set up Serial library at 9600 bps

  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
  // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
  pinMode(trigPin2, OUTPUT); 
  pinMode(echoPin2, INPUT);
  pinMode(salida, OUTPUT);

  // Set the speed to start, from 0 (off) to 255 (max speed)
  myMotor1->setSpeed(70);
  myMotor1->run(FORWARD);
  myMotor1->run(RELEASE);
  myMotor2->setSpeed(70);
  myMotor2->run(FORWARD);
  myMotor2->run(RELEASE);
  digitalWrite(salida,HIGH);
}

/* LOOP  */


void loop() {
  recibir();
  DISTANCIA = sensor1();
  DISTANCIA2 = sensor2();

  if (mensaje_disp()){
    String aux = leerMensaje();   
     
  if(aux[4] == activado){
    temporal = 1;
    //Serial.println("activado");
    }
    
  if(aux[4] == desactivado){
    temporal = 0;
    myMotor1->run(RELEASE);
    myMotor2->run(RELEASE);
    //Serial.println("desactivado");
   }

   // el carrito debe estar encendido para funcionar
  if(temporal == 1){ 
    if (aux[1] == inGrua1 && st_carrito == descargado){
      ESTADO = G2;
    }
    
    if (aux[1] == outGrua1){
      ESTADO = E2;
      st_carrito = cargado;
      BT.write(outGrua1);
    }

    if (aux[2] == inGrua2 && st_carrito == cargado){
      ESTADO = G1;  
    } 

    if (aux[2] == outGrua2){
      ESTADO = E1;
      st_carrito = descargado;
      BT.write(outGrua2);
    }

  }
  aux = "";
  }

  // if dedicados a la detencion ante obstaculos y el desvio
  if (DISTANCIA<10 || DISTANCIA2<10){
    myMotor1->run(RELEASE);
    myMotor2->run(RELEASE);      
  }

  if (DISTANCIA>150 || DISTANCIA2>150){
      ESTADO = D;
      myMotor1->run(RELEASE);
      myMotor2->run(RELEASE); 
  }

    //volver a la marcha cuando esta en el carril segun su estado interno
  if (ESTADO == D && DISTANCIA<150 && DISTANCIA2<150){
    if(st_carrito == descargado){
      ESTADO = E1;
      //Serial.println("corregido E1");
      }
    if(st_carrito == cargado){
      ESTADO = E2;
      //Serial.println("corregido E2");
      }
  }

  // acciones segun el estado el estado que corresponda
  if(ESTADO == G2){
    myMotor1->run(BACKWARD);
    myMotor2->run(BACKWARD);
    myMotor1->setSpeed(30);
    myMotor2->setSpeed(30);
  }  

  if (ESTADO == G1) {
    myMotor1->run(FORWARD);
    myMotor2->run(FORWARD);
    myMotor1->setSpeed(30);
    myMotor2->setSpeed(30);
  }

  delay(10);
}