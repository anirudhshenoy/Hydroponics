#include <elementory.h>

#define TIME_INTERVAL_HRS 12
#define DIVIDE_BY 3600L
int pumppin=10;
int waterpin=A0;
int water_level=0;
unsigned long time_start=0;
int tray_height=7;

long last_pump_unix;
DateTime last_pump; 
DateTime local_time;


int get_water_level(int water_pin){
    return (tray_height-get_ultrasonic_distance(water_pin));
    
}


void update_last_pump(){
    DateTime temp = rtc.now(); 
    NVRAMWritelong(temp.unixtime());
    lcd.setCursor(0,1);
    lcd.print("             ");
    last_pump_unix=NVRAMReadlong();             //check if NVRAM has some value
    last_pump=last_pump_unix;  
    lcd.setCursor(0,1);
    lcd.print("Last "+ String(last_pump.hour())+":"+String(last_pump.minute())+"  "+String(last_pump.day())+"/"+String(last_pump.month()));   
}

void pump_cycle(int pumppin){
  update_time();
  DateTime failsafe;
  pump_start(pumppin);
  while(1){
      water_level=get_water_level(waterpin);
      failsafe =rtc.now();
      if(water_level>4) {
        break;
      }
      else if(((failsafe.unixtime()/60L)-(local_time.unixtime()/60L))>7L){
        lcd.setCursor(7,0);
        lcd.print("F");
        break;
      }
      delay(2000);
 }
  pump_stop(pumppin);
  update_last_pump();
  while(1){
      water_level=get_water_level(waterpin);
      if(water_level<=1) {
        break;   
      }
      delay(1500);
   }
}

void update_time(){
  local_time = rtc.now(); 
  lcd.setCursor(0,0);
  lcd.print("     ");
  lcd.setCursor(0,0);
  lcd.print(String(local_time.hour())+":"+String(local_time.minute()));

}

void setup() {
   lcd.begin(16,2);    
   lcd.backlight();                 
   rtc.begin();
   last_pump_unix=NVRAMReadlong();             //check if NVRAM has some value
   last_pump=last_pump_unix;
   lcd.setCursor(0,1);
   lcd.print("Last "+ String(last_pump.hour())+":"+String(last_pump.minute())+"  "+String(last_pump.day())+"/"+String(last_pump.month()));
   pinMode(pumppin,OUTPUT);
 
}

void loop() {
  update_time(); 
 
  if ((local_time.unixtime()/DIVIDE_BY)-(last_pump_unix/DIVIDE_BY)>=TIME_INTERVAL_HRS) {
    for (int cycle=0 ; cycle<=2; cycle++){
      
      lcd.setCursor(9,0);
      lcd.print("Pump:" + String(cycle));
      pump_cycle(pumppin);
      delay(180000);
      }
    lcd.setCursor(9,0);
    lcd.print("       ");
  }

  delay(60000); 
  
}



