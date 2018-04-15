# ESP8266 SolarData-RGB

![](https://image.ibb.co/cVrU7S/IMG0.jpg)


> [Versión en español aquí..](https://github.com/DonJaume/ESP8266_SolarData_RGB)

It is a simple circuit that shows us on a WS2812B LED strip, by means of colors, the status of the amateur radio HF bands, obtaining said information from the website hamqsl.com.

For control and connection to the Wi-Fi network, the ESP8266 module is used. Used in this circuit the first model ESP-01, which is the simplest and smallest of the series but sufficient for this project.

![](https://image.ibb.co/bzdOHS/Secuencia.gif)
![](https://image.ibb.co/dshHcS/secuencia_reinicio.gif)
[![](https://image.ibb.co/dNiyZ7/Secuencia_config.gif)](https://image.ibb.co/fD1o1n/secuencia_config2.gif)



## Material

- ESP8266 ESP-01 module.
- 8 WS2812 RGB LEDs, neopixel type or similar.
- 2 push buttons (normally open)
- 1 10K resistor.
- 3.3V USB to TTL converter (only for programming)
- 3.3v power supply for final installation.

## Circuit
![](https://image.ibb.co/eKOgMn/circuito.jpg)


![](https://image.ibb.co/ck4tZ7/LED_RGB_f.jpg)
![](https://image.ibb.co/i8Y97S/LED_RGB_t.jpg)


## Programming
1. Install the latest version of [Arduino](https://www.arduino.cc/en/Main/Software).
2. Install the ESP8266 card.
	- Start Arduino and open Preferences window.
	- Enter ```http://arduino.esp8266.com/stable/package_esp8266com_index.json``` into *Additional Board Manager URLs* field. You can add multiple URLs, separating them with commas.
	- Open Boards Manager from Tools > Board menu and install *esp8266* platform (and don't forget to select your ESP8266 board from Tools > Board menu after installation).
	- You can find more information or other installation modes on your [GitHub page.](https://github.com/esp8266/Arduino) 
3. Installation of the library that controls the LED's.
	- From the Program / Include Library menu, open *Manage Libraries*. Search and install the library called **Adafruit NeoPixel** by Adafruit.
4. Install the WifiManager library.
	- From the Program / Include Library menu, open *Manage Libraries*. Search and install the library called **WifiManager** by tzapu.
	- You can find more information or other installation modes on your [GitHub page.](https://github.com/tzapu/WiFiManager)
5. Select the module *Generic ESP8266 Module* for the ESP-01; select the COM port of your USB-TTL converter; in *Upload speed* select 115200.
6. To upload the program to ESP8266 ESP-01, we must first start it in programming mode, for this, following the proposed circuit:
	- Connect the power, Tx and Rx to our USB-TTL **(important that this is 3.3v, both power and data bus)**.
	- Press and hold the reset button "BT2".
	- Press and hold the button 1 "BT1".
	- Release reset button "BT2".
	- Wait 2 seconds and release button 1 "BT1".
	- Upload the program from the Arduino application.


## Operation and start-up
Once the *sketch* is uploaded to our ESP8266 and if everything went well, we should see all the blue LEDs light up for a few seconds, then turn to a purple color.

The purple color means that our circuit has entered AP mode (*"Access Point"*), creating its own Wi-Fi network with the name AP_SolarData.

#### Configure our wifi network and other options.
- Connecting with a tablet, PC, phone or similar to the Wi-Fi network created *AP_SolarData*, is an open network.
- Open a web browser, in case of not redirecting us directly, access the address 192.168.4.1
- On the next screen, click on *Configure WIFI*.

![Inicio](https://image.ibb.co/mxW0CS/IMG1.jpg)

- We will see a page like the following (in English):

![Configuracion](https://image.ibb.co/fdesRn/IMG2.jpg)

- In the list, click on our network and enter the access password.
- The section *Solar-Terrestrial Data Server XML* is the host and the url of access to the XML that serves us the data, should not be modified unless the location of the file changes.
- In the section *Configuration*, we can select some parameters to our liking:
	- The First (30) is the intensity of illumination of the LEDs, we can put a value between 0 and 255.
	- The second allows us to write *true* o *false*
		- **true**: The LEDs will turn off after a few seconds. By pressing the "BT2" a new data request is made to the web and the LEDs are highlighted as appropriate.
		- **false**: The LEDs are kept on. By pressing the "BT2" a new data request is made to the web and the LEDs are highlighted as appropriate.
		- In any case, the module is disconnected from the network and goes to low consumption once the data is loaded.
	- The third section allows us to configure (in case of being true the previous section) the seconds that the LED's will be kept illuminated.
- Click on *save* and if everything went well our ESP should restart, connect to our wifi and display the colors as appropriate. If the lighting of the LEDs is still purple, it means that it has not been able to connect to our Wi-Fi, so it will have entered AP mode to reconfigure it.

![](https://image.ibb.co/e4O2Rn/IMG3.jpg) 
![](https://image.ibb.co/nMbBK7/IMG4.jpg)


#### Change settings
Once our ESP has the saved data and manages to enter our Wi-Fi, it does not return to AP mode, only this mode is activated if it is not able to connect to our Wi-Fi or if we restart the data.

To make a data reset to re-enter configuration mode, you must:

- Restart the ESP with the button 2 "BT2" and release it.
-When the lighting is BLUE, press the "BT1".

Now it should light up in purple. You are ready to reconfigure from the Wi-Fi *AP_SolarData*.

*BLUE illumination is a window of 2.5 seconds that allows us to reset the values to our ESP by pressing the "BT1"*

*This document has been translated by the google translator, sorry for the possible mistranslation.*




