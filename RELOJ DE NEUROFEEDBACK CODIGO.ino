#include <TFT_eSPI.h> 
#include <SPI.h> 
#include <BluetoothSerial.h>
#include "uwu.h"
#include <string.h>
#include <TFT_eWidget.h>      
#define DEBUG(a) Serial.println(a);
#include <SoftwareSerial.h>
SoftwareSerial mySerial(27, 26); // RX, TX
float volt=0;
float adcraw=0;
int x=1;
const int potPin = 1;
float meditation1;
float concentration1;
String dato;
int signal, concentration, meditation,delta,theta,lowalpha,highalpha,lowbeta,highbeta,lowgamma,highgamma; //aumentas mas
BluetoothSerial SerialBT;
String name = "FER";
char *pin = "1234"; //<- standard pin would be provided by default
bool connected;
TFT_eSPI tft = TFT_eSPI(); 
TFT_eSprite sprite= TFT_eSprite(&tft);
GraphWidget gr = GraphWidget(&tft);    // Graph widget
TraceWidget genTrace = TraceWidget(&gr);    // Graph trace 1
TraceWidget useTrace = TraceWidget(&gr);    // Graph trace 2
const int kScreenResX = 200;
const int kScreenResY = 135;
const int kXGraphMarginLeft = 30; // fit the legend at left
const int kXGraphMarginRight = 10;
const int kYGraphMarginTop = 5;
const int kYGraphMarginBottom = 15; // fit the legend below
const int kSamples = 100; // number of power samples to keep
const float kXMin = 0.0;
const float kXMax = float(kSamples);
const float kYMin = -10.0;  // minimum power 
float gYMax = 10.0; // maximum power seen
float gGenSamples[kSamples+1];
float gUseSamples[kSamples+1];
const int  pulsador_asc = 0;    // pin 2 como entrada para el contador de pulsos 
const int  pulsador_des = 35;    // pin 3 como entrada para el contador de pulsos
int contador            = 0;   // contador para el numero de veces presinados
int estado_pulsador_asc = 0;         // estado actual del pulsador
int lastButtonState_asc = 1;     // estado anterior del pulsador
int estado_pulsador_des = 0;         // estado actual del pulsador
int lastButtonState_des = 1;     // estado anterior del pulsador
int graphdelta=0;
int deltamax=0;
int deltamin=0;
int functiontheta=0;
int graphtheta=0;
int thetamax=0;
int thetamin=0;
int graphlowalpha=0;
int lowalphamax=0;
int lowalphamin=0;
int functionhighalpha=0;
int graphhighalpha=0;
int highalphamax=0;
int highalphamin=0;
int graphlowbeta=0;
int lowbetamax=0;
int lowbetamin=0;
int functionhighbeta=0;
int graphhighbeta=0;
int highbetamax=0;
int highbetamin=0;
int graphlowgamma=0;
int lowgammamax=0;
int lowgammamin=0;
int functionhighgamma=0;
int graphhighgamma=0;
int highgammamax=0;
int highgammamin=0;
int color;
int batOx = 200;
int batOy = 0;
int delta100=0;
int theta100=0;  
int lowalpha100=0;
int highalpha100=0;
int lowbeta100=0;
int highbeta100=0;
int lowgamma100=0;
int highgamma100=0;
void setup() {
  float batteryVoltage = ((float)analogRead(34) / 4095.0) * 2.0 * 3.3 * (1100 / 1000.0);
  if (batteryVoltage > 4.0) {
  Serial.begin(9600);
  SerialBT.begin("ESP32test", true); 
  connected = SerialBT.connect(name);
  SerialBT.connect();
  }else{
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  tft.pushImage(60,0,128,128,uwu);
  delay(5000);
  tft.fillScreen(TFT_BLACK);
  tft.drawString("Conecting",10,10,2);
  Serial.begin(9600);
  SerialBT.begin("ESP32test", true); 
  connected = SerialBT.connect(name);
  SerialBT.connect();
  while (!SerialBT.connected()) {
    tft.fillScreen(TFT_BLACK);  
    delay(500);
    }
tft.drawString("CONECTED!!",10,10,2);
delay(5000);
tft.fillScreen(TFT_BLACK);
}
gr.createGraph(kScreenResX - kXGraphMarginLeft - kXGraphMarginRight, kScreenResY - kYGraphMarginTop - kYGraphMarginBottom, tft.color565(5, 5, 5));
configGraphScale();
clearSamples();
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
pinMode(12,OUTPUT);
pinMode(13,OUTPUT);
pinMode(15,OUTPUT);
pinMode(17,OUTPUT);
pinMode(22,OUTPUT);
pinMode(33,OUTPUT);
pinMode(34,INPUT);
pinMode(pulsador_asc, INPUT_PULLUP); //Resistencia de pullup interna
pinMode(pulsador_des, INPUT_PULLUP); //Resistencia de pullup interna
mySerial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
sprite.createSprite(240,135);
}
void configGraphScale() {
  // x scale units is from 0 to 100, y scale units is -50 to 50
  gr.setGraphScale(0.0, 100.0, kYMin, gYMax);
  // X grid starts at 0 with lines every 10 x-scale units
  // Y grid starts at -50 with lines every 25 y-scale units
  // blue grid
  gr.setGraphGrid(0.0, 10.0, kYMin, (gYMax - kYMin) / 10.0, 0x734D);
  // Draw empty graph, top left corner at 40,10 on TFT
  gr.drawGraph(kXGraphMarginLeft, kYGraphMarginTop);
  // Draw the x axis scale
  tft.setTextDatum(TC_DATUM); // Top centre text datum
  tft.drawNumber(kXMin, gr.getPointX(0.0), gr.getPointY(kYMin) + 3);
  tft.drawNumber(kXMax / 2, gr.getPointX(kXMax / 2), gr.getPointY(kYMin) + 3);
  tft.drawNumber(kXMax, gr.getPointX(kXMax), gr.getPointY(kYMin) + 3);
  // Draw the y axis scale
  tft.setTextDatum(MR_DATUM); // Middle right text datum
  tft.drawNumber(kYMin, gr.getPointX(0.0), gr.getPointY(kYMin));
  float halfValue = ((gYMax - kYMin) / 2) + kYMin;  
  tft.drawNumber(halfValue, gr.getPointX(0.0), gr.getPointY(halfValue));
  tft.drawNumber(gYMax, gr.getPointX(0.0), gr.getPointY(gYMax));
  //Serial.printf("min = %f, half = %f, max = %f\n", kYMin, halfValue, gYMax);
  // Restart traces with new colours
  genTrace.startTrace(TFT_WHITE);
  useTrace.startTrace(TFT_YELLOW);
}
void clearSamples() {
  for(int i = 0; i < kSamples; i++) {
    gGenSamples[i] = 0.0;
    gUseSamples[i] = 0.0;
  }
}
float maxInSamples() {
  float max = 0.0;
  for(int i = 0; i < kSamples; i++) {
    if(gGenSamples[i] > max) {
      max = gGenSamples[i];
    }
    if(gUseSamples[i] > max) {
      max = gUseSamples[i];
    }
  }
  return max;
}
void addSample(float gen, float use) {
  slideArrayBack();
  gGenSamples[kSamples] = gen;
  gUseSamples[kSamples] = use;
  // slowly drop gYMax over time
  gYMax -= 10;
  // see if we need to increase gYMax
  float maxInArray = maxInSamples();
  if(maxInArray > gYMax){
    gYMax = maxInArray + 11;
  }
  // round up to the next multiple of 10
  if (int(gYMax) % 10) {
     gYMax = float(int(gYMax) + (10 - int(gYMax) % 10));
  }
}
void slideArrayBack() {
  for(int i = 0; i < kSamples; i++) {
    gGenSamples[i] = gGenSamples[i+1];
    gUseSamples[i] = gUseSamples[i+1];
  }
}
void visudatos(){
if(delta>deltamax){
    deltamax=delta;
  }
  if(delta<deltamin){
    deltamin=delta;
  }
  graphdelta=map(delta,deltamin,deltamax,0,60);
  if(theta>thetamax){
    thetamax=theta;
  }
  if(theta<thetamin){
    thetamin=theta;
  }
  graphtheta=map(theta,thetamin,thetamax,0,46);
  functiontheta=sin(45)*(graphtheta);
   if(lowalpha>lowalphamax){
    lowalphamax=lowalpha;
  }
  if(lowalpha<lowalphamin){
    lowalphamin=lowalpha;
  }
  graphlowalpha=map(lowalpha,lowalphamin,lowalphamax,0,60);
  if(highalpha>highalphamax){
    highalphamax=highalpha;
  }
  if(highalpha<highalphamin){
    highalphamin=highalpha;
  }
  graphhighalpha=map(highalpha,highalphamin,highalphamax,0,46);
  functionhighalpha=sin(45)*(graphhighalpha);
  if(lowbeta>lowbetamax){
    lowbetamax=lowbeta;
  }
  if(lowbeta<lowbetamin){
    lowbetamin=lowbeta;
  }
  graphlowbeta=map(lowbeta,lowbetamin,lowbetamax,0,60);
  if(highbeta>highbetamax){
    highbetamax=highbeta;
  }
  if(highbeta<highbetamin){
    highbetamin=highbeta;
  }
  graphhighbeta=map(highbeta,highbetamin,highbetamax,0,46);
  functionhighbeta=sin(45)*(graphhighbeta);
  if(lowgamma>lowgammamax){
    lowgammamax=lowgamma;
  }
  if(lowgamma<lowgammamin){
    lowgammamin=lowgamma;
  }
  graphlowgamma=map(lowgamma,lowgammamin,lowgammamax,0,60);
  if(highgamma>highgammamax){
    highgammamax=highgamma;
  }
  if(highgamma<highgammamin){
    highgammamin=highgamma;
  }
  graphhighgamma=map(highgamma,highgammamin,highgammamax,0,46);
  functionhighgamma=sin(45)*(graphhighgamma);

}
void drawGraph()
  { 
  sprite.fillSprite(TFT_BLACK);
  float batteryVoltage = ((float)analogRead(34) / 4095.0) * 2.0 * 3.3 * (1100 / 1000.0);
  if (batteryVoltage > 4.0) {
    sprite.setTextSize(1);
    sprite.setCursor(20,0);
    sprite.drawString("Cargando",150,0,1);
  }
   else if (batteryVoltage> 3.9 ) {  
    sprite.drawLine(batOx, batOy, batOx, batOy + 17, TFT_WHITE);
    sprite.drawLine(batOx + 1, batOy, batOx + 1, batOy + 17, TFT_WHITE);
    sprite.drawLine(batOx, batOy, batOx + 31, batOy, TFT_WHITE);
    sprite.drawLine(batOx, batOy + 1, batOx + 31, batOy + 1, TFT_WHITE);
    sprite.drawLine(batOx + 30, batOy, batOx + 30, batOy + 5, TFT_WHITE);
    sprite.drawLine(batOx + 31, batOy, batOx + 31, batOy + 5, TFT_WHITE);
    sprite.drawLine(batOx, batOy + 16, batOx + 31, batOy + 16, TFT_WHITE);
    sprite.drawLine(batOx, batOy + 17, batOx + 31, batOy + 17, TFT_WHITE);
    sprite.drawLine(batOx + 30, batOy + 17, batOx + 30, batOy + 12, TFT_WHITE);
    sprite.drawLine(batOx + 31, batOy + 17, batOx + 31, batOy + 12, TFT_WHITE);
    sprite.drawLine(batOx + 32, batOy + 4, batOx + 32, batOy + 5, TFT_WHITE);
    sprite.drawLine(batOx + 32, batOy + 12, batOx + 32, batOy + 13, TFT_WHITE);
    sprite.drawLine(batOx + 33, batOy + 4, batOx + 33, batOy + 13, TFT_WHITE);
    sprite.drawLine(batOx + 34, batOy + 4, batOx + 34, batOy + 13, TFT_WHITE);
    sprite.fillRect(batOx + 4, batOy + 3, 6, 12, TFT_WHITE);
    sprite.fillRect(batOx + 12, batOy + 3, 6, 12, TFT_WHITE);  // 66%
    sprite.fillRect(batOx + 20, batOy + 3, 6, 12, TFT_WHITE);  // 100%
  } else if (batteryVoltage >3.7 ) {  
    sprite.drawLine(batOx, batOy, batOx, batOy + 17, TFT_WHITE);
    sprite.drawLine(batOx + 1, batOy, batOx + 1, batOy + 17, TFT_WHITE);
    sprite.drawLine(batOx, batOy, batOx + 31, batOy, TFT_WHITE);
    sprite.drawLine(batOx, batOy + 1, batOx + 31, batOy + 1, TFT_WHITE);
    sprite.drawLine(batOx + 30, batOy, batOx + 30, batOy + 5, TFT_WHITE);
    sprite.drawLine(batOx + 31, batOy, batOx + 31, batOy + 5, TFT_WHITE);
    sprite.drawLine(batOx, batOy + 16, batOx + 31, batOy + 16, TFT_WHITE);
    sprite.drawLine(batOx, batOy + 17, batOx + 31, batOy + 17, TFT_WHITE);
    sprite.drawLine(batOx + 30, batOy + 17, batOx + 30, batOy + 12, TFT_WHITE);
    sprite.drawLine(batOx + 31, batOy + 17, batOx + 31, batOy + 12, TFT_WHITE);
    sprite.drawLine(batOx + 32, batOy + 4, batOx + 32, batOy + 5, TFT_WHITE);
    sprite.drawLine(batOx + 32, batOy + 12, batOx + 32, batOy + 13, TFT_WHITE);
    sprite.drawLine(batOx + 33, batOy + 4, batOx + 33, batOy + 13, TFT_WHITE);
    sprite.drawLine(batOx + 34, batOy + 4, batOx + 34, batOy + 13, TFT_WHITE);
    sprite.fillRect(batOx + 4, batOy + 3, 6, 12, TFT_WHITE);
    sprite.fillRect(batOx + 12, batOy + 3, 6, 12, TFT_WHITE);  // 66%
  }
  else if(batteryVoltage>3.4) {
    sprite.drawLine(batOx, batOy, batOx, batOy + 17, TFT_WHITE);
    sprite.drawLine(batOx + 1, batOy, batOx + 1, batOy + 17, TFT_WHITE);
    sprite.drawLine(batOx, batOy, batOx + 31, batOy, TFT_WHITE);
    sprite.drawLine(batOx, batOy + 1, batOx + 31, batOy + 1, TFT_WHITE);
    sprite.drawLine(batOx + 30, batOy, batOx + 30, batOy + 5, TFT_WHITE);
    sprite.drawLine(batOx + 31, batOy, batOx + 31, batOy + 5, TFT_WHITE);
    sprite.drawLine(batOx, batOy + 16, batOx + 31, batOy + 16, TFT_WHITE);
    sprite.drawLine(batOx, batOy + 17, batOx + 31, batOy + 17, TFT_WHITE);
    sprite.drawLine(batOx + 30, batOy + 17, batOx + 30, batOy + 12, TFT_WHITE);
    sprite.drawLine(batOx + 31, batOy + 17, batOx + 31, batOy + 12, TFT_WHITE);
    sprite.drawLine(batOx + 32, batOy + 4, batOx + 32, batOy + 5, TFT_WHITE);
    sprite.drawLine(batOx + 32, batOy + 12, batOx + 32, batOy + 13, TFT_WHITE);
    sprite.drawLine(batOx + 33, batOy + 4, batOx + 33, batOy + 13, TFT_WHITE);
    sprite.drawLine(batOx + 34, batOy + 4, batOx + 34, batOy + 13, TFT_WHITE);
    sprite.fillRect(batOx + 4, batOy + 3, 6, 12, TFT_WHITE);
  }
  sprite.drawCircle(85,60,60, 0x4A08);
  sprite.drawCircle(85,60,40, 0x4A08);
  sprite.drawCircle(85,60,20, 0x4A08);
  sprite.drawLine(85,4,85,120, 0x4A08);
  sprite.drawLine(25,60,145,60, 0x4A08);
  sprite.drawLine(131,20,39,100, 0x4A08);
  sprite.drawLine(131,100,39,20, 0x4A08);
  visudatos();
  color=0x07F7;
  sprite.drawPixel(85,60-graphdelta,color);//First variable DELTA
  sprite.drawLine(85,60-graphdelta,85+graphtheta,60-(functiontheta),color);//LINE 1
  sprite.drawPixel(85+graphtheta,60-(functiontheta),color);//second variable A THETA
  sprite.drawLine(85+graphtheta,60-(functiontheta),85+graphlowalpha,60,color);//LINE 2
  sprite.drawPixel(85+graphlowalpha,60,color);//third variable LOW ALPHA
  sprite.drawLine(85+graphlowalpha,60,85+graphhighalpha,60+(functionhighalpha),color);//LINE 3
  sprite.drawPixel(85+graphhighalpha,60+(functionhighalpha),color);//fourth variable A HIGH ALPHA
  sprite.drawLine(85+graphhighalpha,60+(functionhighalpha),85,60+graphlowbeta,color);  //LINE 4
  sprite.drawPixel(85,60+graphlowbeta,color); //fifth variable LOW BETA
  sprite.drawLine(85,60+graphlowbeta,85-graphhighbeta,60+(functionhighbeta),color);  //LINE 6
  sprite.drawPixel(85-graphhighbeta,60+(functionhighbeta),color); //sixth variable  A HIGH BETA
  sprite.drawLine(85-graphhighbeta,60+(functionhighbeta),85-graphlowgamma,60,color);  //LINE 7
  sprite.drawPixel(85-graphlowgamma,60,color);//seventh variable LOW GAMMA
  sprite.drawLine(85-graphlowgamma,60,85-graphhighgamma,60-(functionhighgamma),color);    //LINE 8
  sprite.drawPixel(85-graphhighgamma,60-(functionhighgamma),color);  //eighvariable A HIGH GAMMA
  sprite.drawLine(85-graphhighgamma,60-(functionhighgamma),85,60-graphdelta,color);//LINE 9
  sprite.setTextColor(0xF7C0);//color delta
  sprite.drawString("Delta",73,0,1);
  sprite.setTextColor(0x57C0);//color theta
  sprite.drawString("Theta",135,20,1);
  sprite.setTextColor(0x07D5);//color L ALPHA
  sprite.drawString("L/",148,52,1);
  sprite.drawString("Alpha",148,62,1);
  sprite.setTextColor(0x065F);  //color H APLHA
  sprite.drawString("H/Alpha",133,101,1);
  sprite.setTextColor(0x029F);  //color L BETA
  sprite.drawString("L/Beta",73,122,1);
  sprite.setTextColor(0xA07C);  //color H BETA
  sprite.drawString("H/Beta",5,103,1);
  sprite.setTextColor( 0xF819);  //color L GAMMA
  sprite.drawString("L/",0,52,1);
  sprite.drawString("Gamma",0,62,1);
  sprite.setTextColor(0xF80C); //color H GAMMA 
  sprite.drawString("H/Gamma",0,10,1);
  sprite.setTextColor(0xF4E0); //color FOCUS
  sprite.drawString("Focus",178,115,1);
  sprite.drawString(String(concentration-1),178,125,1); //variable de atencion en banda mental    
  sprite.fillRect(190,112-(concentration-20),10,(concentration-20),0xF4E0);    //sin problema porque los valores son de 0 a 100   
  sprite.setTextColor(0xFAC0); //color FOCUS 
  sprite.drawString("Medit",210,115,1);
  sprite.drawString(String(meditation-1),210,125,1); //variable de medit en banda mental     
  sprite.fillRect(220,112-(meditation-20),10,(meditation-20),0xFAC0); //sin problema porque los valores son de 0 a 100    
  sprite.pushSprite(0,0);
  }
