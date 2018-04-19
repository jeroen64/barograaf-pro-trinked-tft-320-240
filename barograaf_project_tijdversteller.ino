 
#include <Wire.h>               //RTC  en BMP
#include "SPI.h"                //TFT  en BMP
#include <Adafruit_ILI9341.h>   //TFT
#include <Adafruit_GFX.h>       //TFT
#include <Adafruit_Sensor.h>    //BMP
#include <Adafruit_BMP280.h>    //BMP 
#include <RTClib.h>             //RTC 
#include <EEPROM.h>             //Resident geheugen 
RTC_DS1307 rtc;                 //RTC
#define TFT_DC 9                //TFT
#define TFT_CS 10               //TFT

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);   //TFT
Adafruit_BMP280 bme;     // I2C voor de BMP
 byte druk[319] ;        // De array die de lijngrafiek maakt voor barometer
 byte temp[319] ;        //  De array die de lijngrafiek maakt voor thermometer    
const byte upbut   =6;           
const byte selbut  =5;              // de drie drukknoppen voor tijdverstelling. 
const byte downbut =4;               // vooor pin  digital 4 5 6
unsigned long recent,past;
int uur;
int minuut;
byte dag;
byte maand;
char maanden[12] [4] = {"jan", "feb", "mrt", "apr", "mei", "jun", "jul", "aug", "sep", "okt", "nov", "dec"};
int jaar;
int barocorr;            // correctie voor de barometerstand
byte barobyte;
int tempcorr;            // correctie voor de temperatuur  
byte tempbyte;

void setup() {         //=================SETUP=================================================
  rtc.begin();
  bme.begin();               
  tft.begin();
  DateTime now = rtc.now();
  Serial.begin(9600); 
 pinMode( selbut, INPUT_PULLUP); 
 pinMode(  upbut, INPUT_PULLUP);
 pinMode(downbut, INPUT_PULLUP);
  tft.setRotation(3);          //zet het scherm horizontaal landscape 0,0 is linksboven 319,0 rechtsboven
 barobyte =  EEPROM[0] ;            // Correctie luchtdruk
 tempbyte =  EEPROM[1] ;            // Correctie temp
             Serial.println(" barobyte en tempbyte   ");
if (barobyte != 0)
 {barocorr= int(barobyte-127);}
if (tempbyte != 0)
 {tempcorr= int(tempbyte-127);}
             Serial.print("barobyte  = ");
             Serial.println(barobyte);             
             Serial.print("barocorr = ");
             Serial.println(barocorr);
             Serial.print("tempbyte = ");
             Serial.println(tempbyte);                          
              Serial.print("tempcorr = ");
             Serial.println(tempcorr);    
 past = EEPROMReadlong( 600 );  // past is nu de waarde die staat op 600
             Serial.print("past =  ");      // this is unix time since data saving
             Serial.println(past);            
 recent = now.unixtime();
            Serial.print(" recent = ");
            Serial.println(recent);
 recent=recent-past;   // recent is nu tijdverschil sinds laatste power down.in secondes
            Serial.print(" tijdverschil =   ");
            Serial.println (recent  );
 recent=recent/900;     // het aantal kwartieren power down = recent /60 /15 = recent/900
              Serial.println("  tijdverschil in kwartieren =  ") ;
              Serial.println(recent); 
         for (int a=24; a<=306; a++)      // hier worden oude meetwaardes opgehaald uit het EEprom
               {druk[a] = EEPROM[a- 22];   // druk[ 24]staat dus op EEpromplaats 2  24- 22=  2
                                           // druk[306]staat dus op EEPromplaats   306- 22=284
                temp[a] = EEPROM[a+261]; } //   temp[24]staat op  285    ,,      24+261=285 
                                            //  temp[306]staat op  567    ,,     306+261=567
 if (recent >= 283) { recent = 283; } // if power down is longer then 283 kwartieren , make alle 283 kwartieren 0                             
   for (int x = 0 ; x <= recent ; x++)      // Het aantal kwartieren dat de grafiek moet worden opgeschoven en met nullen gvuld                          
         { druk[306]=0;
           temp[306]=0;
    //   Serial.print(" druk 306  =  ");
     //  Serial.println( druk [306]);
         for (int a=24; a<=306; a++)      
                    { druk[a]=druk[a+1]; 
                      temp[a]=temp[a+1];}  }
   
 tft.fillScreen(ILI9341_BLACK);     //maak scherm zwart
  tft.setTextSize (1);
    verlijnen();
    horlijnen();
druklijnmaker();
templijnmaker();
}
void loop(void) {  //=====================LOOP=================================================================   
DateTime now = rtc.now();
//Serial.println(" Loop started  ");
 tft.setTextSize (1);
 if (now.minute()%15==0  &&  now.second()==0) //activeert de grafiek elke 15 minuten 
                { tft.fillScreen(ILI9341_BLACK);   
                  verlijnen();
                  horlijnen();     
                  druklijnmaker();
                  templijnmaker(); }  
 minitijdprinter();
 if  (digitalRead(selbut)==LOW) 
      { tijdprinter();
           delay(200);
         timesetter();  } 
 }   //==================================Loop einde====================================
