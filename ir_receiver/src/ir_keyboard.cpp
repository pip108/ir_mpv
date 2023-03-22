#include "Arduino.h"
#include "IRremote.hpp"
#include "WiFi.h"
#include "WiFiUdp.h"

const char *wifi_ssid = "";
const char *wifi_pw = "";
const char *hostname = "ir_remote";

const uint8_t ir_pin = 23;
const char *remote_addr = "";
const uint16_t remote_port = 30108;

WiFiUDP udp;

uint32_t debounce;
uint16_t last_cmd;

void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(hostname);
    WiFi.begin(wifi_ssid, wifi_pw);
    debounce = millis();
    int wifi_status;
    while ((wifi_status = WiFi.status()) != WL_CONNECTED)
    {
        Serial.println("Connecting WiFi...");
        delay(1000);
    }
    IrReceiver.begin(ir_pin);
}

void send_cmd(uint16_t cmd)
{
    if (cmd == last_cmd && millis() - debounce < 200)
    {
        return;
    }
    last_cmd = cmd;
    debounce = millis();
    udp.beginPacket(remote_addr, remote_port);
    udp.write((uint8_t)IrReceiver.decodedIRData.command);
    udp.endPacket();
}

void loop()
{
    if (IrReceiver.decode())
    {
        Serial.printf("IR cmd: %u\r\n", IrReceiver.decodedIRData.command);
        send_cmd(IrReceiver.decodedIRData.command);
        IrReceiver.resume();
    }
    delay(1);   
}