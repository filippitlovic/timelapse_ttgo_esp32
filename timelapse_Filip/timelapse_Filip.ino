/*TIME LAPSE
 * 
   * Esto es una Demo para probar el funcionamiento del hardware, este sketch carga el logo de inicio "fotoigual.bmp". Después aparece un menú de uso de los
   * dos botones uno hace dos disparos y el otro muestra el voltage de carga / batería y si hacemos una pulsación larga entra el modo deep sleep. 
   * Tocando el mismo botón, provoca el despertar del dispositivo
   * Este sketch es una modificación  de: https://github.com/Xinyuan-LilyGO/TTGO-T-Display/blob/master/TFT_eSPI/examples/FactoryTest/
   * 
   * 
   * Muy importante: Elegir el User_Setup.h que corresponde a "TTGO T Display".  Está en \TFT_eSPI\User_Setups\Setup25_TTGO_T_Display.h.
   * 

*/
#include <TFT_eSPI.h>
#include "Button2.h"
#include "esp_adc_cal.h"
#include "bmp.h"


// TFT Pins has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
// #define TFT_MOSI            19
// #define TFT_SCLK            18
// #define TFT_CS              5
// #define TFT_DC              16
// #define TFT_RST             23
// #define TFT_BL              4   // Display backlight control pin


#define CAM_S 25  //shutter
#define CAM_F 26  //focus

#define ADC_EN              14  //ADC_EN is the ADC detection enable port
#define ADC_PIN             34
#define BUTTON_1            0
#define BUTTON_2            35

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

char buff[512];
int vref = 1100;
int btnClick = false;
const int button_1_pin = 0;
const int button_2_pin = 35;

//screens
int currentScreen = 0; 
int intervalScreen = 1;
int numberOfShotsScreen = 2;
int startScreen = 3;
int runningScreen=4;
int rampScreenInterval=6;
int rampScreenTime=7;

//menus
int runningMenu=5;
int intervalChooseMenu = 9;
int noShotsChooseMenu = 10;
int rampMenu = 11;

//variables
int x=0;
int y=0;
int intervalTime = 0;
int rampTime=0;
int rampInterval=0;
int numberOfShots = 0;
int rampIndexCount = 0;
int rampIntervalTimeSeconds = 0;
int noShotX=50;
int noShotY=70;
int noShotsTens = 0;
int noShotsHundreds=0;
int noShotsOnes=0;
unsigned long pressedTime  = 0;
unsigned long pressedTime2  = 0;
int currentState;
int lastState = 0;
int currentState2;
int lastState2 = 0;
int finalSeconds = 0;
int minutes = 0;
int shotsDone = 0;
int shotsLeft = 0;
int shotsCounter = 1;
int intervalHours=0;
int rampIntervalMinutes=0;
int rampIntervalSeconds=0;
int intervalMinutes=0;
int intervalSeconds=0;
int intervalX=50;
int intervalY = 70;
int menuY = 0;
int intervalXrampTime = 50;
int intervalYrampTime =  70;
int intervalCursorX=50;
int intervalCursorY=70;
int intervalTimeSeconds = 0;
int minutesForInterval = 0;
int hoursInterval = 0;
int hoursLeft = 0;
int minutesLeft = 0;
int secondsLeft = 0;

//flags
int rampOn = 0;
int rampIndexUp = 0;
int rampIndexDown = 0;
int changeTimeIndex = 0;
bool btn1Clicked = false;
bool btn2Clicked = false;
bool btn1LongClicked = false;
bool button1Flag = 0;
bool button2Flag = 0;
int btnFlag=0;
int btnFlag2=0;
int menuFlag=0;
int menuButtonClicked=0;
unsigned long timerStartFlag = 0;
int timerFlag=0;
int btn1LongClickFlag=0;
int flagFirstNumber = 0;
int runningFlag = 0;
int firstButtonClick = 0;

