#include <Arduino.h>
#include "WiFi.h"
#include "WiFiUdp.h"
#include "driver/dac.h"

const char *ssid = "Tenda_HelloKitty";
const char *password = "Wrg2020.Tddl";

WiFiUDP udpServer;
unsigned int udpServerPort = 8001;

/*
 * 连接 WiFi
 */
void connectWiFi() {
    WiFiClass::mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFiClass::status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting ...");
    }

    Serial.print("Connected to: ");
    Serial.println(ssid);

    // ESP32 的 IP 地址
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    // write your initialization code here
    Serial.begin(115200);

    // 连接 WiFi
    connectWiFi();

    // 在 8001 端口启动 UDP 服务
    udpServer.begin(udpServerPort);

    // 25和26引脚输入电压
    dac_output_enable(DAC_CHANNEL_1);
    dac_output_voltage(DAC_CHANNEL_1, 0);
    dac_output_enable(DAC_CHANNEL_2);
    dac_output_voltage(DAC_CHANNEL_2, 0);
}

void loop() {

    // write your code here
    int packetSize = udpServer.parsePacket();
    if (packetSize) {
        // UDP 客户端信息
        Serial.println(packetSize);

        Serial.print("Received From: ");
        Serial.print(udpServer.remoteIP());
        Serial.print(":");
        Serial.println(udpServer.remotePort());

        // 接收到的 UDP 数据
        char packetBuffer[packetSize];
        udpServer.read(packetBuffer, packetSize);
        Serial.print("Received Data: ");
        Serial.println(packetBuffer);

        // CPU 和 RAM 使用率百分比
        int cpu = String(packetBuffer[0]).toInt() * 100 + String(packetBuffer[2]).toInt() * 10 + String(packetBuffer[3]).toInt();
        int ram = String(packetBuffer[5]).toInt() * 100 + String(packetBuffer[7]).toInt() * 10 + String(packetBuffer[8]).toInt();
        int cpuDACValue = cpu * 79 / 100;
        int ramDACValue = ram * 79 / 100;

        // 调节电压
        dac_output_voltage(DAC_CHANNEL_1, cpuDACValue);
        dac_output_voltage(DAC_CHANNEL_2, ramDACValue);
    }

}