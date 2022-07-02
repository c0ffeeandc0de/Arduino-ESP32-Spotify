/** Project Settings
 * clientId and clientSecret are the "REST-Client" credentials and not the Spotify user's credentials 
 * deviceName is the Name of the Speaker if you want to address the same speaker every time
 * The refreshToken you have to get in advance (Very important - we did it through the REST client insomnia)
 * GET https://accounts.spotify.com/authorize
 * Body: client_id = the ID from our REST-Client
 *          response_type = token
 *          redirect_uri = the uri we defined in the spotify "App-Settings" (Developer Backend)
 *          scope = the services we want the app to have access to. In our example: user-read-playback-state user-modify-playback-state
 * 
 * --> https://accounts.spotify.com/authorize?client_id=21c0b0ea3eeb4b8fbdbfac30a0c0703a&response_type=token&redirect_uri=http%3A%2F%2Fdev2.ddns.net&scope=user-read-playback-state%20user-modify-playback-state
 * 
 * Requesting this URL in a browser will lead to a window where we have to insert our spotify credentials and give access to the defined services. 
 * After giving access we will be redirected to the defined URL and we can extract the code from this URL. This code we will need to get our refresh token. 
 * 
 */


// Wifi settings 
 const char* ssid = "the_name_of_your_wifi_network";
 const char* password =  "your_wifi_password";

// Spotify settings
// String deviceName = "The_speakers_name"; // Not yet used. Could be used to parse out this specific device out of a list of several devices (JSON from REST GET/devices response)
String clientId = "clientId_you_get_from_your_app_within_spotify_developer_account";
String clientSecret = "clientSecret_you_get_from_your_app_within_spotify_developer_account";
String refreshToken = "your_refreshToken";

// RFIDs
// Note: You can also use a credit card for example. The credit card's RFID code will change over time. Doesn't matter for testing purposes (i.e. the credit card is the "else-case")
int batch = 123;
int card = 456;