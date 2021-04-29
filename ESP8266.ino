#include <Ethernet.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
extern "C" {
#include "libb64/cdecode.h"
}

const char* ssid = "<wifi ssid>";
const char* password = "<wifi password>";

const char* awsEndpoint = "xxxxxxxx.iot.us-east-1.amazonaws.com";

const String certificatePemCrt = \
//-----BEGIN CERTIFICATE-----
"MIIDWjCCAkKgAwIBAgIVAJNJ7h0KGtZnuA/H8dfDiabKJGqKMA0GCSqGSIb3DQEB" \
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t" \
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMTA0MjkwNDA3" \
"NTVaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh" \
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDwHESKXOUghiLgwff9" \
"M7FXW4qvOOAZxUqnV9o8XXhg+x1Sw/IWc2R5a42yA8DltHdqlVH9BqeSTMs/RNXv" \
"xKt1zY/LUEMlTbQAZZB9l9vcH3LBJLZPrzcPIAp4r2+09RmQYwaj85lrBTd6TYfy" \
"y40pfWdE/oI/Pz9CEzAlIDHIN4LtO/Aj8aBSPg3xeRKQEOjxU0xybi3JwMFMS5r2" \
"m4Y5AgMBAAGjYDBeMB8GA1UdIwQYMBaAFMNt4RMV9Y8xpHntUCR7gSYr/Q7QMB0G" \
"A1UdDgQWBBQpKXG4PcUafRZne/5C7Eq7v87yAjAMBgNVHRMBAf8EAjAAMA4GA1Ud" \
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAZSi4fT/6EcMcz6fNhIEWhW+S" \
"gvLNDK3Zl2bevRwbpg8QLj/SR9RZpdG+STct7kMOwgmB0UrqY+Sdhc4b+WafjWjq" \
"Pbno8geBWzBavzWxMS8h2CRnOKDMHjclywchKoqvHjr5R++mIsJ9JsICmpxKmbKv" \
"w9HXfo7LDtImvz8HgVnXT6bBCKeGwOTvxKOQbL/ohWztdiEW+pOopJuKT2f6daQH" \
"GQb4NSA7BnlKcWorTw0+Twj5QkFJ3oOqY/GUNeqcthy/jycZU3DY76q95oQg/ssm" \
"mwpKmUTwoA5v8DwAJ8N22Hu2mwEATuhW42y9nQGtqAZAIXimLQrWAYz/w4qrLQ==";
//-----END CERTIFICATE-----