//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms)
{
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

//prikaz voltage
void showVoltage()
{
    static uint64_t timeStamp = 0;
    if (millis() - timeStamp > 1000) {
        timeStamp = millis();
        uint16_t v = analogRead(ADC_PIN);
        float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
        String voltage = "Voltage :" + String(battery_voltage) + "V";
        Serial.println(voltage);
        tft.fillScreen(TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(voltage,  tft.width() / 2, tft.height() / 2 );
    }
}

//initializing buttons
void button_init()
{
    //clicking button2 once is starting the timelapse menu
    btn2.setPressedHandler([](Button2 & b) {
    btn2Clicked = true;
    });
    btn1.setPressedHandler([](Button2 & b) {
      btn1Clicked = true;
    });

    btn2.setLongClickHandler([](Button2 & b) {
    });
}

void button_loop()
{
    btn1.loop();
    btn2.loop();
}

void time_lapse()
{

tft.fillScreen(TFT_BLACK);
tft.setTextDatum(MC_DATUM);

  //setting camera shutter and focus on high
  digitalWrite(CAM_S, HIGH);
  digitalWrite(CAM_F, HIGH);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(30,60);
  tft.print("Timelapse ...");
  delay(2000);
  // //changing cam_s and cam_f to reset them and set
    tft.fillScreen(TFT_BLACK);
    digitalWrite(CAM_S, LOW);
    digitalWrite(CAM_F, LOW);
  delay(200);
    digitalWrite(CAM_S, HIGH);
    digitalWrite(CAM_F, HIGH);
    tft.fillEllipse(tft.width() / 2, tft.height() / 2, 10, 10, TFT_RED );
  delay(2000);
    tft.fillScreen(TFT_BLACK);
    digitalWrite(CAM_S, LOW);
    digitalWrite(CAM_F, LOW);
  delay(200);
  setInterval();
}

void setCursorPosition(){ //// cursor for setting the underline that indicates which number is currently able to be changed
    if(btn1LongClickFlag==1){
      btn1LongClickFlag=0;

      if(currentScreen==intervalScreen || currentScreen==rampScreenInterval){ 
          //if cursor is on 'menu', set to first number (Minutes)
        if(intervalCursorX == 140){
          intervalCursorX = 50;
          intervalCursorY = 70;
        }  
        //set to menu
        else if(intervalCursorX==90){
          intervalCursorX = 140;
          intervalCursorY = 110;
        }else{
          intervalCursorX = intervalCursorX + 40;
        }
        if(currentScreen==intervalScreen){
          setInterval();
        }else if(currentScreen==rampScreenInterval){
          setRampInterval();
        }
      }
      
      if(currentScreen==numberOfShotsScreen){  
        //set to first digit
        if(noShotX == 140){
          noShotX = 50;
          noShotY = 70;
        }  
        //set to menu
        else if(noShotX==90){
          noShotX = 140;
          noShotY = 110;
        }
        else{
          noShotX = noShotX + 20;
        }
        setNumberOfShots();
      }
      else if(currentScreen == rampScreenTime){
        if(intervalXrampTime == 160){
          intervalXrampTime = 50;
          intervalYrampTime = 70;
        }
        else{
          intervalXrampTime = 160;
          intervalYrampTime = 110;
        }
        setRampTime();
      }
    }
}

void openMenu(){
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,menuY);
  tft.print(">");

  tft.setCursor(20,0);
  if(currentScreen==intervalChooseMenu){
    tft.print("Set no of shots");
  }else if(currentScreen==noShotsChooseMenu){
    tft.print("Start");
  }else if(currentScreen==runningMenu){
    tft.print("Ramp interval");
  }else if(currentScreen==rampMenu){
    tft.print("Start ramping");
  }

  tft.setCursor(20,30);
  if(currentScreen==intervalChooseMenu){ 
    tft.print("Reset interval");
  }else if(currentScreen==noShotsChooseMenu){
    tft.print("Reset no shots");
  }else if(currentScreen==runningMenu){
    tft.print("Stop shooting");
  }else if(currentScreen==rampMenu){
    tft.print("Change ramp time");
  }
  
  tft.setCursor(20,60);
  tft.print("Main menu");
  tft.setCursor(20,90);
  tft.print("Go back");


  if(menuButtonClicked){ //ako je kliknut pause 'menu'
    menuButtonClicked = 0;
    if(menuY==0){
      flagFirstNumber=0; //prvi broj da mi ne bude u startu 1 vec da bude 0
      if(currentScreen==intervalChooseMenu){
        setNumberOfShots();  
      }else if(currentScreen == noShotsChooseMenu ){
        openStartScreen();
      }else if(currentScreen == runningMenu){ //set ramp interval
        setRampTime();  
      }else if(currentScreen == rampMenu){ 
        runRamping();
        openRunningScreen();  
      }
    }else if(menuY==30){
      if(currentScreen==intervalChooseMenu){ //reset interval
        flagFirstNumber==0;
        intervalMinutes=0;
        intervalSeconds=0;
        intervalX = 50;
        intervalY = 70;
        setInterval(); 
      }else if(currentScreen == noShotsChooseMenu){ //reset number of shots
        flagFirstNumber==0;
        noShotsHundreds=0;
        noShotsTens=0;
        noShotsOnes=0;
        setNumberOfShots();
      }else if(currentScreen == rampMenu){ 
        setRampTime();  
      }
    }
    else if(menuY==60){ //main menu
     time_lapse(); //ovdje ide vjj nesta novo, da me odvede na main menu
    }else if(menuY==90){ //go back
      if(currentScreen==intervalChooseMenu){
        intervalX = 50;
        intervalY = 70;
        setInterval();  
      }else if(currentScreen == noShotsChooseMenu){
        setNumberOfShots();
      }else if(currentScreen == runningMenu){
        openRunningScreen();
      }else if(currentScreen == rampMenu){
        flagFirstNumber==0;
        rampIntervalMinutes = 0;
        rampIntervalSeconds = 0;
        intervalX = 50;
        intervalY = 70; 
        setRampInterval();
      }
    }  
  }
}

void openIntervalMenu(){
  currentScreen=intervalChooseMenu;
  openMenu();
}

void updateMenuCursor(){ //scrolling menu
  if(menuFlag==1){ //pressed button for going down 
    if(menuY==90){
      menuY=0;
    }
    else{
      menuY += 30;
    }
  }
  openMenu();
}


///////////////////////////////////////////////////////////NUMBER OF SHOTS SCREEN

void setNumberOfShots()
{
    tft.fillScreen(TFT_BLACK);
    currentScreen=numberOfShotsScreen;
    tft.setCursor(20,5);
    tft.print("Set noShots");
    tft.setTextSize(2);
    tft.setCursor(50,50);
    tft.print(String(noShotsHundreds));
    tft.setCursor(70,50);
    tft.print(String(noShotsTens));
    tft.setCursor(90,50);
    tft.print(String(noShotsOnes));
    tft.setCursor(noShotX,noShotY);
    tft.print("-");
    tft.setCursor(160,110);
    tft.print("next>");

    //increase
    if(button2Flag==1){
      button2Flag=0;
      if(noShotX==50){
        noShotsHundreds++;
        if(flagFirstNumber==0){
          noShotsHundreds=0;
          flagFirstNumber=1;
        }
        setNumberOfShots();
      }
      if(noShotX==70){
        noShotsTens++;
        setNumberOfShots();
      }
      if(noShotX==90){
        noShotsOnes++;
        setNumberOfShots();
      }
    }
    //decrease
    if(button1Flag==1){
      button1Flag=0;
      if(noShotX==50 && noShotsHundreds > 0){
        noShotsHundreds--;
        setNumberOfShots();
      }
      if(noShotX==70 && noShotsTens > 0){
        noShotsTens--;
        setNumberOfShots();
      }
      if(noShotX==90 && noShotsOnes > 0){
        noShotsOnes--;
        setNumberOfShots();
      }
      //next
      if(noShotX==140){
        noShotX = 50;
        noShotY = 70;
        openNoShotsMenu();
      }
    }
  setCursorPosition();
}

void openNoShotsMenu(){
  currentScreen=noShotsChooseMenu;
  numberOfShots = (noShotsHundreds*100)+(noShotsTens*10)+(noShotsOnes);
  openMenu();
}

/////////////////////////////////////////////////////////////START SCREEN
void openStartScreen(){
  currentScreen = startScreen;
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0,0);
    tft.print("Interval: " + String(intervalMinutes) + "m " + String(intervalSeconds) + "s");
    tft.setCursor(0,30);
    tft.print("No shots: " + String(numberOfShots));

    intervalTimeSeconds = ((intervalMinutes*60)+(intervalSeconds));
    hoursInterval = ((intervalTimeSeconds * numberOfShots) - intervalTimeSeconds) / 3600;
    minutesForInterval = ((((intervalTimeSeconds * numberOfShots)-intervalTimeSeconds) - (3600*hoursInterval))) / 60;
    finalSeconds = ((((intervalTimeSeconds * numberOfShots)-intervalTimeSeconds) - (3600*hoursInterval)))  % 60;

    while(finalSeconds >= 60){
      finalSeconds = finalSeconds - 60;
      minutesForInterval++;
    }

    while(minutesForInterval >= 60){
      minutesForInterval = minutesForInterval - 60;
      hoursInterval++;
    }
    if(finalSeconds==0){
      minutesForInterval=minutesForInterval-1;
      finalSeconds=59;
    }
    hoursLeft=intervalHours;
    minutesLeft=intervalMinutes;
    secondsLeft=intervalSeconds;

    tft.setCursor(0,50);
    tft.print("Time:     "+ String(hoursInterval) + "h " + String(minutesForInterval) + "m " +  String(finalSeconds)  + "s ");

    tft.setCursor(150,110);
    tft.print("Start>");   
}

