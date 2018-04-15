#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>


#define TRIGGER_PIN 0               //pin used to reset the default values. This starts the configuration wizard. (must be pressed when the LEDs are blue)
#define PIN 2                       //output pin where the information bus of the neopixels (LED's) is connected
#define NUMPIXELS 8                 //number of LED's connected to the matrix
#define TIEMPO_RESET 2500           //time in milliseconds of the window time to reset the default values.

#define L_Good      0x9600          //Green
#define L_Poor      0x960000        //Red
#define L_Fair      0x969600        //Yellow
#define L_Reset     0x96            //Blue
#define L_Off       0x0             //Off
#define L_Program   0x892496        //Purple

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


//------------------------------------------------------------- GLOBAL VARIABLES -------------------------------------------------------------
char host[20] = "www.hamqsl.com";   //dir 0   default host to which you are going to connect
char Url[20] = "/solarxml.php";     //dir 20  default address of the host where we make the XML request
char Tiempo[10] = "5";              //dir 40  time in seconds by default to turn off the LEDs (if turning off is false, it is not taken into account)
char Apagar[5] = "true";            //dir 50  Sets whether the LEDs will turn off after a period of time
char Brillo[3] = "30";              //dir 55  intensity of illumination of the LEDs between 0 and 255

char Estado[8];                     //variable used to store "G", "F", "P" (Good, Fair, Poor) correlatively as provided by the host.
char brillo = 30;                   //variable converted to interpretable information of the variable Brillo[3] declared previously
int TmpApagado = 5000;              //variable converted to int type multiplied by 1000, time after matched in milliseconds
bool apagar = true;                 //variable converted to Booan type

bool shouldSaveConfig = false;      //Flag to save the data

//------------------------------------------------------------- Call active saving data -------------------------------------------------------------
void saveConfigCallback () {
  shouldSaveConfig = true;
}

