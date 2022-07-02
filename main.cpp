/** Arduino ESP32 Spotify integration v1.0
 
    created by Dani and licensed under GNU/GPL.

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

 */
    
	
// Include libraries
#include <WiFi.h>
#include <HTTPClient.h>
#include <base64.h>
#include "settings.h"
#include <SPI.h>
#include <MFRC522.h>

// Define buttons
#define SS_PIN 21  // SDA (pin 10 @ Arduino Mega)
#define RST_PIN 22 // (RST pin 9 @ Arduino Mega)

MFRC522 mfrc522(SS_PIN, RST_PIN); // RFID reader

// Debugging
bool debug = true; // if set to true some information will be sent to the serial monitor

// Initialize functions
String ParseJson(String key, String json);
int RFID_reader();

void setup()
{

	Serial.begin(115200);		// Serial monitor @ baude rate 115200
	SPI.begin();				// SPI connection
	mfrc522.PCD_Init();			// Initialize RFID
	WiFi.begin(ssid, password); // Start wifi connection

	while (WiFi.status() != WL_CONNECTED && debug)
	{
		delay(1000);
		Serial.println("Connecting to WiFi...");
	}

	if (debug)
	{
		Serial.println((String) "Connected to the WiFi network: " + ssid);
	}
}

void loop()
{
	void play_song(int);

	if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) // looking for new Card - PICC_IsNewCardPresent needs to be called first in order to call PICC_ReadCardSerial
	{
		int code = RFID_reader();

		if (debug)
		{
			Serial.println((String) "The (converted) RFID-Code out of play_song is:" + code);
		}

		play_song(code);
	}
}

int RFID_reader()
{
	// One integer instead of 4 blocks (HEX, DEZ)
	int code = 0;

	for (byte i = 0; i < mfrc522.uid.size; i++)
	{
		code = ((code + mfrc522.uid.uidByte[i]) * 10); // Hack - read each of the four blocks and multiply with 10
	}

	if (debug)
	{
		Serial.println((String) "The (converted) RFID-Code out of RFID_reader is:" + code);
	}

	return code;
}

String ParseJson(String key, String json)
{ // Function has been copied from the makeratplay-project to parse the access token
	String retVal = "";
	int index = json.indexOf(key);

	// Serial.print( "index: " );
	// Serial.println(index);
	if (index > 0)
	{
		bool copy = false;
		for (int i = index; i < json.length(); i++)
		{
			// Serial.print( "charAt: " );
			// Serial.println( json.charAt(i) );
			if (copy)
			{
				if (json.charAt(i) == '"' || json.charAt(i) == ',')
				{
					break;
				}
				else
				{
					retVal += json.charAt(i);
				}
			}
			else if (json.charAt(i) == ':')
			{
				copy = true;
				if (json.charAt(i + 1) == '"')
				{
					i++;
				}
			}
		}
	}
	return retVal;
}

void play_song(int code)
{ // Hack - Information should later be written on cards (context_uri, position, position_ms), such that we don't have to change the code here for each new song

	int position;
	int position_ms;
	String context_uri;

	if (code == batch)
	{
		// Mickey Mouse
		// https://open.spotify.com/album/0mxAVqEADEgJw80UI2CJMQ?si=uBwPqkmEQGm4xyYkQaRNFw
		// pos1 pos_ms2000
		context_uri = "spotify:album:0mxAVqEADEgJw80UI2CJMQ"; // Fix coded URI could be implemented dynamically
		position = 1;
		position_ms = 2000;
	}
	else if (code == card)
	{
		// Sweet Home Alabama
		// https://open.spotify.com/album/54V1ljNtyzAm053oJqi0SH?si=rWVsArLiSfqT3ldRc8yB2g
		// pos0 pos_ms2500
		context_uri = "spotify:album:54V1ljNtyzAm053oJqi0SH"; // Fix coded URI could be implemented dynamically
		position = 0;
		position_ms = 2500;
	}
	else
	{
		// Barfuss am Klavier
		// https://open.spotify.com/album/5Hefroewxjp5s8i4CCmPGP?si=sHUukXzySVqz7WxTv5UCNA
		// pos10 pos_ms0
		context_uri = "spotify:album:5Hefroewxjp5s8i4CCmPGP"; // Fix coded URI could be implemented dynamically
		position = 8;
		position_ms = 0;
	}

	if (debug)
	{
		Serial.println((String) "context_uri" + context_uri + "postiion: " + position + ", postition_ms: " + position_ms);
	}

	// Fetch Token and play song
	if ((WiFi.status() == WL_CONNECTED))
	{ // Check the current connection status

		HTTPClient http;

		String body = "grant_type=refresh_token&refresh_token=" + refreshToken;
		String authorizationRaw = clientId + ":" + clientSecret;
		String authorization = base64::encode(authorizationRaw);

		http.begin("https://accounts.spotify.com/api/token"); // Specify the URL
		http.addHeader("Content-Type", "application/x-www-form-urlencoded");
		http.addHeader("Authorization", "Basic " + authorization);

		int httpCode = http.POST(body); // Make the request

		if (httpCode > 0)
		{ // Check for the returning code

			String payload = http.getString();

			if (httpCode == 200)
			{
				String accessToken = ParseJson("access_token", payload); // Get the acces token out of the JSON response

				if (debug)
				{
					Serial.println((String) "payload: " + payload);
					Serial.println((String) "Got new access token: " + accessToken);
				}

				http.end(); // Free the resources

				// Get device - we have to redo this regularly, because the id changes sometimes (idk why...)
				http.begin("https://api.spotify.com/v1/me/player/devices");
				http.addHeader("Content-Type", "application/json");
				http.addHeader("Authorization", "Bearer " + accessToken);
				http.GET();
				String deviceIdfromGET = http.getString();
				String parsedDeviceId = deviceIdfromGET.substring(32, 72); // Hack - get the deviceId; PAY ATTENTION --> Takes the first device from the JSON
				if (debug)
				{
					Serial.println((String) "deviceIds found: " + deviceIdfromGET);
					Serial.println((String) "deviceId_substring first id: " + deviceIdfromGET.substring(32, 72));
				}
				http.end(); // Free the resources

				// Play song
				String body = "{\"context_uri\":\"" + context_uri + "\",\"offset\":{\"position\":" + position + "},\"position_ms\":" + position_ms + "}"; // Example how to choose a song (and the starting postion within the song) out of an album.
				String url = "https://api.spotify.com/v1/me/player/play?device_id=" + parsedDeviceId;

				// API - call
				http.begin(url);
				// Header
				http.addHeader("Content-Type", "application/x-www-form-urlencoded");
				http.addHeader("Authorization", "Bearer " + accessToken);
				// Body
				http.PUT(body); // after this PUT request Spotify will (our should...) play the given song
			}
			else
			{
				Serial.println("Failed to get new access token");
				Serial.println(httpCode);
				Serial.println(payload);
			}

			if (debug)
			{
				Serial.println((String) "httpCode: " + httpCode);
				Serial.println((String) "payload: " + payload);
			}
		}

		else
		{
			Serial.println("Error on HTTP request");
		}
	}
	else
	{
		Serial.println("Error lost internet connection play_song ");
	}
}