void startingTimelapse(){
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(65,20);
  tft.print("TIMELAPSE");
  tft.setCursor(115,80);
  tft.print("3");
  delay(1000);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(65,20);
  tft.print("TIMELAPSE");
  tft.setCursor(115,80);
  tft.print("2");
  delay(1000);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(65,20);
  tft.print("TIMELAPSE");
  tft.setCursor(115,80);
  tft.print("1");
  delay(1000);
  openRunningScreen();
}

void openRunningScreen(){
  menuButtonClicked=0;
  currentScreen=runningScreen;
  runningFlag=1;
  
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,0);
  tft.print("Interval: " + String(intervalMinutes) + "m " + String(intervalSeconds) + "s");
  tft.setCursor(0,30);
  tft.print("Time: " + String(hoursInterval) + "h " + String(minutesForInterval) + "m " + String(finalSeconds) + "s");
  tft.setCursor(0,50);
  tft.print("Shots done: " + String(shotsDone));
  tft.setCursor(0,70);
  tft.print("Shots left: " + String(numberOfShots));
  tft.setCursor(0,90);
  tft.print("Next shot in:" + String(minutesLeft) + "m " + String(secondsLeft) + "s");
  
  tft.setCursor(150,110);
  tft.print("Menu>");   
    if(rampOn==1){
    tft.setTextColor(TFT_RED);
    tft.setCursor(0,110);
    tft.print("*ramping*");
    tft.setTextColor(TFT_WHITE);
  }
}
void runRunningMenu(){
  runningFlag=1;
  runningFunction();
}