void plotData() {
  // Draw empty graph at 40,10 on display
  gr.drawGraph(kXGraphMarginLeft, kYGraphMarginTop);
  // Start new trace
  genTrace.startTrace(TFT_GREEN);
  useTrace.startTrace(TFT_RED);
  for(int i = 0; i < kSamples; i++) {
    genTrace.addPoint(i, gGenSamples[i]);
    useTrace.addPoint(i, gUseSamples[i]);
    //USE_SERIAL.printf("plot x = %d, y = %f\n", i, value); 
  }
}
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
void loop() {
   if (SerialBT.available()>0) {
    dato = SerialBT.readStringUntil('\n');
    DEBUG(dato);
  dato.trim();
  signal = getValue(dato, ',', 0).toInt();
  concentration = getValue(dato, ',', 1).toInt();
  meditation = getValue(dato, ',', 2).toInt();
  delta = getValue(dato, ',', 3).toInt();
  theta = getValue(dato, ',', 4).toInt();
  lowalpha= getValue(dato, ',', 5).toInt();
  highalpha= getValue(dato, ',', 6).toInt();
  lowbeta= getValue(dato, ',', 7).toInt();
  highbeta= getValue(dato, ',', 8).toInt();
  lowgamma= getValue(dato, ',', 9).toInt();
  highgamma= getValue(dato, ',', 10).toInt();
  }
  dato="";
mySerial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
//  almacenamos la lectura de la entrada de pin 2
  estado_pulsador_asc = digitalRead(pulsador_asc);
  estado_pulsador_des = digitalRead(pulsador_des);
  // comparar el estado del botón a su estado anterior
  if (estado_pulsador_asc != lastButtonState_asc) {
    // si el estado fue cambiado, incremente el conteo
    if (estado_pulsador_asc == HIGH) {
      // si el estado actual es alto, entonces 
      // que pase de off a on:
      contador++;
      tft.fillScreen(TFT_BLACK);
graphdelta=0;
deltamax=0;
deltamin=0;
functiontheta=0;
graphtheta=0;
thetamax=0;
thetamin=0;
graphlowalpha=0;
lowalphamax=0;
lowalphamin=0;
functionhighalpha=0;
graphhighalpha=0;
highalphamax=0;
highalphamin=0;
graphlowbeta=0;
lowbetamax=0;
lowbetamin=0;
functionhighbeta=0;
graphhighbeta=0;
highbetamax=0;
highbetamin=0;
graphlowgamma=0;
lowgammamax=0;
lowgammamin=0;
functionhighgamma=0;
graphhighgamma=0;
highgammamax=0;
highgammamin=0;
    } 
  }
 lastButtonState_asc  = estado_pulsador_asc;
   // comparar el estado del botón a su estado anterior
  if (estado_pulsador_des != lastButtonState_des ) {
    // si el estado fue cambiado, decrementa el conteo
    if (estado_pulsador_des == HIGH) {
      // si el estado actual es alto, entonces 
      // que pase de off a on:
      contador--;
      tft.fillScreen(TFT_BLACK);
      graphdelta=0;
deltamax=0;
deltamin=0;
functiontheta=0;
graphtheta=0;
thetamax=0;
thetamin=0;
graphlowalpha=0;
lowalphamax=0;
lowalphamin=0;
functionhighalpha=0;
graphhighalpha=0;
highalphamax=0;
highalphamin=0;
graphlowbeta=0;
lowbetamax=0;
lowbetamin=0;
functionhighbeta=0;
graphhighbeta=0;
highbetamax=0;
highbetamin=0;
graphlowgamma=0;
lowgammamax=0;
lowgammamin=0;
functionhighgamma=0;
graphhighgamma=0;
highgammamax=0;
highgammamin=0;
    } 
  }
  //guarda el último estado actual como el ultimo estado
  //para el proximo bucle
  lastButtonState_des  = estado_pulsador_des;
if(contador<0){
  
   
}
else if(contador>4){
tft.fillRect(5, 4, 240, 16, TFT_RED);
tft.setCursor(150,4);
tft.setTextColor(TFT_WHITE); 
tft.setTextSize(potPin);
adcraw = meditation;
volt=  adcraw;
tft.print(adcraw);

tft.setTextColor(TFT_WHITE,TFT_RED);
tft.drawRect(0,20,200,100,TFT_BLUE);
tft.drawPixel(x++,map(adcraw,0,100,120,20),TFT_BLUE);
tft.drawLine(x,map(adcraw,0,100,120,20), x, 120,0xC85E );//Relleno
x=x+1;
if(x>199)
{tft.fillRect(1,20,200,100, TFT_BLACK);
  x=1; 
}
if(meditation>85){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,HIGH);
    digitalWrite(17,HIGH);
    digitalWrite(22,HIGH);
    digitalWrite(33,LOW);
  }else if(meditation>60){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,HIGH);
    digitalWrite(17,HIGH);
    digitalWrite(22,LOW);
    digitalWrite(33,LOW);
    mySerial.write("d");
    
  }else if(meditation>40){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,HIGH);
    digitalWrite(17,LOW);
    digitalWrite(22,LOW);
    digitalWrite(33,LOW);
    
  }else if(meditation>30){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,LOW);
    digitalWrite(17,LOW);
    digitalWrite(22,LOW);
    digitalWrite(13,HIGH);
    if(concentration>50){
    digitalWrite(33,LOW);
   }else{
     mySerial.write("u");
     digitalWrite(33,HIGH);
   }
    
  }else{
    digitalWrite(12,HIGH);
    digitalWrite(13,LOW);
    digitalWrite(15,LOW);
    digitalWrite(17,LOW);
    digitalWrite(22,LOW);
  
    if(concentration>50){
    
    digitalWrite(33,LOW);
    
    }else{
    
    digitalWrite(33,HIGH);}
    mySerial.write("u");

  }

