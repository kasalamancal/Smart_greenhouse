#include <OLED_I2C.h>
#include <DHT.h>
#include <SD.h>
#include <virtuabotixRTC.h> 
#include <virtuabotixRTC.h> 

// CLK -> 6, DAT -> 7, RST -> 8
// SCLK -> 6, I/O -> 7, CE -> 8 



//Fuentes en otro fichero
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
#define DHT_TYPE DHT11
#define DHT_PIN 22
#define RESET_PIN 8
#define CHIP_SELECT 53
#define CLK 6
#define DATA 7
#define RST 8
bool IS_SD_CONNECTED = false;
String FILE_NAME = "data.csv";

float humidity;
float temperature;

//SDA = 20 & SCL = 21
OLED oled(SDA, SCL, RESET_PIN);
DHT dht(DHT_PIN, DHT_TYPE);
virtuabotixRTC rtc(CLK, DATA, RST);
File dataFile;

/*
    Prototipos de las funciones
*/

// Inicializa el display OLED
void initOled();
// Se verifica la conexion con la SD
void connectToSD();
// Escribir los datos en el archivo CSV
void writeFile();
// Nombrar al archivo con la fecha actual
void setFileName();
// Informacion a mostrar en el display
void showInformationInOled();

void setup()
{
    Serial.begin(9600);
    myRTC.setDS1302Time(00, 00, 00, 7, 9, 2, 2019); // SS, MM, HH, DW, DD, MM, YYYY
    setFileName();
    connectToSD();
    initOled();
    dht.begin();
}

void loop()
{
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
    showInformationInOled();
    if (IS_SD_CONNECTED)
    {
        writeFile();
    }
}

void writeFile()
{
    dataFile = SD.open(FILE_NAME, FILE_WRITE);
    String data = "";
    if (dataFile)
    {
        IS_SD_CONNECTED = true;
        data += String(humidity) + ", " + String(temperature);
        dataFile.println(data);
        delay(1000);
        dataFile.close();
    }
    else
    {
        digitalWrite(13, HIGH);
        Serial.println("Error opening file");
        IS_SD_CONNECTED = false;
        Serial.end();
    }
}

void connectToSD()
{
    pinMode(13, OUTPUT);
    if (!SD.begin(CHIP_SELECT))
    {
        Serial.println("Connection Failed");
        digitalWrite(13, HIGH);
    }
    else
    {
        IS_SD_CONNECTED = true;
        if (SD.exists(FILE_NAME))
        {
            Serial.println("File exists");
        }
        else
        {
            String columnNames = "Humedad, Temperatura";
            dataFile = SD.open(FILE_NAME, FILE_WRITE);
            Serial.println("File created");
            dataFile.println(columnNames);
            delay(1000);
            dataFile.close();
        }
    }
}

void showInformationInOled()
{
    oled.print("SD STATUS:", LEFT, 0);
    oled.print(String(IS_SD_CONNECTED), RIGHT, 0);
    oled.print("HR  TE", LEFT, 10);
    oled.printNumI(humidity, 0, 20);
    oled.printNumI(temperature, 25, 20);
    oled.update();
}

void setFileName()
{
    // FILE_NAME = CURRENDATE.csv
}

void initOled()
{
    oled.begin();
    oled.setFont(SmallFont);
    delay(1000);
}
