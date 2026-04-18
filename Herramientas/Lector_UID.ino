#include <SPI.h>
#include <MFRC522.h> 
#include <Wire.h>

// Librerías compatibles con ESP8266 para I2C LCD
#include <hd44780.h> 
#include <hd44780ioClass/hd44780_I2Cexp.h>

// ------------------------------------------------------------------
// --- CONFIGURACIÓN
// ------------------------------------------------------------------

// PINES RFID (SPI)
#define RST_PIN D3 // RST conectado a D3 (GPIO0)
#define SS_PIN D4 // SDA/SS conectado a D4 (GPIO2)

// PINES I2C LCD (Estándar para ESP8266)
#define SDA_PIN D2 // SDA (Data) conectado a D2 (GPIO4)
#define SCL_PIN D1 // SCL (Clock) conectado a D1 (GPIO5)

// ------------------------------------------------------------------
// --- INICIALIZACIÓN DE PERIFÉRICOS ---
// ------------------------------------------------------------------
MFRC522 reader(SS_PIN, RST_PIN);

hd44780_I2Cexp lcd; 
MFRC522::MIFARE_Key key; 

void setup() {
    Serial.begin(115200); 
    while (!Serial);

    // Inicialización I2C (LCD)
    Wire.begin(SDA_PIN, SCL_PIN);
    lcd.begin(16, 2); // Inicia la LCD con 16 columnas y 2 filas
    lcd.backlight();
    
    // Inicialización SPI (RFID)
    SPI.begin();
    reader.PCD_Init();
    delay(4);

    // Llave de seguridad por defecto
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    // Mensaje inicial en LCD y Serial
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lector de UID OK");
    lcd.setCursor(0, 1);
    lcd.print("Esperando tarjeta");
    Serial.println("Sistema de lectura de UID iniciado.");
}

void loop() {
    // 1. Verifica si hay una nueva tarjeta presente
    if (!reader.PICC_IsNewCardPresent()) return;
    
    // 2. Lee el Serial (UID) de la tarjeta
    if (!reader.PICC_ReadCardSerial()) return;

    // 3. Convierte UID a formato de texto (String)
    String current_uid = "";
    for (byte i = 0; i < reader.uid.size; i++) {
        if (reader.uid.uidByte[i] < 0x10) current_uid += "0";
        current_uid += String(reader.uid.uidByte[i], HEX);
        if (i + 1 != reader.uid.size) current_uid += "-";
    }
    current_uid.toUpperCase(); 
    
    // 4. Muestra el UID en el LCD 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tarjeta UID:");
    lcd.setCursor(0, 1);
    lcd.print(current_uid);

    // 5. Imprime el UID en el monitor serial para copiarlo
    Serial.print("UID LEIDO: ");
    Serial.println(current_uid);

    delay(2000); 
    
    // 6. Vuelve al estado de espera
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lector de UID OK");
    lcd.setCursor(0, 1);
    lcd.print("Esperando tarjeta");

    reader.PICC_HaltA();
    reader.PCD_StopCrypto1();
}
