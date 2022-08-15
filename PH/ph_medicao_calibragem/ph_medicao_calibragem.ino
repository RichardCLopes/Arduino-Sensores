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

void setup() {
  Serial.begin(9600);
  }

void loop() {
  int measure = analogRead(ph_pin);
  Serial.print("Measure: ");
  Serial.print(measure);

  double voltage = 5 / 1024.0 * measure; 
  Serial.print("\tVoltage: ");
  Serial.print(voltage, 3);

  //se tiver calibrado em 2.5V, use este
  float Po = 7 + ((2.5 - voltage) / 0.18);
  Serial.print("\tPH: ");
  Serial.print(Po, 3);

  //caso contrario use este
  float Ph2 = multi * voltage + valor_calibracao;
  Serial.print("\tPH2: ");
  Serial.print(Ph2, 3);

  Serial.println("");
  delay(2000);
}
