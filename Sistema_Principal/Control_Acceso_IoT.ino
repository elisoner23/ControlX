#include <SPI.h>
#include <MFRC522.h> 
#include <Wire.h>
#include <hd44780.h> 
#include <hd44780ioClass/hd44780_I2Cexp.h>

// Librerías de Red
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

// ------------------------------------------------------------------
// --- CONFIGURACIÓN DE RED Y TELEGRAM
// ------------------------------------------------------------------
#define WIFI_SSID       "NOMBRE DE RED"
#define WIFI_PASSWORD   "CLAVE"

// **USAR CONST CHAR* PARA GARANTIZAR LA INTEGRIDAD DEL TOKEN/ID**

const char* BOT_TOKEN = "pegar token";
const char* CHAT_ID = "123456789";

// ------------------------------------------------------------------
// --- CONFIGURACIÓN DE PINES Y TARJETAS ---
// ------------------------------------------------------------------
#define RST_PIN D3 
#define SS_PIN D4
#define SDA_PIN D2
#define SCL_PIN D1

const int NUM_CARDS = 2; 
const String authorized_uids[NUM_CARDS] = {
    "A9-4A-C3-01", 
    "85-F3-35-02" 
};
const String authorized_names[NUM_CARDS] = {
    "Charly", 
    "Erick"
};

MFRC522 reader(SS_PIN, RST_PIN);
hd44780_I2Cexp lcd; 
MFRC522::MIFARE_Key key; 

// --- VARIABLES DE ESTADO ---
bool is_authorized = false; 
String user_name = ""; 
// --- MENSAJE DE BIENVENIDA ---
const String WELCOME_MESSAGE = "ControlX Tu mundo, Siempre Seguro";
const int LCD_COLS = 16;
const int LCD_ROWS = 2;


// ------------------------------------------------------------------
// --- FUNCIÓN DE ANIMACIÓN DE TEXTO
// ------------------------------------------------------------------
void scrollText(String text, int row, int delayTime) {
    if (text.length() <= LCD_COLS) {
        // Si el texto cabe, simplemente lo centra y lo muestra.
        lcd.setCursor((LCD_COLS - text.length()) / 2, row);
        lcd.print(text);
        delay(delayTime * 2); 
        return;
    }
    
    // Calcula cuántas veces se debe desplazar
    int maxScroll = text.length() - LCD_COLS;
    
    // Primero, muestra el texto entrando por la derecha
    for (int i = -LCD_COLS; i <= maxScroll; i++) {
        lcd.setCursor(0, row);
        lcd.print("                "); // Limpia la línea
        lcd.setCursor(0, row);
        
        // Determina el inicio y fin de la subcadena a mostrar
        int start = max(0, i);
        int end = min((int)text.length(), i + LCD_COLS);
        
        // Imprime la subcadena
        lcd.print(text.substring(start, end));
        delay(delayTime); 
    }
    
    // Deja el texto final visible por un momento
    lcd.setCursor(0, row);
    lcd.print(text.substring(maxScroll, maxScroll + LCD_COLS));
    delay(delayTime * 3); 
}


// ------------------------------------------------------------------
// --- FUNCIÓN DE ENVÍO DE TELEGRAM (MÉTODO HTTP GET) ---
// ------------------------------------------------------------------
void sendNotificationTelegram(String message) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Error: No hay WiFi para enviar notificación.");
        return;
    }
    
    // 1. Crear el cliente seguro (requerido para HTTPS/Telegram)
    WiFiClientSecure client; 
    client.setInsecure(); // Permite conexiones HTTPS sin verificar el certificado

    // 1. Codificar el mensaje (reemplazar espacios por %20)
    String encodedMessage = "";
    for (int i = 0; i < message.length(); i++) {
        if (message.charAt(i) == ' ') {
            encodedMessage += "%20";
        } else {
            encodedMessage += message.charAt(i);
        }
    }

    // 2. Construir la URL completa de Telegram
    String url = String(api de telegram.org) + BOT_TOKEN + CHAT_ID ;
    
    Serial.print("Intentando enviar Telegram...");

    // 3. Enviar la solicitud HTTP (USANDO LA API MODERNA)
    HTTPClient http;
    // Ahora pasamos el objeto 'client' (WiFiClientSecure)
    http.begin(client, url); 
    
    int httpCode = http.GET();

    // 4. Reportar el resultado
    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            Serial.println("¡Notificación enviada con éxito!");
        } else {
            Serial.print("Fallo al enviar. Código HTTP: ");
            Serial.println(httpCode); 
            // Esto imprimirá el error de Telegram (ej: 404, 400 Bad Request si el token o ID son malos)
            Serial.println(http.getString());
        }
    } else {
        Serial.printf("[HTTP] Fallo de conexión: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end(); // Liberar recursos
}

// ------------------------------------------------------------------
// --- SETUP y LOOP (Llamando a Telegram) ---
// ------------------------------------------------------------------
void setup() {
    Serial.begin(115200); 
    while (!Serial);

    // Inicialización de periféricos
    Wire.begin(D2, D1); 
    SPI.begin();
    reader.PCD_Init();
    lcd.begin(LCD_COLS, LCD_ROWS); // Usamos las constantes
    lcd.backlight();
        
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
        
    // Conexión WiFi
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Conectando WiFi...");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi conectado.");
    
    // ** ANIMACIÓN INICIAL SOLICITADA **
    lcd.clear();
    // Animación en la primera línea
    scrollText(WELCOME_MESSAGE, 0, 200); // 200 ms de retardo para el scroll
    
    // Inicialización de estado final
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ControlX"); 
    lcd.setCursor(0, 1);
    lcd.print("Esperando Acceso");
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.reconnect();
        return; 
    }

    // 1. Lógica RFID
    if (!reader.PICC_IsNewCardPresent()) return;
    if (!reader.PICC_ReadCardSerial()) return;

    String current_uid = "";
    for (byte i = 0; i < reader.uid.size; i++) {
        if (reader.uid.uidByte[i] < 0x10) current_uid += "0";
        current_uid += String(reader.uid.uidByte[i], HEX);
        if (i + 1 != reader.uid.size) current_uid += "-";
    }
    current_uid.toUpperCase(); 
    
    // 2. VERIFICACIÓN DE ACCESO
    is_authorized = false;
    user_name = ""; 
    
    for (int i = 0; i < NUM_CARDS; i++) {
        if (current_uid.equals(authorized_uids[i])) {
            is_authorized = true;
            user_name = authorized_names[i]; 
            break; 
        }
    }

    // 3. MOSTRAR RESULTADO Y ENVIAR NOTIFICACIÓN
    lcd.clear();
    lcd.setCursor(0, 0);

    if (is_authorized) {
        String notification = "✅ ACCESO PERMITIDO: " + user_name + ". UID: " + current_uid;
        
        lcd.print("ACCESO AUTORIZADO");
        lcd.setCursor(0, 1);
        lcd.print("Welcome " + user_name);

        sendNotificationTelegram(notification); 
    } else {
        String notification = "❌ ALERTA: ACCESO DENEGADO. UID: " + current_uid;

        lcd.print("ACCESO DENEGADO");
        lcd.setCursor(0, 1);
        lcd.print("UID No Valido");

        sendNotificationTelegram(notification); 
    }

    // 4. Espera y limpieza
    delay(3000); 
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ControlX");
    lcd.setCursor(0, 1);
    lcd.print("Esperando Acceso");

    reader.PICC_HaltA();
    reader.PCD_StopCrypto1();
}