void minitijdprinter()  // =========================minitijdprinter===============
{ DateTime now = rtc.now();   // print de tijd , luchtruk , en temp op onderste regel op elke seconde
  tft.setCursor(0,232);
  tft.setTextColor(ILI9341_YELLOW,0);
         //   float a = float(decapascals()/ 10.0); 
         float a = (bme.readPressure()/100.0) + (barocorr/10.0);    
         tft.print("luchtruk = ");
         if (a<1000){tft.print(" ");}
         tft.print (a);tft.print(" hPa.    ");
         tft.setTextColor(ILI9341_ORANGE,0);
                if (now.hour()<10){ tft.print("0");}
          tft.print(now.hour(),DEC);
          tft.print(':');
               if (now.minute()<10){ tft.print("0");}
         tft.print(now.minute(), DEC);
          tft.print(':');
               if (now.second()<10){ tft.print("0");}
         tft.print(now.second(), DEC);                   
          float b =(bme.readTemperature())+( tempcorr/10.0);
         tft.setTextColor(ILI9341_GREEN,0);
            tft.print("   temp = ");tft.print (b);tft.print(" 'C");
}
//--------------------------------------minitijdprinter einde------------------------
void verlijnen() //----------tekent de verticale lijnen en tijd aanduiding-----------------------------------------
{
  DateTime now = rtc.now();
byte minut  = now.minute();
byte punt=(minut/15);             
byte uur = now.hour();
unsigned int kleur;
for (int x=306; x>25; x-=4)            // gewijzigd for (int x=306; x>24; x-=4)  
   { byte bklijn;
     byte letterplek;  
   if      (uur==0){kleur= ILI9341_RED;bklijn=0;}
  else             { if (uur%6 == 0 && uur != 0)
                           { bklijn = 0; kleur = ILI9341_ORANGE;} 
                      else { bklijn =19; kleur = ILI9341_DARKGREY;} }//gewijzigd orange en anders grijs  
   tft.drawLine(x-punt, bklijn, x-punt, 219,kleur);
   if (uur<10) {letterplek=8;} else {letterplek=14;}
   tft.setCursor(x-letterplek,0);
   tft.setTextColor(ILI9341_ORANGE,0);   
   if (uur%6==0){tft.print(uur);}  //gewijzigd
   uur--; if ( uur> 24) {uur=23;}  // uur is byte. . . negatief wordt 255  . . is groter dan 24
   }   
  }//----------------------einde verlijnen----------------------------------------------------------------------
void druklijnmaker() //--------------------druklijnmaker------------- druk-------------------------------------
{                       // maakt de eigenlijke grafiek van links naar rechts 
                        //luchtdruk grafiekmaker
                 int b = float(decapascals()/-5.0)+2119; //  5 decapascals per pixel
                                                        // druk worden de daadwerkelijke beeldpunten. 1/2 millibar per pixel  
            (float(bme.readPressure())+(float( barocorr)))/50 +2119 ;
     if (b< 19) {b+=200;}          // beeldpunten buiten het bereik worden naar boven of onder doorgegeven.
     if (b>220) {b-=200;}          // bovenaan 306-24=282 beeldpunten reserveren
  druk[306]=b;
       for (int a=24; a<=306; a++)                                
         {  if (druk[a] !=0)
             { Serial.println(druk[306]);
               tft.drawPixel(a,druk[a],ILI9341_YELLOW);}           //schrijft 1 beeldpunt
               druk[a]=druk[a+1];              }                   // schuift alle punten 1 door
      
 }  //-------------einde druklijnmaker-------------------------------------------------------------------------------
 void templijnmaker() //--------------------templijnmaker------------- temp-------------------------------------
{                       // maakt de eigenlijke grafiek van links naar rechts 
                        //  temp grafiekmaker
 int b = float (tiendengraden()*-0.5)+219 ;  // temp worden de daadwerkelijke beeldpunten. 0.2 'C per pixel  negatief want o.o is rechtsboven
     if (b< 19) {b+=200;}          // beeldpunten buiten het bereik worden naar boven of onder doorgegeven.
     if (b>220) {b-=200;}
  temp[306]=b;
       for (int a=24; a<=306; a++)                                 // links 24 pixels vrij en rechts 320-14=306 voor de legenda
         {  if (temp[a] !=0)                                       // Lege beeldpuntenn worden niet geprint
             {   Serial.println("temp in templijnmaker   ");Serial.println(temp[306]);
               tft.drawPixel(a,temp[a],ILI9341_GREEN);}           //schrijft 1 beeldpunt
               temp[a]=temp[a+1];              }                  //schuift alle beeldpunten 1 op      
 }  //-------------einde templijnmaker-------------------------------------------------------------------------------

