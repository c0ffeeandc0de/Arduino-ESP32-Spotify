# Arduino ESP32 Spotify integration

Version 1.0 which is basically a proof of concept. 

In order to plug and play the main.cpp file it is necessary to write down your credentials to the settings.h file (more information you'll find within settings.h). Also you need to open a spotify developer account and create an app with this account. 
A spotify client needs to be running on the device you're referring to in your api call. Otherwise it won't play anything.

For best debugging experience set the variable **debug** in the main.cpp file to **true**. This will send some information to the serial monitor. 

Hardware parts needed are:
- ESP32 module
- RFID reader RC522
- RFID cards, stickers, batches... (you can also try with your contactless credit card)
- Some jumper wires, a breadboard and or a soldering iron

# Change Log

## Version 1.0 (02.07.2022)

- Spotify API calls through RFID codes -> Hold your rfid card to the reader and your spotify client will play some music