delay(90);
}else if(contador>3) {
  addSample(concentration, meditation );
  configGraphScale();
            plotData();
  tft.setTextColor(TFT_GREEN,TFT_BLACK);          
  tft.drawString("Focus",230,8,1);
  tft.drawString(String(concentration-1),230,23,2);
  tft.setTextColor(TFT_RED,TFT_BLACK);
  tft.drawString("Medit",230,38,1);
  tft.drawString(String(meditation-1),230,53,2);
  tft.setTextColor(TFT_BLUE,TFT_BLACK);
  tft.drawString("Mode:",230,70,1);
  tft.drawString("MEDIT",230,80,1);
  tft.drawString("u:",230,90,1);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  if(meditation>85){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,HIGH);
    digitalWrite(17,HIGH);
    digitalWrite(22,HIGH);
    digitalWrite(33,LOW);
    mySerial.write("d");
  }else if(meditation>60){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,HIGH);
    digitalWrite(17,HIGH);
    digitalWrite(22,LOW);
    digitalWrite(33,LOW);
    mySerial.write("d");
    
  }else if(meditation>40){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,HIGH);
    digitalWrite(17,LOW);
    digitalWrite(22,LOW);
    digitalWrite(33,LOW);
    
  }else if(meditation>30){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,LOW);
    digitalWrite(17,LOW);
    digitalWrite(22,LOW);
    digitalWrite(13,HIGH);
    if(concentration>50){
    digitalWrite(33,LOW);
   }else{
     mySerial.write("u");
     digitalWrite(33,HIGH);
   }
    
  }else{
    digitalWrite(12,HIGH);
    digitalWrite(13,LOW);
    digitalWrite(15,LOW);
    digitalWrite(17,LOW);
    digitalWrite(22,LOW);
  
    if(concentration>50){
    
    digitalWrite(33,LOW);
    
    }else{
    
    digitalWrite(33,HIGH);
    mySerial.write("u");}
    

  }
   delay(1500);
   
  
}else if(contador>2){
tft.fillRect(5, 4, 240, 16, TFT_RED);
tft.setCursor(150,4);
tft.setTextColor(TFT_WHITE); 
tft.setTextSize(potPin);
adcraw = concentration;
volt=  adcraw;
tft.print(adcraw);
tft.setTextColor(TFT_WHITE,TFT_RED);
tft.drawRect(0,20,200,100,TFT_BLUE);
tft.drawPixel(x++,map(adcraw,0,100,120,20),TFT_BLUE);
tft.drawLine(x,map(adcraw,0,100,120,20), x, 120,0xC85E );//Relleno
x=x+1;
if(x>199)
{tft.fillRect(1,20,200,100, TFT_BLACK);
  x=1; 
}
if(concentration>85){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,HIGH);
    digitalWrite(17,HIGH);
    digitalWrite(22,HIGH);
    digitalWrite(33,LOW);
  }else if(concentration>60){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,HIGH);
    digitalWrite(17,HIGH);
    digitalWrite(22,LOW);
    digitalWrite(33,LOW);
    mySerial.write("d");
    
  }else if(concentration>40){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,HIGH);
    digitalWrite(17,LOW);
    digitalWrite(22,LOW);
    digitalWrite(33,LOW);
    
  }else if(concentration>30){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,LOW);
    digitalWrite(17,LOW);
    digitalWrite(22,LOW);
    digitalWrite(13,HIGH);
    if(meditation>50){
    digitalWrite(33,LOW);
   }else{
     mySerial.write("u");
     digitalWrite(33,HIGH);
   }
    
  }else{
    digitalWrite(12,HIGH);
    digitalWrite(13,LOW);
    digitalWrite(15,LOW);
    digitalWrite(17,LOW);
    digitalWrite(22,LOW);
  
    if(meditation>50){
    
    digitalWrite(33,LOW);
    
    }else{
    
    digitalWrite(33,HIGH);}
    mySerial.write("u");

  }

