#include <Arduino.h>

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
  double m_knownR = 1000;

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