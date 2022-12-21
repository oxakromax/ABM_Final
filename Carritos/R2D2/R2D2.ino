#include <Adafruit_MotorShield.h>
#include <SoftwareSerial.h>   

// Para los Motores
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor1 = AFMS.getMotor(1);
Adafruit_DCMotor *myMotor2 = AFMS.getMotor(2);
// Pines para el BT
SoftwareSerial BT(10,11);    //6 7  10 11 (rx, tx)
// Variables para el mensaje BT
unsigned char *buffer;
String mensaje = "";

// Estados de lectura en BT
#define LEYENDO 1
#define LEIDO 2
#define ESPERANDO 0
// Estados del movimiento del carro
#define CARGA 1
#define ADELANTE 2
#define DESCARGA 3
#define ATRAS 4
#define DURMIENDO 7
// Estado Simulacion Gruas
#define Activar_AD 'w'
#define Activar_AT 's'
// Estado del desvio
#define DESACTIVADO 0
#define ACTIVADO 1

// Dicionario
// Mensaje para matar al robot
#define Desactivar 'f'
// Mensaje desde grua 1
#define inGrua1 'a'
// Mensaje Para grua1
#define outGrua1 'c'
// Mensaje desde grua 2
#define inGrua2 'r'
// Mensaje para grua 2
#define outGrua2 'd'

// Sensores
#define echoPin 12 
#define trigPin 13 
long duration1;
int distance1;

//Atras
#define echoPin2 2 
#define trigPin2 3 
long duration2;
int distance2;
// Variables estado
int estado; // estado del carro
int desviado; // si el carro se fue de trayecto

void setup() {
  // Se inicia el bluetooth, AT1 es para usar AT2 para configurar
  BT.begin(9600);  //AT1:9600 AT2:38400    
  buffer = malloc(1000); // Memoria del Buffer
  Serial.begin(9600);           // set up Serial library at 9600 bps

  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
  // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);}

  // Pines para los sensores  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  pinMode(trigPin2, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin2, INPUT); // Sets the echoPin as an INPUT
  // Set the speed to start, from 0 (off) to 255 (max speed)

  estado = DURMIENDO;
  desviado = DESACTIVADO;

  myMotor1->setSpeed(30);
  myMotor1->run(FORWARD);
  myMotor1->run(RELEASE);
  myMotor2->setSpeed(30);
  myMotor2->run(FORWARD);
  myMotor2->run(RELEASE);
  //BT.write(outGrua1);
}
// Se inicia la variable estado lectura y se marca la posicion a sobreescribir
int st_read = ESPERANDO;
int size = 0;

void recibir(){
  unsigned char c;
  if(BT.available())   
  { 
    //Serial.write(BT.read());
    c = BT.read();
    //Serial.println(char(c));
    if (st_read == LEYENDO && c == '}')
    {
      size = size + 1;
      buffer[size] = c;      
      for (int i=0; i<size+1;i++){
          mensaje =  mensaje + String(char(buffer[i]));}

      st_read = LEIDO;}

    if (st_read == ESPERANDO && c == '{'){
      size = 0;
      st_read = LEYENDO;
      buffer[size] = c;}

    if (st_read == LEYENDO && c != '{' && c != '}'){
      size = size + 1;
      buffer[size] = c;}
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



int distAtras(){
  // Trig 12 y Echo 11
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration1 = pulseIn(echoPin, HIGH);
  return duration1 * 0.034 / 2;
}

int distFrente(){
  // Trig 3 y echo 2
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  return duration2 * 0.034 / 2;
}

void loop() {
  // Percepciones
  recibir(); // Intentar leer un byte del mensaje
  distance1 = distFrente(); // Mirar al frente
  distance2 = distAtras(); // Mirar Atras
  //Serial.print("Estado: ");
  //Serial.println(estado);
  //Serial.print(distance1);
  //Serial.print("-");
  //Serial.println(distance2);
  //  Verificar si tengo un mensaje disponible
  // Reglas con mensaje
  if (mensaje_disp()){
    String aux = leerMensaje();
    // AUX[3] Mensajes que me llegan a mi (R1)
    // AUX[1] Mensajes que leo de G1
    // AUX[2] Mensajes que leo de G2
    if (aux[3] == Activar_AD){estado = ADELANTE ;} 
    if (aux[3] == Activar_AT){estado = ATRAS ;} 
    if (aux[1] == inGrua1 && estado == CARGA){estado = ADELANTE;}         
    if (aux[2] == inGrua2 && estado == DESCARGA){estado = ATRAS;}
    if (aux[3] == Desactivar){estado = DURMIENDO;} 
    //Serial.println(aux);
    //Serial.println(estado);  
    aux = "";
    }
  
  // Reglas con Distancia
  if (distance1 < 5 && estado == ADELANTE){//llegue enviar mensaje Grua2
    estado = DESCARGA;
    BT.write(outGrua2);}

  if (distance2 < 5 && estado == ATRAS){//llegue enviar mensaje Grua1
    estado = CARGA;
    BT.write(outGrua1);}
  
  if ((distance1 == 0 && distance2 == 0)||(distance1 > 1000 && distance2 > 1000)){estado = DURMIENDO;}  
  //   Si algun sensor marca mal
  if (distance1 == 0 || distance1 > 180 || distance2 == 0 || distance2 > 180 ){
    desviado = ACTIVADO;  }

  // Si los sensores vuelven a marcar bien
  if (desviado == ACTIVADO && distance1 != 0 && distance2 != 0 && distance1<=180 && distance2<=180)  { desviado = DESACTIVADO;}
  
  //
  //
  //Acciones segun los estados definidos por reglas anteriores
  if (estado == ADELANTE){
      myMotor1->run(FORWARD);
      myMotor2->run(FORWARD);}

  if (estado == ATRAS){ 
      myMotor1->run(BACKWARD);
      myMotor2->run(BACKWARD);}

  if (estado == CARGA || estado == DESCARGA){
    myMotor1->run(RELEASE);
    myMotor2->run(RELEASE); }    

  if (desviado == ACTIVADO){
    myMotor1->run(RELEASE);
    myMotor2->run(RELEASE); 
    
     }

  if (estado == DURMIENDO){
    myMotor1->run(RELEASE);
    myMotor2->run(RELEASE);  }

  if (desviado == DESACTIVADO && estado == ADELANTE){
      myMotor1->run(FORWARD);
      myMotor2->run(FORWARD);}

  if (desviado == DESACTIVADO && estado == ATRAS){
    myMotor1->run(BACKWARD);
    myMotor2->run(BACKWARD);}



}