// xxxxxxxxxx-private.pem.key
const String privatePemKey = \
//-----BEGIN RSA PRIVATE KEY-----
"MIIEpgIBAAKCAQEA8BxEilzlIIYi4MH3/TOxV1uKrzjgGcVKp1faPF14YPsdUsPy" \
"FnNkeWuNsgPA5bR3apVR/QankkzLP0TV78Srdc2Py1BDJU20AGWQfZfb3B9ywSS2" \
"T683DyAKeK9vtPUZkGMGo/OZawU3ek2H8suNKX1nRP6CPz8/QhMwJSAxyDeC7Tvw" \
"I/GgUj4N8XkSkBDo8VNMcm4tycDBTEua9g+5hCeVhRVfZ+MHe2eu5RqXMm8JCEb6" \
"tONs/3vPgJbZ8mhXAHc96rUrAyw4ClErEsWm5X86k4ohx8hDxEWrSx+nerfGoX69" \
"csn41kepoWr9kWwj4oeMbroqgFIQBI/T2ZuGOQIDAQABAoIBAQCuSpamKRBZL4UZ" \
"yTMOJPvSAa0FzC9ABapPFsJflynOm/mJHpsYGOMSEDRmx7q6Aum8AG/nT3FMDVpr" \
"rrfst8IOXQzH9JcI+QJ0V0UvJZB+lIVkFhYQeYtp+0QenppjrJbF7Fvhnu36dWrz" \
"6FuKrp7lQxY3SteYuXxdpjgIevon7luv15mVVnPFBvCbbyZLZtbSabjyxnHMCG+5" \
"u3FKy+xjnJhQ72Wi4JFts0yOdrO2+I7JKp0iqECRTClo/P8xa9OdvjEZFHTtzrLp" \
"9Vpm7PT7bYdQ7Z7WHxsw3nFtRXZ26I3QLzafaAe2OwYmnE7TapHtpAgcPMqymcmy" \
"Jp+CImR/wg7GK29+xzQkFkWWf+V1eGw7Hp5Ejxo2KvwWaxuEMzfRTyutAoGBAPS+" \
"0kz2MivHD9wT6JXSSofgbFAyWGOaaFA1qSoRW5VojZ55XLKBV/f9UhciPv+gzf4x" \
"97N1EA9EtfWcKjkaT2iwDbM2u27sQ3yjrnEuDLwm7skr4fWZK8MgLwRY8Xn8mTrp" \
"F2QLsS/3aYUY8eYp4gNAOw8GLIkJ7X24/WlRWnJNX3CmKALkppUyfAmanFaeGGeE" \
"DqfAJxryVn8NR/imQmHLepHazr5iZfnWQHPwIW11GAh1HnWO9VQUljiz2BMVxlgq" \
"KWHvHRjs5n3Onq65FmZaNf3ZAoGBAKJaGwM2pd1odVrWUglgsFQmSXOBkiSwftYG" \
"YqsnTIt2CcvAUf+P0Z0OC/CQpR3JvBwNZXVECF04hUPiThgHHfoiH1Rs0DUqwTHj" \
"Goq1s3j0kp4cYA4dh2wu40GNBtKW/8aWrlUwYej7xpfgi3forINOY/FLtxkYjbXL" \
"zCWGT1k5AoGBAIWzrFd7yVc2NjxTMZ8nJOzSpCnE2RMi8NTZECPMvTckiM6CM6U0" \
"iBEnuqWbiPtgKBOQC+yImmfeCXhIUL+ThPqJKK+J010mx6+ghTTnHGKpwtbsHDNs" \
"avV7Ct2lIDuf+JGFGrFKdJyX9cqDzb/EckEDc5oUjDCv8HyJ1ZrjYXvx";
//-----END RSA PRIVATE KEY-----

const String caPemCrt = \
//-----BEGIN CERTIFICATE-----
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF" \
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6" \
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL" \
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv" \
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM" \
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw" \
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6" \
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm" \
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC" \
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA" \
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs" \
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv" \
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU" \
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy" \
"rqXRfboQnoZsG4q5WTP468SQvvG5";
//-----END CERTIFICATE-----

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

void setup() {
  pinMode(0, OUTPUT); 
  digitalWrite(0, HIGH); 
  Serial.begin(115200); Serial.println();
 
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());

  // get current time, otherwise certificates are flagged as expired
  setCurrentTime();

  uint8_t binaryCert[certificatePemCrt.length() * 3 / 4];
  int len = b64decode(certificatePemCrt, binaryCert);
  wiFiClient.setCertificate(binaryCert, len);
  
  uint8_t binaryPrivate[privatePemKey.length() * 3 / 4];
  len = b64decode(privatePemKey, binaryPrivate);
  wiFiClient.setPrivateKey(binaryPrivate, len);

  uint8_t binaryCA[caPemCrt.length() * 3 / 4];
  len = b64decode(caPemCrt, binaryCA);
  wiFiClient.setCACert(binaryCA, len);
}

void loop() {

  pubSubCheckConnect();
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  
  StaticJsonBuffer<200> jsonBuffer;
  String message = "";
  
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
    Serial.print((char)payload[i]);
  }
 
 
  JsonObject& root = jsonBuffer.parseObject(message);
  if(root.success()){
    String alert = root["alert"];
    int duration = (int)root["duration"];
    Serial.println(alert);
    
    if(alert.equals("ring")){
      Serial.println("yes");
      digitalWrite(0, LOW);
      delay(duration);
      digitalWrite(0, HIGH); 
    }
    
    else if(alert.equals("stop")){
      Serial.println("stop");
      digitalWrite(0, HIGH); 
    }
  }
  Serial.println();
}

void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("critical-alert");
      //delay(1000);
    }
    Serial.println(" connected");
    pubSubClient.subscribe("critical-alert");
  }
  pubSubClient.loop();
}

int b64decode(String b64Text, uint8_t* output) {
  base64_decodestate s;
  base64_init_decodestate(&s);
  int cnt = base64_decode_block(b64Text.c_str(), b64Text.length(), (char*)output, &s);
  return cnt;
}

void setCurrentTime() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: "); Serial.print(asctime(&timeinfo));
}