void runningFunction(){
checkTimeUntilNextShot();  

if(rampOn==1){
  rampIndexCount++;
  if(rampTime >0){
    rampTime--;
  }
  else{
    rampOn=0;
  }
  if(rampIndexDown == rampIndexCount){
    if(intervalSeconds != 0){
      intervalSeconds--;
    }
    else{
      intervalMinutes--;
      intervalSeconds = 59;
    }
    rampIndexCount = 0;
    changeTimeIndex = 1; 
  }
  else if(rampIndexUp == rampIndexCount){
    if(intervalSeconds == 59){
      intervalMinutes++;
      intervalSeconds = 0;
    }
    else{
      intervalSeconds++;
    }
    rampIndexCount = 0;
    changeTimeIndex = 1; 
  }
}  

if(finalSeconds >= 0){
  if(shotsDone == 0){
      shotsDone++;
      releaseCamera();
      numberOfShots--;
  }
}
//counting seconds down
if(finalSeconds>=1){
  if(finalSeconds==1 && minutesForInterval != 0){ 
    if(minutesForInterval > 0){
        minutesForInterval--;
        finalSeconds = 59;
        if(shotsCounter == intervalTimeSeconds){
          shotsCounter = 0; 
          numberOfShots--;
          shotsDone++;
          releaseCamera();
          if(changeTimeIndex==1){
            changeTimeIndex = 0;
            changeRunningTime();
            checkTimeUntilNextShot();
          }
        }
    }
    shotsCounter++;
    if(currentScreen == runningScreen){
      updateRunningScreen();
    }
  }
  else{
    finalSeconds--;
      if(shotsCounter == intervalTimeSeconds){
        secondsLeft=0;
        shotsCounter = 0; 
        numberOfShots--;
        shotsDone++;
        releaseCamera();
        if(changeTimeIndex==1){
          changeTimeIndex = 0;
          changeRunningTime();
          checkTimeUntilNextShot();
        }
      }
    shotsCounter++;
    if(currentScreen == runningScreen){
       updateRunningScreen();
    }     
  }
} 
   else if(hoursInterval != 0 && minutesForInterval == 0){
      hoursInterval--;
      minutesForInterval = 59;
      finalSeconds = 59;
      if(currentScreen == runningScreen){
        updateRunningScreen();
      }
   }
    //last shot
  else if(hoursInterval == 0 && minutesForInterval==0 && finalSeconds == 0 ){
      if(shotsCounter == intervalTimeSeconds ){
        secondsLeft=0;
        shotsCounter = 0; 
        numberOfShots--;
        shotsDone++;
        releaseCamera();
        }
      if(currentScreen == runningScreen){
        updateRunningScreen();
      }
      runningFlag = 0;
    }
}

