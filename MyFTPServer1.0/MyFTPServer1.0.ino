#include<SD.h>
#include<SPI.h>
#include<Ethernet.h>
File sd;
void setup() {
  Serial.begin(9600);
  if(!SD.begin(4))Serial.println("Error");
  else Serial.println("Done");
  sd=SD.open("sample.txt",FILE_WRITE);
  if(sd){
    sd.println("sample text");
    sd.close();
  }else{
    Serial.println("Error");
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