//------------------------------------------------------------- Call before entering AP mode -------------------------------------------------------------
void configModeCallback(WiFiManager *myWiFiManager){
  allColor(L_Program);
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

//------------------------------------------------------------- Function to record in the EEPROM -------------------------------------------------------------
void grabar(int addr, char* inchar, int tamano) {
  for (int i = 0; i < tamano; i++) {
    if(inchar[i] == 0) EEPROM.write(addr+i, 255);
    else EEPROM.write(addr+i, inchar[i]);
  }
  EEPROM.commit();
  delay(20);
}

//------------------------------------------------------------- Function to read the EEPROM -------------------------------------------------------------
void leer(int addr, int tm, char* lect) 
{
   byte lectura;
   int e = 0;
   for (int i = addr; i < (addr + tm); i++) 
   {
      lectura = EEPROM.read(i);
      if (lectura != 255) lect[e] = (char)lectura;
      else lect[e] = 0;
      e++;     
   }
}

//------------------------------------------------------------- LOAD DATA FROM THE EEPROM -------------------------------------------------------------
void cargaDatos()
{
  leer(0,20, host);
  leer(20,20, Url);     
  leer(40,10, Tiempo);
  leer(50,5, Apagar);
  leer(55,3, Brillo);

  refrescaDatos();
}

//------------------------------------------------------------- CONVERT THE DATA TO YOUR USED VARIABLES -------------------------------------------------------------
void refrescaDatos()
{  
  String temp="";
  
  for(int i= 0; i<10; i++)  
    if(Tiempo[i] != 0) temp+= Tiempo[i];
  TmpApagado = temp.toInt() * 1000;

  temp="";
  for(int i= 0; i<3; i++)  
    if(Brillo[i] != 0) temp+= Brillo[i];
  brillo = (char)temp.toInt();  

  if(Apagar[0] == 'f') apagar = false;
  else if(Apagar[0] == 'F') apagar = false;
  else apagar = true;
  
  Serial.println();
  Serial.println("Carga de datos:");
  Serial.print("Host------> "); Serial.println(host);
  Serial.print("Url-------> "); Serial.println(Url);
  Serial.print("Time------> "); Serial.println(Tiempo);
  Serial.print("Turn off--> "); Serial.println(Apagar);
  Serial.print("Brightness> "); Serial.println(Brillo);
}

//------------------------------------------------------------- FUNCTION "SPLIT" -------------------------------------------------------------
//Search the propagation states in the text received from the XML consulted on the web
//enter in the 8-byte array the characters 'G', 'F', 'P' according to the GOOD, FAIR or POOR states respectively
bool Split(String texto)
{
  bool retorno = false;
  int puntero = 0;
  for(int i =0; i < 4; i++)
  {
    puntero = texto.indexOf("\"day\">", puntero + 1);
    if(puntero < 0) break;
    Estado[i] = texto[puntero + 6];
    retorno = true;
  }
  puntero = 0;
  for(int i =0; i < 4; i++)
  {
    puntero = texto.indexOf("\"night\">", puntero + 1);
    if(puntero < 0) break;
    Estado[i+4] = texto[puntero + 8];
  }
  return retorno;
}

//------------------------------------------------------------- TRANSLATOR FUNCTION OF COLOR CHARACTERS -------------------------------------------------------------
//traduce los caracteres 'G','F','P' a los colores correspondientes
uint32_t SetColor (char est)
{
  if(est == 'G') return L_Good;      //Green
  if(est == 'F') return L_Fair;      //Yellow
  if(est == 'P') return L_Poor;      //Red
  return pixels.Color(0,0,0);
}

//------------------------------------------------------------- FUNCTION PAINTS ALL LEDS  -------------------------------------------------------------
void allColor(uint32_t _color)
{
  for(int i=0; i< NUMPIXELS; i++)  
    pixels.setPixelColor(i, _color);
  pixels.show();
}

//------------------------------------------------------------- OFF SEQUENCE  -------------------------------------------------------------
void Sec_Apagado()
{
  delay(TmpApagado);              //wait for the scheduled time
        
        for(int i=0; i< 4; i++)         //we made 4 flashes of the leds
        {
          pixels.setBrightness(10);
          pixels.show();
          delay(500);
          pixels.setBrightness(brillo);
          pixels.show();
          delay(500);
          Serial.print("*");
        }        
        allColor(L_Off);            //we turn off all the leds
}

//---------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------- SETUP -------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
void setup() {
  
  Serial.begin(115200);                 //we start serial port to 115200
  Serial.println();

  pinMode(TRIGGER_PIN, INPUT);          //TRIGGER pin as input
  pixels.begin();                       //we started the library of neopixels
  pixels.setBrightness(brillo);         //assigns the brightness intensity of the led's
  allColor(L_Off);                      //turn off all LED's

  EEPROM.begin(256);                    //we started the EEPROM management library

  

  //We create the variables for the new parameters that will be displayed in the configuration web
  WiFiManagerParameter custom_text0("<p><strong> Solar-Terrestrial Data XML Server </p> </strong>");
  WiFiManagerParameter custom_xml_host("host", "host XML", host, 21);
  WiFiManagerParameter custom_xml_url("url", "url XML", Url, 21);
  WiFiManagerParameter custom_text1("<p><strong> Configuration </p> </strong>");
  WiFiManagerParameter custom_Brillo("Brillo", "Brightness 0-255", Brillo, 4);
  WiFiManagerParameter custom_Apagar("Apagar", "Turn off LEDs true or false", Apagar, 6);
  WiFiManagerParameter custom_Tiempo("Tiempo", "Time off in seconds", Tiempo, 11);

  WiFiManager wifiManager;              //we started the library of the Wifi manager

  //We set the call to the data saving function
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //We set the function call before entering AP mode
  wifiManager.setAPCallback(configModeCallback);
  
    
  //We introduce the custom parameters of the web
  wifiManager.addParameter(&custom_text0);  
  wifiManager.addParameter(&custom_xml_host);
  wifiManager.addParameter(&custom_xml_url);
  wifiManager.addParameter(&custom_text1);
  wifiManager.addParameter(&custom_Brillo);
  wifiManager.addParameter(&custom_Apagar);
  wifiManager.addParameter(&custom_Tiempo);


  
  //********************************** WINDOW TO RESET TO FACTORY VALUES *****************
  allColor(L_Reset);                                                     //we put all the led in blue  
  delay(TIEMPO_RESET);                                                  //We wait for the allotted time
  
  if ( digitalRead(TRIGGER_PIN) == LOW ) wifiManager.resetSettings();   //if the button is pressed, we reset default values
  else 
  {
    cargaDatos();                                                       //else we load the values stored in the EEPROM
    pixels.setBrightness(brillo);                                       //we set the brightness of the led's according to the saved configuration
  } 
    
  allColor(L_Off);                                                     //we turn off all the led's
  
  wifiManager.setDebugOutput(false);                                    //DEBUG mode of wifimanager off

  //************************************************END WINDOW ************************************

  //Manager's Loop
  //If you can not connect to the Wi-Fi or if you have performed a reset of "factory" values, create an Access Point with the web configuration
  //dmust redirect automatically, in any case its address is 192.168.4.1
  if (!wifiManager.autoConnect("AP_SolarData")) {
    Serial.println("Connection failure");
    delay(3000);
    //If the connection data is not correct, reset the ESP
    ESP.reset();
    delay(5000);
  }

  //Wifi connected
  Serial.println();
  Serial.println("Connected:");

  //We read the data returned from the configuration page and assign it to its variables
  strcpy(host, custom_xml_host.getValue());
  strcpy(Url, custom_xml_url.getValue());
  strcpy(Brillo, custom_Brillo.getValue());
  strcpy(Apagar, custom_Apagar.getValue());
  strcpy(Tiempo, custom_Tiempo.getValue());

  //We save custom parameters
  if (shouldSaveConfig) {
    Serial.println("Saving configuration");
           
    grabar(0, host, 20);    
    grabar(20, Url, 20);    
    grabar(40, Tiempo, 10);    
    grabar(50, Apagar, 5);    
    grabar(55, Brillo, 3);

    refrescaDatos();
  }

  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());

}