void changeRunningTime(){
  intervalTimeSeconds = ((intervalMinutes*60)+(intervalSeconds));
  hoursInterval = ((intervalTimeSeconds * numberOfShots)) / 3600;
  minutesForInterval = ((((intervalTimeSeconds * numberOfShots)) - (3600*hoursInterval))) / 60;
  finalSeconds = (((((intervalTimeSeconds * numberOfShots)) - (3600*hoursInterval)))  % 60)+1;
}

void checkTimeUntilNextShot(){
  if(hoursLeft==0 && minutesLeft==0 && secondsLeft==0){
    hoursLeft=intervalHours;
    minutesLeft=intervalMinutes;
    secondsLeft=intervalSeconds;
  }
  if(secondsLeft==0){
    if(minutesLeft==0 && hoursLeft != 0){
      hoursLeft--;
      secondsLeft = 59;
      minutesLeft= 59;
    }
    else if(minutesLeft != 0){
      minutesLeft--;
      secondsLeft=59;
    }
  }
  else if(secondsLeft>0){
    secondsLeft--;
  }
}

void startOneSecondTimer() {
  timerStartFlag = millis();
  timerFlag=0;
}
float isOneSecondElapsed() {
  return millis() - timerStartFlag;
}

void checkTimer(){
  if(timerStartFlag==0){
    startOneSecondTimer();
  }
  float tims = isOneSecondElapsed();
  if(tims >= 1000){
    digitalWrite(CAM_S,LOW);
    //digitalWrite(CAM_F,LOW);
    timerFlag=1;
    startOneSecondTimer();
    runRunningMenu();
  }
}

void updateRunningScreen(){
  currentScreen=runningScreen;
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0,0);
  tft.print("Interval: " + String(intervalMinutes) + "m " + String(intervalSeconds) + "s");

  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0,30);
  tft.print("Time: " + String(hoursInterval) + "h " + String(minutesForInterval) + "m " + String(finalSeconds) + "s");

  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0,50);
  tft.print("Shots done: " + String(shotsDone));

  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0,70);
  tft.print("Shots left: " + String(numberOfShots));

  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0,90);
  tft.print("Next shot in:" + String(minutesLeft) + "m " + String(secondsLeft) + "s");

  tft.setCursor(150,110);
  tft.print("Pause>");
  if(rampOn==1){
    tft.setTextColor(TFT_RED);
    tft.setCursor(0,110);
    tft.print("*ramping*");
    tft.setTextColor(TFT_WHITE);
  }

  loop(); 
  openRunningScreen();
}

