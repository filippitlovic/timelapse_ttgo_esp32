  /*TIME LAPSE
   * NOTA: esnecesario grabar los iconos que estan en la carpeta \data. para ello es necesario usar "ESP32 Sketch Data Upload". Sólo funciona con IDE antiguo versiones 
   * anteriores a 2.0.
   * Muy importante también: Elegir el User_Setup.h que corresponde a "TTGO T Display".  Está en \TFT_eSPI\User_Setups\Setup25_TTGO_T_Display.h.
   * 






*/
#include "Pangodream_18650_CL.h"
#include "SPIFFS.h" 
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
TFT_eSPI tft = TFT_eSPI();

#define ICON_WIDTH 50
#define ICON_HEIGHT 26
#define STATUS_HEIGHT_BAR ICON_HEIGHT
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define ICON_POS_X (tft.width() - ICON_WIDTH)

#define MIN_USB_VOL 4.8 //(originalmente en 4.9 pero la experiencia es que pocas fuentes consiguen este valor)
#define ADC_PIN 34
#define CONV_FACTOR 1.8
#define READS 20
#define BUTTON1PIN 35
#define BUTTON2PIN 0
#define CAM1 25
#define CAM2 26
#define CAM3 27 //en un futuro  #define CAM3 18
#define CAM4 19

int opcion = 1;


Pangodream_18650_CL BL(ADC_PIN, CONV_FACTOR, READS);
char *batteryImages[] = {"/battery_01.jpg", "/battery_02.jpg", "/battery_03.jpg", "/battery_04.jpg", "/battery_05.jpg"};
                    
void setup() {
  Serial.begin(115200);
  pinoutInit();
  SPIFFSInit();
  displayInit();
  menu(1);
  delay(200);
  xTaskCreate(battery_info, "battery_info", 2048, NULL, 1, NULL);
  pinMode(BUTTON1PIN, INPUT);
  pinMode(BUTTON2PIN, INPUT);
  pinMode(CAM1, OUTPUT);
  pinMode(CAM2, OUTPUT);
  pinMode(CAM3, OUTPUT);
      
  }

void loop() {
  if(digitalRead(35) == LOW){
  opcion++;
    if(opcion == 4)  opcion = 1;
  menu(opcion);
  delay(100);
  switch (opcion){
   case 1: 
   digitalWrite(CAM3, HIGH);
    delay(100);
    digitalWrite(CAM3, LOW);
   break;
   case 2: 
   digitalWrite(CAM4, HIGH);
    delay(100);
    digitalWrite(CAM4, LOW);
   break;
   case 3:
    digitalWrite(CAM1, HIGH);
    digitalWrite(CAM2, HIGH);
    delay(1000);
    digitalWrite(CAM1, LOW);
    digitalWrite(CAM2, LOW);
   break;
  }
   }
  delay(200);
}  


void menu(int seleccion){
   //tft.clearScreen();  //Limpiar pantalla
tft.fillScreen(TFT_BLUE);
tft.setCursor(10, 0);
//Rectangulo y su posición
int posicion[4] = {0, 27, 61, 97};
tft.drawRect(6, posicion[seleccion], 233, 35, TFT_RED);
//Opciones del menu
//tft.setTextColor(TFT_WHITE);
tft.setCursor(10, 34,4);
tft.println("* LRTimelapse.com");
tft.setCursor(10, 67,4);
tft.println("* Pro-Timer  0.87");
tft.setCursor(110, 104,4);
tft.println("Con. --->");
}
void pinoutInit(){
  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);
}

void SPIFFSInit(){
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");
}

void displayInit(){
  tft.begin();
  tft.setRotation(1);
  tft.setTextColor(TFT_WHITE,TFT_BLUE); 
  tft.fillScreen(TFT_BLUE);
  tft.setSwapBytes(true);
  tft.setTextFont(2);
  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(tft_output);
  TJpgDec.drawFsJpg(0, 0, "/240_135.jpg");
   delay(3000);
 
  }
 



void battery_info(void *arg)
{
  while (1) {
    //tft.setCursor (0, STATUS_HEIGHT_BAR);
    //tft.println("");
    //tft.print("Average value from pin: ");
    //tft.println(BL.pinRead());
    //tft.print("Volts: ");
    //tft.println(BL.getBatteryVolts());
    //tft.print("Charge level: ");
    //tft.println(BL.getBatteryChargeLevel());
    
    if(BL.getBatteryVolts() >= MIN_USB_VOL){
      for(int i=0; i< ARRAY_SIZE(batteryImages); i++){
        drawingBatteryIcon(batteryImages[i]);
        drawingText("Chrg");
        vTaskDelay(500);
      }
    }else{
        int imgNum = 0;
        int batteryLevel = BL.getBatteryChargeLevel();
        if(batteryLevel >=80){
          imgNum = 3;
        }else if(batteryLevel < 80 && batteryLevel >= 50 ){
          imgNum = 2;
        }else if(batteryLevel < 50 && batteryLevel >= 20 ){
          imgNum = 1;
        }else if(batteryLevel < 20 ){
          imgNum = 0;
        }  
    
        drawingBatteryIcon(batteryImages[imgNum]);    
        drawingText(String(batteryLevel) + "%");
        vTaskDelay(1000);
    }
    // Serial.print("Never Used Stack Size: ");//tft.print
   //  Serial.println(uxTaskGetStackHighWaterMark(NULL));//tft.print
    }  
}

void drawingBatteryIcon(String filePath){
   TJpgDec.drawFsJpg(ICON_POS_X, 0, filePath);
}

void drawingText(String text){
  tft.fillRect(0, 0, ICON_POS_X, ICON_HEIGHT,TFT_BLUE);
  tft.setTextDatum(5);
  tft.drawString(text, ICON_POS_X-2, STATUS_HEIGHT_BAR/2, 4);
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}