void horlijnen()//--------------------- geeft horizontale lijnen en luchtdrukwaardes 1050 tot 950 als achtergrond
  {
   tft.setCursor(0,0);tft.setTextColor(ILI9341_YELLOW);
   tft.fillRect(0, 0, 24, 8, ILI9341_BLACK);
   tft.print("hPa");
   tft.setCursor(306,0);tft.setTextColor(ILI9341_GREEN);
   tft.fillRect(306, 0, 12, 8, ILI9341_BLACK);
   tft.setCursor(306,0); 
   tft.print("'C");
  int druk=1050;    // eerste luchtdruk lijn
  byte temp= 40 ;            
    for (byte x=19; x<239; x+=20)
          { tft.setCursor(0,x-8);
            tft.setTextColor(ILI9341_YELLOW);
            tft.print(druk);
            druk-=10;
            tft.drawFastHLine (0,x,320,ILI9341_DARKGREY);
            tft.setCursor(307,x-8);
            tft.setTextColor(ILI9341_GREEN);
            if (temp<10){tft.print(" ");}
            tft.print(temp);
            temp-=4;       }
  }  //----------------------------------------------------------------------------------------------------
   void timesetter() {   //-----------------------------timesetter----------------------------
    DateTime now = rtc.now();               
 jaar   = now.year();
 maand  = now.month();
 dag    = now.day();
 uur    = now.hour();
 minuut = now.minute();
 tft.setTextSize (3);
   tft.setCursor(7*18,2*24);// ---urensteller-----     
   tft.print("->");  
 while(digitalRead(selbut )==HIGH)  {
      if (digitalRead(  upbut)==LOW){uur++; if (uur > 23){uur = 0;}}   // houd de uren tussen 0 en 24
      if (digitalRead(downbut)==LOW){uur--; if (uur < 0){uur = 23;}} 
      rtc.adjust(DateTime(jaar,maand,dag,uur,minuut,0)); // This line sets the RTC with an explicit date & time      
      tft.setCursor(12*18,2*24);                       // 12 plaatsen naar rechts en 3 plaatsen naar beneden
      if (uur  < 10)   { tft.print  ("0");}   // replacing space by the low numbers
       tft.print(uur);     
      delay(200);} 
      tft.setCursor(7*18,2*24);
      tft.print("  "); 
      delay(200);   
 tft.setCursor(7*18,3*24); //--- minutensteller----
 tft.print("->");        
 while(digitalRead(selbut )==HIGH)  {
      if (digitalRead(  upbut)==LOW){minuut++; if (minuut > 59){minuut = 0;}} // houd de minuten tussen 0 en 59
      if (digitalRead(downbut)==LOW){minuut--; if (minuut < 0){minuut = 59;}} 
      rtc.adjust(DateTime(jaar,maand,dag,uur,minuut,0)); // This line sets the RTC with an explicit date & time      
      tft.setCursor(12*18,3*24);                       // 12 plaatsen naar rechts en 3 plaatsen naar beneden
      if (minuut  < 10)   { tft.print  ("0");}        // replacing space by the low numbers
       tft.print(minuut);     
      delay(200);} 
 tft.setCursor(7*18,3*24); 
 tft.print("  ");  
 delay(200);      
 tft.setCursor(7*18,4*24); //---dagsteller-------
 tft.print("->");        
 while(digitalRead(selbut )==HIGH)  {
      if (digitalRead(  upbut)==LOW){dag++; if (dag > 31){dag = 1;}}    // houd  de dagen tussen 0 en 31
      if (digitalRead(downbut)==LOW){dag--; if (dag < 1){dag = 31;}} 
      rtc.adjust(DateTime(jaar,maand,dag,uur,minuut,0)); // This line sets the RTC with an explicit date & time      
      tft.setCursor(12*18,4*24);                       // 12 plaatsen naar rechts en 3 plaatsen naar beneden
      if (dag  < 10)   { tft.print  ("0");}   // replacing space by the low numbers
       tft.print(dag); 
       delay(200);} 
  tft.setCursor(7*18,4*24);
  tft.print("  ");              
  delay(200);      
 tft.setCursor(7*18,5*24); //--- maandsteller------
 tft.print("->");        
 while(digitalRead(selbut )==HIGH)  {
      if (digitalRead(  upbut)==LOW){maand++; if (maand > 12){maand = 1;}}   //  houd de maanden tussen jan en dec 
      if (digitalRead(downbut)==LOW){maand--; if (maand < 1){maand = 12;}} 
      rtc.adjust(DateTime(jaar,maand,dag,uur,minuut,0)); // This line sets the RTC with an explicit date & time      
      tft.setCursor(11*18,5*24);                       // 12 plaatsen naar rechts en 3 plaatsen naar beneden  
      tft.print( maanden[maand - 1]);     
       delay(200);} 
  tft.setCursor(7*18,5*24);
  tft.print("  ");              
  delay(200); 
 tft.setCursor(7*18,6*24); //---jaarsteller------
 tft.print("->");        
 while(digitalRead(selbut )==HIGH)  {
      if (digitalRead(  upbut)==LOW){jaar++; if (jaar > 2100){jaar = 2017;}} // jaar tussen 2017 en 2100 
      if (digitalRead(downbut)==LOW){jaar--; if (jaar <2017){jaar = 2100;}} 
      rtc.adjust(DateTime(jaar,maand,dag,uur,minuut,0)); // This line sets the RTC with an explicit date & time      
      tft.setCursor(10*18,6*24);                        // 12 plaatsen naar rechts en 3 plaatsen naar beneden  
      tft.print(jaar);     
       delay(200);} 
  tft.setCursor(7*18,6*24);
  tft.print("  ");              
  delay(200);   
 tft.setCursor(7*18,7*24); //---luchtdruk correctie per 10 Pascal= 0.1 hPa  tot max + of - 12,7  hPa ---------
 tft.print("->");        
 while(digitalRead(selbut )==HIGH)  {
      if (digitalRead(  upbut)==LOW){barocorr+=1; if (barocorr >  127){barocorr = -127;}}   
      if (digitalRead(downbut)==LOW){barocorr-=1; if (barocorr < -127){barocorr =  127;}} 
      Serial.print(" barocorr =  ");
    Serial.println( barocorr   );
      tft.setCursor(10*18,7*24);                       // 12 plaatsen naar rechts en 3 plaatsen naar beneden      
     float a = float(decapascals()/10.0);
     Serial.print(" bme luchtdruk = ");
     Serial.println( bme.readPressure()   );
     Serial.print(" gecorrigeerde luchtdruk =  ");
     Serial.println (a );
     if (a<1000){tft.print(" ");} 
        barobyte= (barocorr)+127;   
     tft.print(a) ;                //a is de luchtdruk in mbar op 1 decimaal precies
       delay(200);} 
  tft.setCursor(7*18,7*24);
  tft.print("  ");              
  delay(200);            
 tft.setCursor(7*18,8*24); //-------------temp correctie per 0,1  'Celcius------------------------------
 tft.print("->");        
 while(digitalRead(selbut )==HIGH)  {
          if (digitalRead(  upbut)==LOW){tempcorr++; if (tempcorr >  127){tempcorr = -127;}}   
          if (digitalRead(downbut)==LOW){tempcorr--; if (tempcorr < -127){tempcorr =  127;}}
          tempbyte = tempcorr+127;     
          float  b = float (tiendengraden()/10.0); 
          Serial.print(" tempcorr =  ");
          Serial.println(tempcorr);    
          tft.setCursor(12*18,8*24); 
          if (b<10){tft.print(" ");} 
          tft.print (b); // b is de temperatuur op 1 decimaal precies.
          delay(200);} 
  tft.setCursor(7*18,8*24);
  tft.print("  ");              
  delay(200);          
                          // ------ correctiewaardes omzetten naar bytes en opslaan------------------
     tft.setCursor(8*18,13*24);  
   while(digitalRead(selbut )==HIGH){
    tft.setCursor(12*18,9*24);
                tft.print("--");
                delay(100);
                tft.setCursor(12*18,9*24);
                tft.print("  "); 
                delay(100);          
         
         if (digitalRead(downbut)==LOW){             
              tft.setCursor(12*18,9*24);
              tft.print("  ");           // somehow the space here is needed to prevent error on next screen
              return;                   }//------- return so no data is saved --------
         if (digitalRead(  upbut)==LOW){             //------ here correctionfactor temp and press data and time is being saved
                                 tft.setCursor(8*18,13*24);
                                 tft.print("saving ");                                
                                EEPROM[0]= barobyte;            //EEProm  Correctie luchtdruk
                                EEPROM[1]= tempbyte;            //EEProm  Correctie temp
                                                        
                                for (int a=24; a<=306; a++)       // opslag van druk en temp lijn
                                    {EEPROM[a-22] = druk[a];      // druk[24]komt dus op EEpromplaats 2  24- 22=  2
                                                                  // druk[306]komt dus op EEPromplaats  306- 22=284
                                     EEPROM[a+261]= temp[a];}     // temp[24] moet komen op 285          24+261=285 
                                                                  // temp[306] komt dus op  567         306+261=567
                                 recent= now.unixtime();                              
                                EEPROMWritelong( 600,recent ); // unix time is being saved at eeprom place 600
                                Serial.println( recent );                   
                                 tft.setCursor(8*18,13*24);
                                 tft.print("done ");
                                 delay(300);
                                 return ;                   }
               
    }
   }
  //-------------------------------------------------------------- einde timesetter---------------------
  void tijdprinter() {      //-----------------------------------tijdprinter grote letters---------------
  DateTime now = rtc.now();
  tft.fillScreen( ILI9341_DARKGREEN);
  tft.setCursor(0,0);
  tft.setTextColor (ILI9341_YELLOW, ILI9341_DARKGREEN);
  tft.setTextSize (3);
                                 tft.println("set       -    + ");  
                                 tft.println();       
                                 tft.print  ("uren        ") ;
       if (now.hour()  < 10)   { tft.print  ("0");}            // replacing space by the low numbers
                                 tft.println(now.hour(), DEC);
                                 tft.print  ("minuten     ") ;                            
        if (now.minute()< 10)  { tft.print  ("0");}
                                 tft.println(now.minute(), DEC);
                                 tft.print  ("dag         ");
         if  (now.day()< 10)   { tft.print  ("0");}
                                 tft.println(now.day(), DEC);
                                 tft.print  ("maand      ");
                                 tft.println( maanden[now.month() - 1]);
                                 tft.print  ("jaar      ");
                                 tft.println(now.year(), DEC);
                                 tft.print  ("luchtdr.  ");                                 
       float a= float(decapascals()/10.0);
           if (a<1000)          {tft.print  (" ");}
                                 tft.println(a) ;          
                                 tft.print  ("temp        ");
       float b =   float ( tiendengraden()/10);
                                 tft.println(b) ;  
                                tft.print ("opslaan  nee  ja");
                                                 
 }    // einde tijdprinter============================================================================================
                                  

