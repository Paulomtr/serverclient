#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic;

// Função para gerar um código de resposta aleatório
String generateResponseCode() {
    String characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    String responseCode = "";

    for (int i = 0; i < 5; i++) {
        responseCode += characters[random(characters.length())];
    }

    return responseCode;
}

class MyServerCallbacks : public BLEServerCallbacks {
public:
    void onConnect(BLEServer* pServer) {
        Serial.println("Client connected");
    }

    void onDisconnect(BLEServer* pServer) {
        Serial.println("Client disconnected");
    }
};

class MyCallbacks : public BLECharacteristicCallbacks {
public:
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();

        if (value.length() > 0) {
            Serial.println("Received value: ");
            for (int i = 0; i < value.length(); i++)
                Serial.print(value[i]);

            Serial.println();

            // Check the received value and respond accordingly
            if (value == "Open") {
                // Generate a response code (5 characters - letters and numbers)
                String responseCode = generateResponseCode();
                Serial.println("Sending response code: " + responseCode);

                // Set the response code as the characteristic value
                pCharacteristic->setValue(responseCode.c_str());
                pCharacteristic->notify();

                // Wait for the acknowledgment from the client
                pCharacteristic->indicate();  // Use indicate() instead of notify()
                delay(1000);  // Adjust the delay as needed
            } else {
                pCharacteristic->setValue("Invalid command");
            }
        }
    }
};


void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE work!");

    BLEDevice::init("Long name works now");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
                          CHARACTERISTIC_UUID,
                          BLECharacteristic::PROPERTY_READ |
                          BLECharacteristic::PROPERTY_WRITE |
                          BLECharacteristic::PROPERTY_NOTIFY |
                          BLECharacteristic::PROPERTY_INDICATE
                        );

    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->setValue("Hello World says Neil");
    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it on your phone!");
}

void loop() {
    delay(2000);
}
