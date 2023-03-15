class Thermistor
{
private:

  int TEMP_PIN;
  double calculateAlpha();
  double calculateTemp();

public:
  double m_Vcc = 5; //Applied Voltage
  double m_R0 = 100 + 0.22; //Resistance at 0°C
  double m_R100 = 138.5055 + 0.22; //Resistance at 0°C
  double m_R = 0;
  double m_alpha = 0;
  double m_knownR = 214;

  Thermistor(int pin);
  ~Thermistor();

  double measureResistance();

  double getTemp();
};

Thermistor::Thermistor(int pin)
{
  TEMP_PIN = pin;
  pinMode(TEMP_PIN, INPUT);
}

Thermistor::~Thermistor()
{
}



double Thermistor::measureResistance()
{
  double temp = analogRead(TEMP_PIN)*m_Vcc;
  double Vout = temp/1024.0;
  temp = (m_Vcc/Vout) - 1;
  m_R = m_knownR * temp;
  return m_R;
}
double Thermistor::calculateAlpha()
{
  m_alpha = (m_R100 - m_R0)/(m_R0*100.0);
  return m_alpha;
}

double Thermistor::calculateTemp()
{
  m_R = measureResistance();
  m_alpha = (m_R100 - m_R0)/(m_R0*100.0);
  return m_R/(m_R0*m_alpha) - 1.0/m_alpha;
}

double Thermistor::getTemp()
{
  return calculateTemp();
}

const int tempPin = A0; // analog pin
const int relayPin0 = 5;
const int relayPin1 = 6;

// PID parameters
double Kp = 1;
double Ki = 0.09;
double Kd = 0.001;
double setpoint = 80;

// Variables
double input, output;
double error, last_error, sum_error;
double dt = 0.1;  // Sample time in seconds

Thermistor thermistor(tempPin);

void setup()
{
  pinMode(relayPin0, OUTPUT);
  pinMode(relayPin1, OUTPUT);
  digitalWrite(relayPin0, HIGH);
  digitalWrite(relayPin1, HIGH);
  digitalWrite(relayPin0, LOW);
  Serial.begin(9600);
}

void loop() {
  // Read input from temperature sensor
  input = thermistor.getTemp();
  
  // Compute error
  error = setpoint - input;
  
  // Compute PID output
  output = Kp * error + Ki * sum_error * dt + Kd * (error - last_error) / dt;
  
  // Update error terms
  last_error = error;
  sum_error += error;
  
  // Update output
  if (output > 1) {
    digitalWrite(relayPin1, LOW);
  } else {
    digitalWrite(relayPin1, HIGH);
  }
  
  // Print values for debugging
  Serial.print("Temperature: ");
  Serial.print(input);
  Serial.print(", Output: ");
  Serial.print(output);
//  Serial.print(", Error: ");
//  Serial.print(error);
//  Serial.print(", Sum_error: ");
//  Serial.println(sum_error);
  Serial.println(setpoint);
  
  // Wait for sample time
  delay(dt*1000);
}
