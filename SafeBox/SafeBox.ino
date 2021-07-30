//Code by NakanoMiku13
//Check more codes on https://github.com/NakanoMiku13
//SafeBox
#include <LiquidCrystal_I2C.h>
#include<Wire.h>
#include<Keypad.h>
#include<EEPROM.h>
#include<Servo.h>
const byte ROWS=4;
const byte COLS=4;
const byte LcdRow=2;
const byte LcdCol=16;
//Contraseña por defecto
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
//Funcion para checar si hay algo guardado en la memoria, en este caso la contraseña ingresada por el usuario
auto Check()->void{
  int x=EEPROM.read(0);
  i=0;
  if(x>0 and x<10){
    for(int j=1;j<=x;j++,i++){
      text[i]=(char)EEPROM.read(j);
    }
  }
}
//Función para ajustar la fila en la que se imprimirá el mensaje
auto Set(int x)->void{
  lcd.setCursor(0,x);
}
//Obtener la tecla pulsada por el usuario en el keypad
auto GetKey()->char{
  char customKey=customKeypad.getKey();
  return customKey;
}
//Limpiar pantalla
auto Clean()->void{
  lcd.clear();
}
//Funcion donde vemos el ingreso de la nueva contraseña
auto PrintText()->void{
  Set(0);
  lcd.print("Ingresalo:");
  char customKey=GetKey();
  if(customKey){
    Clean();
    if(customKey=='*'){
      //En la posicion 0 de la memoria de la arduino se guarda la cantidad de datos a almacenar
      //EEPROM nos permite guardarla como un arreglo EEPROM.write(posicion,valor)
      //Solo un valor por celda y la memoria es limitada, cuidado
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
//Funcion General para nueva contraseña
auto NewPwd()->void{
  i=0;
  Clean();
  Set(1);
  lcd.print("Terminar *");
  Set(0);
  lcd.print("Ingresalo: ");
  PrintText();
}
//Funcion para imprimir contraseña, en caso de que no se haya cambiado previamente, se imprimirá la default
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
//Funcion de configuración de contraseña, ver o cambiar contraseña
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
//Funcion para checar que la contraseña ingresada por el usuario sea igual a la guardada
auto PwdCheck()->bool{
  for(int j=0;j<i;j++){
    if(tmp[j]!=text[j]){
      return false;
    }
  }
  return true;
}
//Limpiar el arreglo temporal de la contraseña
auto ClearTmp()->void{
  memset(tmp,0,sizeof(tmp));
  iT=0;
}
//Funcion para abrir el servo
auto Open()->void{
  servo.write(65);
}
//Funcion para cerrar el servo
auto Close()->void{
  servo.write(5);
}
//Funcion donde se ingresa la contraseña y posteriormente es verificada para ver si es la correcta
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
//Menu de opciones, al usuario no se le despliega, pero ingresando D se abre la configuración para cambiar o ver la contraseña
//Y la opción * cierra el servo, ya que despues de ingresar la contraseña se queda abierto (hasta que se apague la arduino)
//Y cualquier otra telca nos permite ingresar la contraseña
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
