push#include <ArduinoJson.h>
#include <ESP8266WebServer.h>

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

#include <Wire.h>
#include <MPU6050.h>
#include <I2Cdev.h>


MPU6050 accelgyro;
int16_t ax, ay, az;
int16_t gx, gy, gz;
float x, y, z, sens_read;
float ax_offset, ay_offset, az_offset;
int final_accel;


#include "./include/ap.h"
#include "./include/data.h"
#include "./include/util.h"
#include "./include/view.h"
#include "./include/wifi.h"
#include "./include/socket.h"
#include "./include/tasker.h"
#include "./include/storage.h"
#include "./include/taskmanager.h"
//#include "./include/SlaveTiny/SlaveTiny.h"
//#include "./include/SlaveTiny/SlaveTiny.cpp"
#include <ESP8266HTTPClient.h>

#include "./constants/routes.h"
#include "./constants/events.h"


//SlaveTiny attiny(0x0A);

void update(String event)
{
    Socket::broadcast(EVT_UPDT, Data::itemAsJson(event));
}

void root(void)
{
    AP::server.send(200, "text/html", View::index);
}

void credentialsRoute(void)
{
    AP::server.send(200, "text/html", View::wifiManager);
}

void connectRoute(void)
{
    if (AP::server.args() != 2)
    {
        AP::server.send(200, "text/html", View::connectionFail);
        return;
    }

    bool connected = false;
    String ssid = AP::server.arg("ssid");
    String password = AP::server.arg("password");
    Serial.println(ssid);
    Serial.println(password);
    
    if(Wifi::isConnected()) Wifi::disconnect();
    connected = Wifi::connect(ssid, password);

    if(connected)
    {
        AP::server.send(200, "text/html", View::connectionSuccess);
        return;
    }
    
    AP::server.send(200, "text/html", View::connectionFail);
}


void readSensors(void)
{

    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); //This function collects data from every direction possible from the mpu
    x = (ax - 32768) / 1670.0  - ax_offset; //Values displayed as m/s2
    y = (ay - 32768) / 1670.0  - ay_offset; //Offset removed from reading
    z = (az - 32768) / 1670.0  - az_offset;
  
    x=abs(x); //Absolute value for vibration. 
    y=abs(y);
    z=abs(z);
  
    //Sens_read = Valor final da aceleração. Enviar esse valor para a IHM
    sens_read = max(max(x, y), z);
    
    // final_accel = Eixo onde está sendo medida a maior aceleração pelo sensor.
    if (sens_read == x) {
      final_accel = 1; //1 = x_axis
    }
    else if (sens_read == y) {
      final_accel = 2; //2 = y_axis
    }
    else if (sens_read == z) {
      final_accel = 3; //3 = z_axis
    }

    Serial.print("eixo:\t");
    Serial.print(final_accel);
    Serial.print("\t");
    Serial.print("Leitura Maxima:\t");
    Serial.print(sens_read);
    Serial.print("\n");

    Data::sensors[2] = 0;
    Data::sensors[0] = sens_read;//attiny.read(0x01); 
    Data::sensors[1] = final_accel;//attiny.read(0x02);


    if (Wifi::isConnected())
    {
        Serial.println("CONNECTED AND READY TO SEND DATA");
        int httpCode = 0;
        String payload = "";
//        int x = Data::sensors[2] >= 750? 1 : 0;

        HTTPClient http1;
        HTTPClient http2;
        String host = "";
        String u1 = "http://" + host + "/api/water/sensor";
        String u2 = "http://" + host + "/api/water/sensor2";
        String m1 = "{\"id\": 3, \"value\": [" + String(Data::sensors[0]) + ", " + String(Data::sensors[1]) + "]}";
//        String m2 = "{\"id\": 4, \"value\": " + String(x) + "}";

        http1.begin(u1);
        http1.addHeader("Content-Type", "application/json");

        httpCode = http1.POST(m1.c_str()); //send the request
        payload = http1.getString();

        Serial.println(httpCode); //Print HTTP return code
        Serial.println(payload);  //Print request response payload

        http1.end();
//
//        http2.begin(u2);
//        http2.addHeader("Content-Type", "application/json");
//
//        httpCode = http2.POST(m2.c_str()); //send the request
//        payload = http2.getString();
//
//        Serial.println(httpCode); //Print HTTP return code
//        Serial.println(payload);  //Print request response payload
//
//        http2.end();
    }
}

void connectionFlag(void)
{
    Data::isConnected = Wifi::isConnected();
    
    if (!Data::isConnected)
    {
        Wifi::tryToConnect();
    }
}

void sendFullUpdate(uint8_t client, String payload)
{
    Socket::emit(client, EVT_UPDT, Data::asJson());
}

void broadcastFullUpdate(uint8_t client, String payload)
{
    Data::parse(payload);
    Socket::broadcast(EVT_UPDT, Data::asJson());
}

void setup()
{
    delay(1000);
    Serial.begin(115200);
    Serial.println("OK");

    
  
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin(D2, D1);
    #endif
    //set mpu
    accelgyro.initialize(); //Initializing MPU6050
    // verify connection
    Serial.println("Testing MPU6050...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    ax_offset = (ax - 32768) / 1670.0; //Converting raw readings 
    ay_offset = (ay - 32768) / 1670.0; // 16bits 
    az_offset = (az - 32768) / 1670.0;

    

    AP::onNotFound(root);
    AP::on(STA_CONNECT, HTTP_GET, connectRoute);
    AP::on(STA_WIFI_FORM, HTTP_GET, credentialsRoute);

    Socket::on(EVT_PULL, sendFullUpdate);
    Socket::on(EVT_PUSH, broadcastFullUpdate);

    Storage::init();
    Wifi::clear();
    AP::init();
    Wifi::init();
    Socket::init();

    Tasker::add("sensors", readSensors, 1000);
    Tasker::add("connection", connectionFlag, 10000);
    Tasker::setActivationHandler(update);
}

void loop()
{
    AP::doTheLoop();
    Socket::doTheLoop();
    Tasker::doTheLoop();
}