void releaseCamera(){
  Serial.println("Camera released! Taking a shot...");
  //triger shot logic

  //digitalWrite(CAM_F,HIGH);
  digitalWrite(CAM_S,HIGH);
  //delay(200);
  //digitalWrite(CAM_S,LOW); //set to low to reset state back 
  //napravit dio da ocitava exposure kamere ako je moguce
  //jer kad se mijenja exposure onda se interval u timelapsu ne izvrsava kako spada
}

void pauseMenu(){
  currentScreen = runningMenu;
  openMenu();
}

////////////////////////////////////RAMPING
void setRampTime(){
  tft.fillScreen(TFT_BLACK);
    currentScreen=rampScreenTime; 
    tft.setCursor(0,0);
    tft.print("Ramp time in min");
    tft.setCursor(50,50);
    tft.print(rampTime);
    tft.setCursor(180,110);
    tft.print("next>");
    tft.setCursor(intervalXrampTime,intervalYrampTime);
    tft.print("-");

    menuButtonClicked=0;

    if(button2Flag==1){
      button2Flag = 0;
      rampTime += 1;
      setRampTime();
    }

    if(button1Flag==1){
      button1Flag=0;
      if(intervalXrampTime == 160){
        setRampInterval();
      }else if(rampTime > 0){
      rampTime--;
      setRampTime();
      }
    }
    setCursorPosition();
}

void setInterval()
{
  currentScreen=intervalScreen;
  intervalsScreen(intervalMinutes,intervalSeconds);
}

void setRampInterval()
{
  currentScreen=rampScreenInterval;  
  intervalsScreen(rampIntervalMinutes,rampIntervalSeconds);
}

void intervalsScreen(int &x, int &y){
    tft.fillScreen(TFT_BLACK); 
    tft.setCursor(20,5);
    if(currentScreen == rampScreenInterval){
          tft.print("Set ramp interval");
    }else if(currentScreen == intervalScreen){
          tft.print("Set interval");
    }
    tft.setTextSize(2);
    tft.setCursor(50,50);
    tft.print(String(x) + "m    ");
    tft.setCursor(90,50);
    tft.print(String(y) + "s    ");
    tft.setCursor(intervalCursorX,intervalCursorY);
    tft.print("-");
    tft.setCursor(160,110);
    tft.print("next>");

      //increase
    if(button2Flag==1){
      button2Flag=0;
      if(intervalCursorX==50){
        x++; 
      Serial.println(String(x));
        if(flagFirstNumber==0){
          x=0;
          flagFirstNumber=1;
        }
        if(currentScreen == rampScreenInterval){
          setRampInterval();
        }else if(currentScreen == intervalScreen){
          setInterval();
        }
      }
      if(intervalCursorX==90){
        y++;
        if(currentScreen == rampScreenInterval){
          setRampInterval();
        }else if(currentScreen == intervalScreen){
          setInterval();
        }
      }
    }
    //decrease
    if(button1Flag==1){
      button1Flag=0;
      if(intervalCursorX==50 && x > 0){
        x--;
        if(currentScreen == rampScreenInterval){
         setRampInterval();
        }else if(currentScreen == intervalScreen){
         setInterval();
        }
      }
      if(intervalCursorX==90 && y > 0){
        y--;
        if(currentScreen == rampScreenInterval){
          setRampInterval();
        }else if(currentScreen == intervalScreen){
          setInterval();
        }
      }
      //next
      if(intervalCursorX==140){
        if(currentScreen == rampScreenInterval){
          openRampMenu();
        }else if(currentScreen == intervalScreen){
          intervalCursorX = 50;
          intervalCursorY = 70;
          openIntervalMenu();
        }  
      }
    }
  setCursorPosition();
}

void openRampMenu(){
  currentScreen = rampMenu;
  openMenu();
}

