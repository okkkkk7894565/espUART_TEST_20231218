#include "GetPower.h"
#include "main.h"

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;

unsigned long sendDataPrevMillis = 0;
unsigned long getButtonData = 0;
bool signupOK = false;

// //define values
// #define LED_PIN1 D1
// float volt;
// float ampe;
// float PF;
// float wat;
// float Frequency;
// float Energy;
// int firstStatus, ledss;
// String data1, data2;
// String id = "minhndhe163891";
// String 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("ESPreceiver data");
  Serial.println();
  delay(100);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to Wi-Fi: ");
    for (int j = 0; j <= 15; j++) {
      Serial.print(".");
      delay(300);
    }
    Serial.println("");
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  // Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  firstStatus = 0;
  pinMode(LED_PIN1, OUTPUT);  // Initialize the LED_BUILTIN pin as an output
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - getButtonData > 1000 || getButtonData == 0)) {

    getButtonData = millis();
    if (firstStatus == 1) {
      if (Firebase.RTDB.setInt(&fbdo, "/METTER/Change", 0)) {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      } else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
      firstStatus = 0;
    }

    if (Firebase.RTDB.getInt(&fbdo, "/METTER/Change")) {
      if (fbdo.dataType() == "int") {
        ledss = fbdo.intData();
        //chang led status
        digitalWrite(LED_PIN1, ledss);
      } else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
    }

    // if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 3000 || sendDataPrevMillis == 0)) {
    if (millis() - sendDataPrevMillis > 3000) {
      sendDataPrevMillis = millis();
      //lấy giá trị sau khi đọc xong

      volt = getVol();
      ampe = getAmp();
      PF = getPF();
      wat = getWat();
      Frequency = getFre();
      Energy = getEnergy();

      showData(volt,  ampe,  PF,  wat,  Frequency,  Energy);

      json.set("Vol", volt/1.0);
      json.set("ampe", ampe);
      json.set("PF", PF);
      json.set("wat", wat);
      json.set("Frequency", Frequency);
      json.set("Energy", Energy);
      Path = "METTER/" + id + "/Data";

        if (Firebase.RTDB.setJSON(&fbdo, Path, &json)) {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbdo.dataPath());
        Serial.println("TYPE: " + fbdo.dataType());
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
      }
    }
  }
}