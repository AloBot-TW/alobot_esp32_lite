
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <arduino-timer.h>

#include "drive.h"
#include "tof.h"
#include "ring.h"
#include "usled.h"

const String fw_version = "1.00";

auto timer = timer_create_default();

Drive drive;
ToF tof;
Ring ring;
UsLed usLed;

bool stop = false;
bool forward = false;
bool obstacleAvoid = true;

uint seqNo = 0;
int dist = -1;

bool tofOn = false;
bool byPassDS = false;

void procDist() {
  if (tofOn) {
      if (dist != -1)
        stop = (dist < 200);
  }

  ring.onDist(stop);

  if (!obstacleAvoid)
    stop = false;

  if (stop&&forward)
    drive.brake(0);

}

static unsigned long startTime = 0;

bool motor_timer(void *) {

  unsigned long mTime = millis();

  //Serial.printf("motor_timer %ul\n", mTime - startTime);

  startTime = mTime;

  drive.onTimer();

  return true; // repeat? true
}

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

BLECharacteristic *pTxCharacteristic;
BLECharacteristic *pRxCharacteristic;

#define SERVICE_UUID            "aea244ea-b2e5-4fa4-8eab-b5ccb9eeea11"
#define CHARACTERISTIC_UUID_RX  "90ef8ac4-e78d-4415-a942-75ce0212b40b"
#define CHARACTERISTIC_UUID_TX  "513b6f86-36bf-4e46-8cd2-2c12d8812bd8"

const char MAX_MSG_SZ = 60;

void bleSend(char* data) {

  if (!deviceConnected)
    return;

  seqNo = seqNo%10;
  char outData[MAX_MSG_SZ] = "";
  sprintf(outData, "S%d ", seqNo);
  strcpy(&outData[strlen(outData)], data);

  pTxCharacteristic->setValue(outData);
  pTxCharacteristic->notify();

   seqNo++;
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};


class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        String receive_data = "";
        for (int i = 0; i < rxValue.length(); i++)
           receive_data += rxValue[i];

        if (receive_data.startsWith("BK")) {
          Serial.println("BK");
          drive.startBrake();
        } else if (receive_data.startsWith("DD")) {
          int nDD = receive_data.length() - 2;
          if (nDD == 3) {
            int l = receive_data[2];
            int r = receive_data[3];
            int d = receive_data[4];

            Serial.printf("DD: %d %d %d\n", l, r, d);

            bool run = true;

            forward = ((l>0x7f)||(r>0x7f));

            if (stop) {
              if ((l>0x7f)||(r>0x7f))
                run = false;
            }

            if (run) {
              drive.startDrive(l, r, d);
            }
          }
        } else if (receive_data.startsWith("RO")) {
          int nRO = receive_data.length() - 2;
          if (nRO == 4) {
            int l = receive_data[2];
            int r = receive_data[3];
            int d = receive_data[4];
            int b = receive_data[5];
            Serial.printf("RO: %d %d %d %d\n", l, r, d, b);

            drive.startRotation(l, r, d, b);
          }
        } else if (receive_data.startsWith("OA")) {
          int nDS = receive_data.length() - 2;
          if (nDS == 1) {
            int on = receive_data[2];
            obstacleAvoid = on;
            Serial.print("OA: ");
            Serial.println(on);
          }
        } else if (receive_data.startsWith("VS")) {
          char buf[MAX_MSG_SZ];
          sprintf(buf, "V%s", fw_version);
          Serial.println(buf);
          bleSend(buf);
        } else if (receive_data.startsWith("ID")) {
          uint64_t mac = ESP.getEfuseMac();
          uint64_t hi = mac >> 48;
          uint64_t chipId = mac - (hi << 48);
          char buf[MAX_MSG_SZ];
          sprintf(buf, "I%06llX", chipId);
          Serial.println(buf);
          bleSend(buf);
        }
      }
    }
};

void initBle() {
  // put your setup code here, to run once:
  BLEDevice::init("ALO_BOT"); // Give it a name

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pTxCharacteristic = pService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_TX), BLECharacteristic::PROPERTY_NOTIFY);
  pTxCharacteristic->addDescriptor(new BLE2902());

  pRxCharacteristic = pService->createCharacteristic(BLEUUID(CHARACTERISTIC_UUID_RX), BLECharacteristic::PROPERTY_WRITE);
  pRxCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();

  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void setup() {

  seqNo = 0;

  stop = false;

  drive.brake(0);

  Serial.begin(115200);
  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }

  initBle();

  ring.init();

  tofOn = tof.init();

  //try
  //tofOn = false;

  if (tofOn)
    Serial.println("Sucessful to boot VL53L0X");
  else
    Serial.println("Failed to detect and initialize sensor!");

  timer.every(10, motor_timer);
}

void procUsLed() {
  stop = usLed.onLoop();
  if (!obstacleAvoid)
    stop = false;
}

void loop() {

    if (!deviceConnected && oldDeviceConnected) {

        delay(100); // give the bluetooth stack the chance to get things ready

        BLEDevice::startAdvertising(); // restart advertising

        oldDeviceConnected = deviceConnected;

        if (tofOn)
          tof.stop();
    }

    // connecting
    if (deviceConnected && !oldDeviceConnected) {

    // do stuff here on connecting
        oldDeviceConnected = deviceConnected;

        if (tofOn)
          tof.start();

        obstacleAvoid = true;
    }

  if (tofOn) {
    if (tof.conOn) {
      dist = tof.getDist();
      char buf[MAX_MSG_SZ];
      if (dist >= 10000)
        dist = 9999;
      sprintf(buf, "D%04d", dist);
      //Serial.println(buf);
      bleSend(buf);
    }
  }

  if (tofOn) {
     if (!tof.conOn)
        ring.rainbow(20);
     else
        procDist();
  } else
     procUsLed();

  timer.tick();

  delay(10);

}