delay(90);
}else if(contador>1) {
  concentration1=concentration;
  meditation1=meditation;
  addSample(concentration1, meditation1 );
  configGraphScale();
            plotData();
  tft.setTextColor(TFT_GREEN,TFT_BLACK);          
  tft.drawString("Focus",230,8,1);
  tft.drawString(String(concentration-1),230,23,2);
  tft.setTextColor(TFT_RED,TFT_BLACK);
  tft.drawString("Medit",230,38,1);
  tft.drawString(String(meditation-1),230,53,2);
  tft.setTextColor(TFT_BLUE,TFT_BLACK);
  tft.drawString("Mode:",230,70,1);
  tft.drawString("STUDY",230,80,1);
  tft.drawString("(:",230,90,1);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  if(concentration>85){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,HIGH);
    digitalWrite(17,HIGH);
    digitalWrite(22,HIGH);
    digitalWrite(33,LOW);
    mySerial.print(concentration);
    mySerial.write(",");
    mySerial.print(meditation);
    mySerial.write(",");
    mySerial.write("P");
    mySerial.write(",");
    mySerial.write("A");
    mySerial.write(",");
    mySerial.println("");
  }else if(concentration>60){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,HIGH);
    digitalWrite(17,HIGH);
    digitalWrite(22,LOW);
    digitalWrite(33,LOW);
    mySerial.print(concentration);
    mySerial.write(",");
    mySerial.print(meditation);
    mySerial.write(",");
    mySerial.write("P");
    mySerial.write(",");
    mySerial.write("B");
    mySerial.write(",");
    mySerial.println("");
  }else if(concentration>40){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,HIGH);
    digitalWrite(17,LOW);
    digitalWrite(22,LOW);
    digitalWrite(33,LOW);
    mySerial.print(concentration);
    mySerial.write(",");
    mySerial.print(meditation);
    mySerial.write(",");
    mySerial.write("P");
    mySerial.write(",");
    mySerial.write("C");
    mySerial.write(",");
    mySerial.println("");
  }else if(concentration>30){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,LOW);
    digitalWrite(17,LOW);
    digitalWrite(22,LOW);
    digitalWrite(13,HIGH);
    mySerial.print(concentration);
    mySerial.write(",");
    mySerial.print(meditation);
    mySerial.write(",");
    mySerial.write("S");
    mySerial.write(",");
    mySerial.write("X");
    mySerial.write(",");
    mySerial.println("");
    if(meditation>50){
    digitalWrite(33,LOW);
   }else{
     digitalWrite(33,HIGH);
     
   }
    
  }else{
    digitalWrite(12,HIGH);
    digitalWrite(13,LOW);
    digitalWrite(15,LOW);
    digitalWrite(17,LOW);
    digitalWrite(22,LOW);
    mySerial.print(concentration);
    mySerial.write(",");
    mySerial.print(meditation);
    mySerial.write(",");
    mySerial.write("S");
    mySerial.write(",");
    mySerial.write("X");
    mySerial.write(",");
    mySerial.println("");
    if(meditation>50){
    digitalWrite(33,LOW);
    }else{
    digitalWrite(33,HIGH);
    }
  }
   delay(1500);
  }else{
  drawGraph();
  if(concentration>85){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,HIGH);
    digitalWrite(17,HIGH);
    digitalWrite(22,HIGH);
    digitalWrite(33,LOW);
  }else if(concentration>60){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,HIGH);
    digitalWrite(17,HIGH);
    digitalWrite(22,LOW);
    digitalWrite(33,LOW);
  }else if(concentration>40){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,HIGH);
    digitalWrite(17,LOW);
    digitalWrite(22,LOW);
    digitalWrite(33,LOW);
  }else if(concentration>30){
    digitalWrite(12,HIGH);
    digitalWrite(13,HIGH);
    digitalWrite(15,LOW);
    digitalWrite(17,LOW);
    digitalWrite(22,LOW);
    digitalWrite(13,HIGH);
    if(meditation>50){
    digitalWrite(33,LOW);
   }else{
     digitalWrite(33,HIGH);
   }
  }else{
    digitalWrite(12,HIGH);
    digitalWrite(13,LOW);
    digitalWrite(15,LOW);
    digitalWrite(17,LOW);
    digitalWrite(22,LOW);
    if(meditation>50){
    digitalWrite(33,LOW);
    }else{
    digitalWrite(33,HIGH);
    }
}  
mySerial.print(concentration);
mySerial.write(",");
mySerial.print(meditation);
mySerial.write(",");
delta100=map(delta,deltamin,deltamax,0,100);
mySerial.print(delta100);
mySerial.write(",");
theta100=map(theta,thetamin,thetamax,0,100);
mySerial.print(theta100);
mySerial.write(",");
lowalpha100=map(lowalpha,lowalphamin,lowalphamax,0,100);
mySerial.print(lowalpha100);
mySerial.write(",");
highalpha100=map(highalpha,highalphamin,highalphamax,0,100);
mySerial.print(highalpha100);
mySerial.write(",");
lowbeta100=map(lowbeta,lowbetamin,lowbetamax,0,100);
mySerial.print(lowbeta100);
mySerial.write(",");
highbeta100=map(highbeta,highbetamin,highbetamax,0,100);
mySerial.print(highbeta100);
mySerial.write(",");
lowgamma100=map(lowgamma,lowgammamin,lowgammamax,0,100);
mySerial.print(lowgamma100);
mySerial.write(",");
highgamma100=map(highgamma,highgammamin,highgammamax,0,100);
mySerial.print(highgamma100);
mySerial.write(",");
mySerial.println("");
delay(200);
}
}

