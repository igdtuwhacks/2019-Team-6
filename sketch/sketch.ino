// import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Arduino.h>
#include "esp_system.h"

// esp internal tempearture
extern "C" {
    uint8_t temprature_sens_read();
}

// replace with your network credentials
const char* ssid = "REU";
const char* password = "apnachalale";

// gpio that dht is connected to
#define DHTPIN 15
// type of dht sensor
#define DHTTYPE DHT11
// initialise dht sensor
DHT dht(DHTPIN, DHTTYPE);

// create bmp object
Adafruit_BMP085 bmp;

// gpio that potentiometer is connected to
#define POTPIN 32

// create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readButton() {
    // read button
    int t = digitalRead(33);
    if (isnan(t)) {   
        Serial.println("Failed to read button value!");
        return "--";
    } else {
        Serial.println(t);
        if (t == 0) {
            digitalWrite(27, LOW);
        } else {
            digitalWrite(27, HIGH);
        }
        return String(t);
    }
}

String readESPTemperature() {
    // read temperature as celsius
    uint8_t temp_farenheit= temprature_sens_read();
    double t = ( temp_farenheit - 32 ) / 1.8;
    if (isnan(t)) {   
        Serial.println("Failed to read from DHT sensor!");
        return "--";
    } else {
        Serial.println(t);
        return String(t);
    }
}

String readLDR() {
    // read ldr
    float t = analogRead(A0);
    if (isnan(t)) {   
        Serial.println("Failed to read from LDR sensor!");
        return "--";
    } else {
        Serial.println(t);
        if (t > 200) {
            digitalWrite(26, LOW);
        } else {
            digitalWrite(26, HIGH);
        }
        return String(t);
    }
}

String readDHTTemperature() {
    // read temperature as celsius
    float t = dht.readTemperature();
    if (isnan(t)) {   
        Serial.println("Failed to read from DHT sensor!");
        return "--";
    } else {
        Serial.println(t);
        return String(t);
    }
}

String readDHTHumidity() {
    // read humidity
    float h = dht.readHumidity();
    if (isnan(h)) {
        Serial.println("Failed to read from DHT sensor!");
        return "--";
    } else {
        Serial.println(h);
        return String(h);
    }
}

String readBMPTemperature() {
    // read temperature
    float h = bmp.readTemperature();
    if (isnan(h)) {
        Serial.println("Failed to read from BMP sensor!");
        return "--";
    } else {
        Serial.println(h);
        return String(h);
    }
}

String readBMPPressure() {
    // read pressure
    float h = bmp.readPressure();
    if (isnan(h)) {
        Serial.println("Failed to read from BMP sensor!");
        return "--";
    } else {
        Serial.println(h);
        return String(h);
    }
}

String readBMPAltitude() {
    // read altitude
    float h = bmp.readAltitude();
    if (isnan(h)) {
        Serial.println("Failed to read from BMP sensor!");
        return "--";
    } else {
        Serial.println(h);
        return String(h);
    }
}

String readBMPRealAltitude() {
    // read real altitude
    float h = bmp.readAltitude(102000);
    if (isnan(h)) {
        Serial.println("Failed to read from BMP sensor!");
        return "--";
    } else {
        Serial.println(h);
        return String(h);
    }
}

String readBMPSeaLevelPressure() {
    // read sea level altitude
    float h = bmp.readSealevelPressure();
    if (isnan(h)) {
        Serial.println("Failed to read from BMP sensor!");
        return "--";
    } else {
        Serial.println(h);
        return String(h);
    }
}

String readPOT() {
    // read pressure
    float h = analogRead(POTPIN);
    if (isnan(h)) {
        Serial.println("Failed to read from POT sensor!");
        return "--";
    } else {
        Serial.println(h);
        return String(h);
    }
}

