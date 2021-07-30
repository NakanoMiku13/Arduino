#include <LiquidCrystal_I2C.h>
#include<Wire.h>
#include<Keypad.h>
#include<EEPROM.h>
#include<Servo.h>
const byte ROWS=4;
const byte COLS=4;
const byte LcdRow=2;
const byte LcdCol=16;
const char DfPwd[]={'1','4','0','8','0','2','A'};
const byte rowPin[ROWS]={9,8,7,6};
const byte colPin[COLS]={5,4,3,2};
const char Keys[ROWS][COLS]={
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
LiquidCrystal_I2C lcd(0x3F,LcdCol,LcdRow);
Keypad customKeypad = Keypad(makeKeymap(Keys),rowPin,colPin,ROWS,COLS);
Servo servo;
char text[17];
char tmp[17];
int i=0,iT=0;
auto Check()->void{
  int x=EEPROM.read(0);
  i=0;
  if(x>0 and x<10){
    for(int j=1;j<=x;j++,i++){
      text[i]=(char)EEPROM.read(j);
    }
  }
}
auto Set(int x)->void{
  lcd.setCursor(0,x);
}
auto GetKey()->char{
  char customKey=customKeypad.getKey();
  return customKey;
}
auto Clean()->void{
  lcd.clear();
}
auto PrintText()->void{
  Set(0);
  lcd.print("Ingresalo:");
  char customKey=GetKey();
  if(customKey){
    Clean();
    if(customKey=='*'){
      EEPROM.write(0,i);
      Clean();
      Set(0);
      lcd.print("Guardando...");
      delay(3000);
      Clean();
      lcd.print("Listo");
      delay(1000);
      Clean();
      return;
    }else{
      text[i]=customKey;
      EEPROM.write(i+1,(int)customKey);
      i++; 
    }
  }
  if(i>0){
    Set(1);
    lcd.print(text);
  }
  PrintText();
}
auto NewPwd()->void{
  i=0;
  Clean();
  Set(1);
  lcd.print("Terminar *");
  Set(0);
  lcd.print("Ingresalo: ");
  PrintText();
}
auto PrintPwd()->void{
  Clean();
  Set(0);
  lcd.print("Contrasenia:");
  Set(1);
  if(i>0){
    lcd.print(text);
  }else{
    lcd.print(DfPwd);  
  }
  delay(2500);
  Clean();
}
auto SetConfig()->void{
   Set(0);
   lcd.print("1)Cambiar pwd");
   Set(1);
   lcd.print("2)Ver pwd");
   char x=GetKey();
   if(x){
      if(x=='1'){
        NewPwd();
      }else if(x=='2'){
        PrintPwd();
      }else{
        Clean();
        Set(0);
        lcd.print("Op no val");
        delay(1000);
        Clean();
      }
   }else{
    SetConfig();
   }
}
auto PwdCheck()->bool{
  for(int j=0;j<i;j++){
    if(tmp[j]!=text[j]){
      return false;
    }
  }
  return true;
}
auto ClearTmp()->void{
  memset(tmp,0,sizeof(tmp));
  iT=0;
}
auto Open()->void{
  servo.write(65);
}
auto Close()->void{
  servo.write(5);
}
auto TypePwd()->void{
  Set(0);
  lcd.print("Contraseia:");
  char x=GetKey();
  if(x){
    if(x=='*'){
      Clean();
      Set(0);
      lcd.print("Espera...");
      delay(1500);
      Clean();
      if(PwdCheck()){
        lcd.print("Correcto");
        Open();
      }else{
        lcd.print("Incorrecto");
      }
      delay(1000);
      Clean();
      ClearTmp();
      return;
    }else{
      tmp[iT]=x;
      iT++;  
    }
  }
  if(iT>0){
    Set(1);
    lcd.print(tmp);
  }
  TypePwd();
}
auto Menu()->void{
  char x=GetKey();
  if(x){
    Clean();
    if(x=='D'){
      SetConfig();
    }else if(x=='*'){
      Close();
    }else{
      TypePwd();
    }  
  }
}
auto setup()->void {
  Serial.begin(9600);
  servo.attach(13);
  servo.write(5);
  lcd.init();
  lcd.backlight();
  Check();
  loop();
}
auto loop()->void {
  Set(0);
  lcd.print("Ingresa #");
  Menu();
}
