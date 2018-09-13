#include <SPI.h>
#include <Ethernet.h>

IPAddress ip(10, 8, 4, 100);
EthernetServer server = EthernetServer(11201);

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xF1, 0xED
};

byte dns1[] = {
  10, 0, 0, 10
};

byte gw[] = {
  10, 8, 0, 1
};

byte sn[] = {
  255, 255, 0, 0
};

char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,

#define relayPinsNum 2
int relayPins[relayPinsNum] = {5, 6};
int buttonPin = A0; // variable for reading the pushbutton status
int prevButtonState = 0; // variable for reading the pushbutton status


String message;
String   messageRef = String("switch");
String   messageRefEnd = String("\r");


void setup() {
  // start the Ethernet and UDP:
  Ethernet.begin(mac, ip, dns1, gw, sn);

  server.begin();

  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  delay(1000);
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  messageRef = messageRef + messageRefEnd;

  for (int i = 0; i < relayPinsNum; i++)
    pinMode(relayPins[i], OUTPUT);

  pinMode(buttonPin, INPUT);

  prevButtonState = digitalRead(buttonPin);
}

void relayOn() {
  for (int i = 0; i < relayPinsNum; i++)
    digitalWrite(relayPins[i], HIGH);
}

void relayOff() {
  for (int i = 0; i < relayPinsNum; i++)
    digitalWrite(relayPins[i], LOW);
}

void relaySwitch() {
  relayOff();
  Serial.print("<delay>");
  delay(120000);
  //delay(1000);
}

void loop() {
  int buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH && prevButtonState == LOW) {
    relaySwitch();
  }
  relayOn();
  prevButtonState = buttonState;
  EthernetClient client = server.available();

  if (client)
  {
    int packetSize = client.available();
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    // read the packet into packetBufffer
    client.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    message = String(packetBuffer);

    if (message.equals(messageRef)) {
      Serial.println("recoginzes");
      delay(100);
      client.write("OK\r");
      relaySwitch();
    } 
  }
}