//--------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------- LOOP -----------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
   //Conexion al host ***************************************************
  
    Serial.println(""); 
    Serial.print("Connecting to ");
    Serial.println(host);
    
    WiFiClient client;                              //We create the TCP connection client
    const int httpPort = 80;                        //port to use
    if (!client.connect(host, httpPort)) {          //we try to connect to the host and assigned ports
      Serial.println("Failed connection");
      return;
    }
    
    
    String url(Url);                                //We convert the url of the XML request to string
    Serial.print("Requesting URL: ");
    Serial.println(url);
    
    // We send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" + 
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    
    while (client.available() == 0) {               //we wait for server response
        if (millis() - timeout > 5000) {
        Serial.println(">>> Customer waiting time!");
        client.stop();                              //if the waiting time passes, we will close the connection
        return;
      }
    }
    
    while(client.available()){                      //if we have response from the server
      String line = client.readStringUntil('\r');   //we read the lines sent
      if(Split(line)) 
      { 
        client.stop();                                //We send them to the decoder to obtain the required data
        Serial.print(line);                           //we print the lines by the serial port 
        break;
      }   
    }
    
    Serial.println();
    Serial.println("Closing connection...");

    //We set the colors of the pixels according to the data obtained from the XML
    for(int i=0; i < NUMPIXELS; i++) pixels.setPixelColor(i, SetColor(Estado[i]));
    pixels.show();

    
    if(apagar)  Sec_Apagado();                        //If the LEDs are turned off in the configuration
    
      
    Serial.println("Sleeping ESP...");
    ESP.deepSleep(0);                       //Once the consultation is finished, whether the LEDs go off or not, we pass the ESP in low consumption mode
                                            //just wake up when you reset it
}