void runRamping(){
    rampOn=1;
    rampTime=rampTime*60;
    rampIntervalTimeSeconds = (rampIntervalMinutes*60)+rampIntervalSeconds;
  if(intervalTimeSeconds<rampIntervalTimeSeconds){
    rampIndexUp = rampTime/(rampIntervalTimeSeconds - intervalTimeSeconds);
  }
  else{
    rampIndexDown = rampTime/(intervalTimeSeconds - rampIntervalTimeSeconds);
  }    
}

void setup()
{
    //baud rate
    Serial.begin(115200);
    //print screen at the start
    Serial.println("Start");

    /*
    ADC_EN is the ADC detection enable port
    If the USB port is used for power supply, it is turned on by default.
    If it is powered by battery, it needs to be set to high level
    */

    //set the pins
    pinMode(CAM_S, OUTPUT);
    pinMode(CAM_F, OUTPUT);
    pinMode(ADC_EN, OUTPUT);
    digitalWrite(ADC_EN, HIGH);

    //initializing arduino tft lcd display
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLUE);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(0, 0);
    tft.setTextDatum(MC_DATUM);
   

    //for setting the image
    tft.setSwapBytes(true);
    tft.pushImage(0, 0,  240, 135, ttgo);
    espDelay(3000);


    button_init();

    //setting ADC / to convert from analog to digital
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);    //Check type of calibration value used to characterize ADC
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
        vref = adc_chars.vref;
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        Serial.printf("Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);
    } else {
        Serial.println("Default Vref: 1100mV");
    }


    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);

  //  tft.setFreeFont(&Font16);
    // //drawing on the display what does every button do

    tft.setCursor(85,10);
    tft.print("Timelapse ->");

    tft.setCursor(85,110);
    tft.print("  Voltage ->");

    // tft.drawString("Top Button:", tft.width() / 2, tft.height() / 2 - 32);
    // tft.drawString("Time Lapse", tft.width() / 2, tft.height() / 2 -16 );

    // tft.drawString("Bottom Button:", tft.width() / 2, tft.height() / 2 );
    // tft.drawString("[Voltage Monitor]", tft.width() / 2, tft.height() / 2 + 16 );

    // tft.drawString("Bottom ButtonLongPress:", tft.width() / 2, tft.height() / 2 + 32);
    // tft.drawString("[Deep Sleep]", tft.width() / 2, tft.height() / 2 + 48 );
    

    tft.setTextDatum(TL_DATUM);
}


