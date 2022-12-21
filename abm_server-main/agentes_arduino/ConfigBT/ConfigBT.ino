#include <SoftwareSerial.h>   

SoftwareSerial BT(10,11);    //6 7  10 11 (rx, tx)


/*
AT
AT+ROLE?
AT+UART?
AT+BIND?
AT+NAME?
At+PSWD?
*/


void setup()
{
  BT.begin(38400);  //AT1:9600 AT2:38400    
  Serial.begin(9600);  

}
 
void loop()
{
  if(BT.available())   
  {
    Serial.write(BT.read());
  }


  if(Serial.available())  
  {
    BT.write(Serial.read());
  }
}