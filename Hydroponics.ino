#include <elementory.h>

#define TIME_INTERVAL_HRS 5
#define DIVIDE_BY 3600L
#define DRIP_DURATION 27L
#define FAILSAFE_DURATION 30L
int pumppin=12;
int lightpin=2;
int humiditypin=A0;
unsigned long time_start=0;
int tray_height=7;

long last_pump_unix;
DateTime last_pump; 
DateTime local_time;



void update_last_pump(){
    DateTime temp = rtc.now(); 
    NVRAMWritelong(temp.unixtime());
    lcd.setCursor(0,1);
    lcd.print("      ");
    last_pump_unix=NVRAMReadlong();             //check if NVRAM has some value
    last_pump=last_pump_unix;  
    lcd.setCursor(0,1);
    lcd.print(String(last_pump.hour()) +":"+String(last_pump.minute()));   
}

void pump_cycle(int pumppin){
  update_time();
  DateTime failsafe;
  unsigned long millis_start=millis();
  pump_start(pumppin);
  while(1){
      failsafe =rtc.now();
      if(((millis() - millis_start)/1000L) >=DRIP_DURATION) {
        break;
      }
      else if((failsafe.unixtime()-local_time.unixtime())>FAILSAFE_DURATION){
        lcd.setCursor(7,0);
        lcd.print("F");
        break;
      }
      delay(2000);
 }
  pump_stop(pumppin);
  update_last_pump();
  
}

void update_time_remaining(){
  lcd.setCursor(9,0);
  lcd.print("   ");
  lcd.setCursor(9,0);
  Serial.println(((TIME_INTERVAL_HRS*DIVIDE_BY)+last_pump.unixtime())); 
  Serial.println(local_time.unixtime());
  long time_remaining=(((TIME_INTERVAL_HRS*DIVIDE_BY)+last_pump.unixtime())-local_time.unixtime())/60L;
  lcd.print(time_remaining);
}

void update_time(){
  local_time = rtc.now(); 
  lcd.setCursor(0,0);
  lcd.print("     ");
  lcd.setCursor(0,0);
  lcd.print(String(local_time.hour())+":"+String(local_time.minute()));
}

void print_humidity(int pin){
  DHT.read11(pin);
  lcd.setCursor(7,1);
  lcd.print(String(int(DHT.humidity)) + " " + String(int(DHT.temperature))+ "C");
}

void setup() {
   Serial.begin(9600);
   lcd.begin(16,2);    
   lcd.backlight();                 
   rtc.begin();
   last_pump_unix=NVRAMReadlong();             //check if NVRAM has some value
   last_pump=last_pump_unix;
   lcd.setCursor(0,1);
   lcd.print(String(last_pump.hour()) +":"+String(last_pump.minute()));   
   pinMode(pumppin,OUTPUT);
   pinMode(lightpin,OUTPUT);
 
}

void loop() {
  update_time();
  update_time_remaining(); 
  print_humidity(humiditypin);
 // if(local_time.hour() >=0 && local_time.hour() <=6){
 //   digitalWrite(lightpin,HIGH);
 // }
 // else {
 //   digitalWrite(lightpin,LOW);
 // }
  
  if ((local_time.unixtime()/DIVIDE_BY)-(last_pump_unix/DIVIDE_BY)>=TIME_INTERVAL_HRS) {
      lcd.setCursor(9,0);
      lcd.print("Drip ST");
      pump_cycle(pumppin);
      lcd.setCursor(9,0);
      lcd.print("       ");
  }

  delay(60000); 
  
}