void loop()
{

  if(runningFlag==1){
     checkTimer();
  }
    if (btnClick) {
        showVoltage();
    }
    button_loop();
///////////////////////////////////////////////////////////////////////BUTTON 1, DOWN BUTTON
  currentState = digitalRead(button_1_pin);

  if(btn1Clicked){
    btnFlag = 1;
  }

  if (lastState == 1 && currentState == 0) { // button is pressed
    pressedTime = millis();
  } else if (lastState == 0 && currentState == 1 && btnFlag == 1) { // button is released
      unsigned long releasedTime  = 0;
      releasedTime = millis();
      btnFlag = 0;
      long pressDuration = releasedTime - pressedTime;
    if (pressDuration > 500) {
      if(currentScreen == intervalScreen){ 
        btn1LongClickFlag=1;
        setInterval();
      }
      else if(currentScreen == rampScreenTime){ 
        btn1LongClickFlag=1;
        setRampTime();
      }
      else if(currentScreen == rampScreenInterval){
        btn1LongClickFlag=1;
        setRampInterval();
      }
      else if(currentScreen == numberOfShotsScreen){
        btn1LongClickFlag=1; 
        setNumberOfShots();
      } 
    }else if (pressDuration <= 500) {
        button1Flag = 1;
        if(currentScreen == intervalScreen){ 
          setInterval();
        }
        else if(currentScreen == rampScreenTime){
          button1Flag=1; 
          setRampTime();
        }
        else if(currentScreen == rampScreenInterval){ 
          button1Flag=1;
          setRampInterval();
        }
        else if(currentScreen==startScreen) { 
          delay(500);
          startingTimelapse();
        }
        else if(currentScreen == numberOfShotsScreen){ 
          button1Flag=1;
          setNumberOfShots();
        }
        else if(currentScreen == runningScreen){
          pauseMenu();
        }
        else if(currentScreen == runningMenu){
          menuFlag=1;
          updateMenuCursor();
        }else if(currentScreen == intervalChooseMenu){
          menuFlag=1;
          updateMenuCursor();
        }
        else if(currentScreen == noShotsChooseMenu){
          menuFlag=1;
          updateMenuCursor();
        }
        else if(currentScreen == rampMenu){
          menuFlag=1;
          updateMenuCursor();
        }
      }
    }

  // save the last state
  lastState = currentState;

  ////////////////////////////////////////////////////// BUTTON 2, UP BUTTON
  currentState2 = digitalRead(button_2_pin);

  if(btn2Clicked){
    btnFlag2 = 1;
  }

  if (lastState2 == 1 && currentState2 == 0) { // button is pressed
    pressedTime2 = millis();
    //for fast increasing interval while holding button
      if(currentScreen==numberOfShotsScreen){
         while(currentState2==0){
            int elapsedTime = millis()-pressedTime2;
            if(elapsedTime >= 500){
              delay(200);
                  numberOfShots++;
                  setNumberOfShots();
              }
            currentState2=digitalRead(button_2_pin);
          }
      }
      if(currentScreen==rampScreenTime){
         while(currentState2==0){
            int elapsedTime = millis()-pressedTime2;
            if(elapsedTime >= 500){
              delay(200);
                  rampTime++;
                  setRampTime();
              }
            currentState2=digitalRead(button_2_pin);
          }
      }
      if(currentScreen==intervalScreen){
        while(currentState2==0){
            int elapsedTime = millis()-pressedTime2;
            if(elapsedTime >= 500){
              delay(200);
                    if(intervalCursorX==50){
                      intervalMinutes++;
                      setInterval();
                    }
                    else if(intervalCursorX==90){
                      intervalSeconds++;
                      setInterval();
                    }
              }
            currentState2=digitalRead(button_2_pin);
          }
     }
      if(currentScreen==rampScreenInterval){
        while(currentState2==0){
            int elapsedTime = millis()-pressedTime2;
            if(elapsedTime >= 500){
              delay(200);
                    if(intervalCursorX==50){ 
                      rampIntervalMinutes++;
                      setRampInterval();
                    }
                    else if(intervalCursorX==90){
                      rampIntervalSeconds++;
                      setRampInterval();
                    }
            }
            currentState2=digitalRead(button_2_pin);
        }
    }

    lastState2 = 0;
  } else if (lastState2 == 0 && currentState2 == 1 && btnFlag2 == 1) { // button is released
      
      unsigned long releasedTime  = 0;
      releasedTime = millis();
      btnFlag = 0;
      long pressDuration = releasedTime - pressedTime2;
      if(firstButtonClick==0){
        pressDuration =1;
        firstButtonClick = 1;
      }
    if (pressDuration > 500) {
      if(currentScreen==runningMenu){
        menuButtonClicked=1;
        pauseMenu();
      }
    } else if (pressDuration <= 500) { //// short pressed button 2 (button up)
        button2Flag = 1;
        Serial.println("A short press is detected");
        if(currentScreen == intervalScreen){ 
          setInterval();
        }
        else if(currentScreen == rampScreenTime){ 
          setRampTime();
        }
        else if(currentScreen == rampScreenInterval){ 
          setRampInterval();
        }
        else if(currentScreen==0){
          time_lapse(); 
          }
        else if(currentScreen == numberOfShotsScreen){
          menuFlag=2; 
          setNumberOfShots();
        }
        else if(currentScreen == runningMenu){
         menuButtonClicked=1;
          pauseMenu();
        }
        else if(currentScreen == intervalChooseMenu){
          menuButtonClicked=1;
          openIntervalMenu();
        }
        else if(currentScreen == noShotsChooseMenu){
          menuButtonClicked=1;
          openNoShotsMenu();
        }
        else if(currentScreen == rampMenu){
          menuButtonClicked=1;
          openRampMenu();
        }
      }
      lastState2 = 1;
    }
}