int tiendengraden()//==============================================geeft de temp in tienden graden=================
{  float a= (bme.readTemperature());     // 19.96 graden
  //    Serial.print(F(" Uitlezing  = ")); 
    //   Serial.println(a);
    a*= 10.0;                              //199.6 
    a+= 0.5;                             // 200.1
    //   Serial.print(F(" Afronding = "));
    //   Serial.println(a);         
       a+=tempcorr;  
    return a;                           //  200 retour
  }//===================================================================einde tiendengraden========================================
 
 
 unsigned long decapascals()//=================================geeft de druk in decapascala 1024 mbar = 10240 decapascal===
      {
      unsigned long  a=  (bme.readPressure());   // bijvoorbeeld  a= 102034,23 Pa a=102034
    //   Serial.print(F("Uitlezing  = "));
    //   Serial.println(a);
      a+= 5.0;                          //  a= 102034+5 = 102039    voor de juiste afronding
      a/=10.0;                          //  a= 102034/10= 10203
     //  Serial.print(F("Afronding = "));
     //  Serial.println(a);           
        a=a+barocorr;
       return a;
       }  
   void EEPROMWritelong(int address, unsigned long value)//======================saves the      unsigned long=================
      {    //This function will write a 4 byte (32bit) long to the eeprom at
           //the specified address to address + 3.
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);
      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address,     four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);         }   
unsigned long EEPROMReadlong( unsigned long address)//======================== gets the   unsigned long ========================
{  //This function will return a 4 byte (32bit) long from the eeprom
   //at the specified address to address + 3.
   //Read the 4 bytes from the eeprom memory.
    unsigned long four  = EEPROM.read(address    );
    unsigned long three = EEPROM.read(address + 1);
    unsigned long two   = EEPROM.read(address + 2);
    unsigned long one   = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
      }
       
