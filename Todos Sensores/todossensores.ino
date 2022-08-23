//Bibliotecas
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#include <EEPROM.h>
#include "GravityTDS.h"

//Declaracoes do termometro----------------------------
const int PINO_ONEWIRE = 12; // Define pino do sensor
OneWire oneWire(PINO_ONEWIRE); // Cria um objeto OneWire
DallasTemperature sensor(&oneWire); // Informa a referencia da biblioteca dallas temperature para Biblioteca onewire
DeviceAddress endereco_temp; // Cria um endereco temporario da leitura do sensor
//-----------------------------------------------------

//Declaracoes do indice UV----------------------------
int leituraUV=0; // VARIÁVEL PARA ARMAZENAR A LEITURA DA PORTA ANALÓGICA
byte indiceUV=0; // VARIÁVEL PARA ARMAZENAR A CONVERSÃO PARA INDICE UV
#define pinSensorUV    A0
//-----------------------------------------------------

//Declaracoes do sensor de umidade---------------------
#define DHTPIN 10     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
//-----------------------------------------------------

//Declaracoes do sensor TDS----------------------------
#define TdsSensorPin A2
GravityTDS gravityTds;
float temperature = 25;
float tdsValue = 0;
//-----------------------------------------------------

//Declaracoes do sensor de PH----------------------------
//Para calibrar o sensor de pH é necessario seguir os seguintes passos
//1º - Medir voltagem do sensor no vinagre (pH 3) e no alvejante(pH 13)
//2º - Tirar a equação da reta em https://pt.symbolab.com/solver/line-equation-calculator
//3º - Altera os valores de valor_calibracao e multi de acordo com a equação
//Caso não tenha os materiais, é possivel calibrar utilizando outros elementos, cujo saiba o pH
//Se for possível calibrar o potenciometro do analogico utilizado em 2.5V a fórmula é -5.7 * X -21.24

int ph_pin = A4; 
//float valor_calibracao = 21.34;
float valor_calibracao = 26.8;
//float multi = -5.7;
float multi = -5.7;
//-----------------------------------------------------

//Declaracoes do sensor de Vazao-----------------------
//definicao do pino do sensor e de interrupcao
const int INTERRUPCAO_SENSOR = 0; //interrupt = 0 equivale ao pino digital 2
const int PINO_SENSORVZ = 2;

//definicao da variavel de contagem de voltas
unsigned long contador = 0;

//definicao do fator de calibracao para conversao do valor lido
const float FATOR_CALIBRACAO = 4.5;

//definicao das variaveis de fluxo
float fluxo = 0;

//definicao da variavel de intervalo de tempo
unsigned long tempo_antes = 0;
//-----------------------------------------------------


void setup() {
  Serial.begin(9600); // Inicia a porta serial
  pinMode(pinSensorUV, INPUT); //pinMode do indice de UV em INPUT
  pinMode(PINO_SENSORVZ, INPUT_PULLUP);
  Serial.println("Start"); // Imprime a mensagem inicial
  dht.begin();
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(4096);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization
  sensor.begin(); // Inicia o sensor
}
  
void loop() {
  delay(1000);
  //Pegando Temperatura do termometro que vai estar no liquido
  sensor.requestTemperatures(); // Envia comando para realizar a conversão de temperatura
  if (!sensor.getAddress(endereco_temp,0)) { // Encontra o endereco do sensor no barramento
    Serial.println("SENSOR NAO CONECTADO"); // Sensor conectado, imprime mensagem de erro
  } else {
    Serial.print("Temp Liq: "); // Imprime a temperatura no monitor serial
    Serial.print(sensor.getTempC(endereco_temp), 2);
    Serial.print("ºC");// Busca temperatura para dispositivo
  }

  //Pegando indice UV
  leituraUV = analogRead(pinSensorUV); // REALIZA A LEITURA DA PORTA ANALÓGICA 
  indiceUV = map(leituraUV, 0,203,0,11) ; // CONVERTE A FAIXA DE SINAL DO SENSOR DE 0V A 1V PARA O INDICE UV DE 0 A 10.
  Serial.print("\tIndice UV: ");
  Serial.print(indiceUV);

  //Pegando umidade e temperatura do ar
  float umidade = dht.readHumidity();
  float temp = dht.readTemperature();
  Serial.print(F("\tUmidade: "));
  Serial.print(umidade);
  Serial.print(F("%\t\tTemp Ar: "));
  Serial.print(temp);
  Serial.print(F("°C "));

  //Pegando TDS
  gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
  gravityTds.update();  //sample and calculate
  tdsValue = gravityTds.getTdsValue();  // then get the value
  Serial.print("\tTDS:");
  Serial.print(tdsValue,0);
  Serial.print("ppm");

  //Pegando o PH
  
  int measure = analogRead(ph_pin);
  //Serial.print("Measure: ");
  //Serial.print(measure);
  double voltage = 5 / 1024.0 * measure; 
  /*
  Serial.print("\tVoltage: ");
  Serial.print(voltage, 3);
*/

  //se tiver calibrado em 2.5V, use este
  float Po = 7 + ((2.5 - voltage) / 0.18);
  Serial.print("\tPH: ");
  Serial.print(Po, 2);
  Serial.print("");
  //caso contrario use este
  /*
  float Ph2 = multi * voltage + valor_calibracao;
  Serial.print("\tPH2: ");
  Serial.print(Ph2, 3);
  Serial.println("");
  */

  //Pega valor de Vazao
  //executa a contagem de pulsos uma vez por segundo
  if((millis() - tempo_antes) > 1000){
    
    detachInterrupt(INTERRUPCAO_SENSOR); //desabilita a interrupcao para realizar a conversao do valor de pulsos

    fluxo = ((1000.0 / (millis() - tempo_antes)) * contador) / FATOR_CALIBRACAO; //conversao do valor de pulsos para L/min

    //exibicao do valor de fluxo
    Serial.print("\tFluxo: ");
    Serial.print(fluxo);
    Serial.println(" L/min");

    contador = 0; //reinicializacao do contador de pulsos

    tempo_antes = millis(); //atualizacao da variavel tempo_antes

    attachInterrupt(INTERRUPCAO_SENSOR, contador_pulso, FALLING); //contagem de pulsos do sensor
    
  }
  delay(1000);
}

void contador_pulso() {
  contador++;
}
