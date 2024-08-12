#include <Wire.h>
#include <math.h>  // 确保包含数学库以使用log函数

const int sensorPin = A0;  // LMV321输出连接的模拟输入引脚
const float knownResistor = 10000;  // 电压分压电路中的已知电阻值
const float beta = 3950;  // NTC的贝塔值
const float nominalResistance = 10000;  // NTC在25°C时的标称阻值
const float nominalTemperature = 25;  // 标称温度

float steinhart;  // 用于存储计算的温度

void setup() {
  Wire.begin(8);  // 设置I2C地址为8
  Wire.onRequest(requestEvent);  // 当树莓派请求数据时调用
  Serial.begin(9600);  // 初始化串口通信
}

void loop() {
  int adcValue = analogRead(sensorPin);
  float voltage = 5.0 - adcValue * (5.0 / 1023.0);  // 将ADC值转换为电压
  
  // 计算NTC的实际阻值
  float ntcResistance = knownResistor * (5.0 / voltage - 1);
  
  // 使用Steinhart-Hart方程计算温度
  steinhart = ntcResistance / nominalResistance;
  steinhart = log(steinhart);
  steinhart /= beta;
  steinhart += 1.0 / (nominalTemperature + 273.15);
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;  // 转换为摄氏温度

  // 打印温度结果
  Serial.print("Temperature: ");
  Serial.print(steinhart);
  Serial.println(" °C");

  delay(1000);  // 等待1秒
}

void requestEvent() {
  // 将温度数据发送到I2C主设备（树莓派）
  Wire.write((byte*)&steinhart, sizeof(steinhart));
}