// html template
const char index_html[] PROGMEM = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset='utf-8'>
        <meta name='viewport' content='width=device-width, initial-scale=1'>
        <title>Weather Station</title>
        <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/bulma/0.7.4/css/bulma.min.css'>
        <style>
            #hero-bg {
                background-image: url('https://img.wallpapersafari.com/desktop/1680/1050/31/46/GgyrPf.jpg');
                background-position: center;
                background-repeat: no-repeat;
                background-size: cover;
            }
            #hero-bg-filter {
                background-color: rgba(0,0,0,0.2);
            }
            #table-bg {
                background-color: rgba(0,0,0,0);
                color: white;
            }
            .table.has-text-right td:nth-child(2) {
                text-align: right;
            }
        </style>
    </head>
    <body>
        <div id='hero-bg' class='hero is-fullheight has-text-centered'>
            <div id='hero-bg-filter' class='hero-body'>
                <div class='container'>
                    <h1 class='title has-text-white'>Weather Station</h1>
                    <table id='table-bg' class='table is-fullwidth has-text-right'>
                        <tbody>
                            <tr>
                                <td>Button Value</td>
                                <td><span id='btnvalu'>%BTNVALU%</span></td>
                            </tr>
                            <tr>
                                <td>ESP Temperature</td>
                                <td><span id='esptemp'>%ESPTEMP%</span> &deg;C</td>
                            </tr>
                            <tr>
                                <td>LDR Light Intensity</td>
                                <td><span id='ldrinte'>%LDRINTE%</span></td>
                            </tr>
                            <tr>
                                <td>DHT Temperature</td>
                                <td><span id='dhttemp'>%DHTTEMP%</span> &deg;C</td>
                            </tr>
                            <tr>
                                <td>DHT Humidity</td>
                                <td><span id='dhthumi'>%DHTHUMI%</span> / 100</td>
                            </tr>
                            <tr>
                                <td>BMP Temperature</td>
                                <td><span id='bmptemp'>%BMPTEMP%</span> &deg;C</td>
                            </tr>
                            <tr>
                                <td>BMP Pressure</td>
                                <td><span id='bmppres'>%BMPPRES%</span> Pa</td>
                            </tr>
                            <tr>
                                <td>BMP Altitude</td>
                                <td><span id='bmpalti'>%BMPALTI%</span> m</td>
                            </tr>
                            <tr>
                                <td>BMP Real Altitude</td>
                                <td><span id='bmpreal'>%BMPREAL%</span> m</td>
                            </tr>
                            <tr>
                                <td>BMP Sea Level Pressure</td>
                                <td><span id='bmpseal'>%BMPSEAL%</span> Pa</td>
                            </tr>
                            <tr>
                                <td>POT</td>
                                <td><span id='pot'>%POT%</span> Ohm</td>
                            </tr>
                        </tbody>
                    </table>
                </div>
            </div>
        </div>
        <script>
        setInterval(() => {
            const xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById('btnvalu').innerHTML = this.responseText;
                }
            };
            xhttp.open('GET', '/btnvalu', true);
            xhttp.send();
        }, 0);
        setInterval(() => {
            const xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById('esptemp').innerHTML = this.responseText;
                }
            };
            xhttp.open('GET', '/esptemp', true);
            xhttp.send();
        }, 5000 );
        setInterval(() => {
            const xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById('ldrinte').innerHTML = this.responseText;
                }
            };
            xhttp.open('GET', '/ldrinte', true);
            xhttp.send();
        }, 100 );
        setInterval(() => {
            const xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById('dhttemp').innerHTML = this.responseText;
                }
            };
            xhttp.open('GET', '/dhttemp', true);
            xhttp.send();
        }, 5000 );
        setInterval(() => {
            const xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById('dhthumi').innerHTML = this.responseText;
                }
            };
            xhttp.open('GET', '/dhthumi', true);
            xhttp.send();
        }, 5000);
        setInterval(() => {
            const xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById('bmptemp').innerHTML = this.responseText;
                }
            };
            xhttp.open('GET', '/bmptemp', true);
            xhttp.send();
        }, 5000);
        setInterval(() => {
            const xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById('bmppres').innerHTML = this.responseText;
                }
            };
            xhttp.open('GET', '/bmppres', true);
            xhttp.send();
        }, 5000);
        setInterval(() => {
            const xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById('bmpalti').innerHTML = this.responseText;
                }
            };
            xhttp.open('GET', '/bmpalti', true);
            xhttp.send();
        }, 5000);
        setInterval(() => {
            const xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById('bmpreal').innerHTML = this.responseText;
                }
            };
            xhttp.open('GET', '/bmpreal', true);
            xhttp.send();
        }, 5000);
        setInterval(() => {
            const xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById('bmpseal').innerHTML = this.responseText;
                }
            };
            xhttp.open('GET', '/bmpseal', true);
            xhttp.send();
        }, 5000);
        setInterval(() => {
            const xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById('pot').innerHTML = this.responseText;
                }
            };
            xhttp.open('GET', '/pot', true);
            xhttp.send();
        }, 1000);
        </script>
    </body>
    </html>
)rawliteral";

// replaces placeholder
String processor(const String& var) {
    if (var == "ESPTEMP") {
        return readESPTemperature();
    } else if (var == "BTNVALU"){
        return readButton();
    } else if (var == "LDRINTE"){
        return readLDR();
    } else if (var == "DHTTEMP"){
        return readDHTTemperature();
    } else if (var == "DHTHUMI") {
        return readDHTHumidity();
    } else if (var == "BMPTEMP") {
        return readBMPTemperature();
    } else if (var == "BMPPRES") {
        return readBMPPressure();
    } else if (var == "BMPALTI") {
        return readBMPAltitude();
    } else if (var == "BMPREAL") {
        return readBMPRealAltitude();
    } else if (var == "BMPSEAL") {
        return readBMPSeaLevelPressure();
    } else if (var == "POT") {
        return readPOT();
    }
    return String();
}

void setup() {
    // serial port for debugging
    Serial.begin(115200);
    // led
    pinMode(33, INPUT);
    pinMode(26, OUTPUT);
    pinMode(27, OUTPUT);
    // initialise dht sensor
    dht.begin();
    // initialise bmp
    bmp.begin();
    // connect to wifi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("WiFi connected.");
    // Print esp32 local ip address
    Serial.println(WiFi.localIP());
    // Routes
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html, processor);
    });
    server.on("/btnvalu", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readButton().c_str());
    });
    server.on("/esptemp", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readESPTemperature().c_str());
    });
    server.on("/ldrinte", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readLDR().c_str());
    });
    server.on("/dhttemp", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readDHTTemperature().c_str());
    });
    server.on("/dhthumi", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readDHTHumidity().c_str());
    });
    server.on("/bmptemp", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readBMPTemperature().c_str());
    });
    server.on("/bmppres", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readBMPPressure().c_str());
    });
    server.on("/bmpalti", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readBMPAltitude().c_str());
    });
    server.on("/bmpreal", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readBMPRealAltitude().c_str());
    });
    server.on("/bmpseal", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readBMPSeaLevelPressure().c_str());
    });
    server.on("/pot", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readPOT().c_str());
    });
    // Start server
    server.begin();
}
 
void loop() {

}
