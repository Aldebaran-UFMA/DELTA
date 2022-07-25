

#define ID_BARCO 0xFFFF
#define espera 3000

#include <SoftwareSerial.h>
#include "LoRa_E32.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

int ldr1 = A0;
int pino_a = A5; //Pino ligado ao A0 do sensor

int valorldr1 = 0; 
int val_a = 0; //Armazena o valor lido do pino analogico

#define DHTPIN 5     /* Digital pin connected to the DHT sensor; Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 -- */
#define DHTTYPE    DHT22

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

SoftwareSerial ss(2,4);
LoRa_E32 slora(&ss,2,7,8); // portas: RX TX AUX M1 M0

char tamanho0 [7];
char tamanho1 [7];
char tamanho2 [7];
char tamanho3 [5];

char string [26];                                         
 
void setup(){

  
  Serial.begin(9600);

  
  slora.begin();  //inicia o Lora
  printLoraConfig();

  
  
  setLoraConfig();

   // Define os pinos do sensor como entrada
  pinMode(ldr1,INPUT);
  pinMode(pino_a, INPUT);

  dht.begin();
  
  sensor_t sensor;
  
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  
  delayMS = sensor.min_delay / 1000;

  
}


void loop(){

  valorldr1 = analogRead(ldr1);
  dtostrf(valorldr1,5,1,tamanho0);
  
  val_a = analogRead(pino_a);
  dtostrf(val_a,5,1,tamanho1);

  delay(delayMS);
  
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  /*else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }*/

  dtostrf(event.temperature,5,2,tamanho2);
  
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  /*else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }*/

  dtostrf(event.relative_humidity,3,1,tamanho3);
  
  sprintf(string,"[%s %s %s %s]",tamanho0,tamanho1, tamanho2, tamanho3);             
  Serial.write(string,26);
 
 transmissao(string);
  
 delay(espera);

}



void setLoraConfig(){
  ResponseStructContainer c;
  c = slora.getConfiguration();
  // It's important get configuration pointer before all other operation
  Configuration configuration = *(Configuration*) c.data;
  Serial.println(c.status.getResponseDescription());
  Serial.println(c.status.code);
  c = slora.getConfiguration();
   configuration.ADDL = 0x00;
  configuration.ADDH = 0x00;
  configuration.CHAN = 0x17;
  configuration.SPED.airDataRate  = AIR_DATA_RATE_000_03;
  configuration.SPED.uartBaudRate = UART_BPS_9600;
  configuration.OPTION.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
  configuration.OPTION.fec = FEC_0_OFF;
  configuration.SPED.uartParity = MODE_00_8N1; // Parity bit
  configuration.OPTION.wirelessWakeupTime = WAKE_UP_2000;
  configuration.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;
  slora.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
  printParameters(configuration);
  c.close();
}

void printLoraConfig(){



ResponseStructContainer c;
  c = slora.getConfiguration();
  // It's important get configuration pointer before all other operation
  Configuration configuration = *(Configuration*) c.data;
  Serial.println(c.status.getResponseDescription());
  Serial.println(c.status.code);

  printParameters(configuration);

  ResponseStructContainer cMi;
  cMi = slora.getModuleInformation();
  // It's important get information pointer before all other operation
  ModuleInformation mi = *(ModuleInformation*)cMi.data;

  Serial.println(cMi.status.getResponseDescription());
  Serial.println(cMi.status.code);

  printModuleInformation(mi);

  c.close();
  cMi.close();


}

void printParameters(struct Configuration configuration) {
  Serial.println("----------------------------------------");

  Serial.print(F("HEAD BIN: "));  Serial.print(configuration.HEAD, BIN);Serial.print(" ");Serial.print(configuration.HEAD, DEC);Serial.print(" ");Serial.println(configuration.HEAD, HEX);
  Serial.println(F(" "));
  Serial.print(F("AddH BIN: "));  Serial.println(configuration.ADDH, BIN);
  Serial.print(F("AddL BIN: "));  Serial.println(configuration.ADDL, BIN);
  Serial.print(F("Chan BIN: "));  Serial.print(configuration.CHAN, DEC); Serial.print(" -> "); Serial.println(configuration.getChannelDescription());
  Serial.println(F(" "));
  Serial.print(F("SpeedParityBit BIN    : "));  Serial.print(configuration.SPED.uartParity, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTParityDescription());
  Serial.print(F("SpeedUARTDataRate BIN : "));  Serial.print(configuration.SPED.uartBaudRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTBaudRate());
  Serial.print(F("SpeedAirDataRate BIN  : "));  Serial.print(configuration.SPED.airDataRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getAirDataRate());

  Serial.print(F("OptionTrans BIN       : "));  Serial.print(configuration.OPTION.fixedTransmission, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getFixedTransmissionDescription());
  Serial.print(F("OptionPullup BIN      : "));  Serial.print(configuration.OPTION.ioDriveMode, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getIODroveModeDescription());
  Serial.print(F("OptionWakeup BIN      : "));  Serial.print(configuration.OPTION.wirelessWakeupTime, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getWirelessWakeUPTimeDescription());
  Serial.print(F("OptionFEC BIN         : "));  Serial.print(configuration.OPTION.fec, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getFECDescription());
  Serial.print(F("OptionPower BIN       : "));  Serial.print(configuration.OPTION.transmissionPower, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getTransmissionPowerDescription());

  Serial.println("----------------------------------------");

}
void printModuleInformation(struct ModuleInformation moduleInformation) {
  Serial.println("----------------------------------------");
  Serial.print(F("HEAD BIN: "));  Serial.print(moduleInformation.HEAD, BIN);Serial.print(" ");Serial.print(moduleInformation.HEAD, DEC);Serial.print(" ");Serial.println(moduleInformation.HEAD, HEX);

  Serial.print(F("Freq.: "));  Serial.println(moduleInformation.frequency, HEX);
  Serial.print(F("Version  : "));  Serial.println(moduleInformation.version, HEX);
  Serial.print(F("Features : "));  Serial.println(moduleInformation.features, HEX);
  Serial.println("----------------------------------------");

}

int count=0;
void transmissao (String message){
//  slora.listen();
  //ResponseStatus rs = slora.sendMessage(message);
  ResponseStatus rs = slora.sendBroadcastFixedMessage(0x17, message);
  Serial.println(rs.getResponseDescription());
  Serial.println("Dados enviados com sucesso!!!");
  ss.listen();
}
static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  //smartDelay(0);
}
static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  //smartDelay(0);
}
