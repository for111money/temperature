#include <Wire.h>
#include <math.h>  // 包含数学库以使用 log 函数

const int sensorPin = A0;  // LMV321 输出连接的模拟输入引脚
const float knownResistor = 10000;  // 电压分压电路中的已知电阻值
const float beta = 3950;  // NTC 的贝塔值
const float nominalResistance = 10000;  // NTC 在 25°C 时的标称阻值
const float nominalTemperature = 25;  // 标称温度（摄氏）

float steinhart;  // 用于存储计算的温度

// 存储 A0 口采集的数据
const int sampleCount = 100;  // 采样数量
int adcValues[sampleCount];  // 存储采集的 ADC 值

void setup() {
  Wire.begin(8);  // 设置 I2C 地址为 8
  Wire.onRequest(requestEvent);  // 当树莓派请求数据时调用
  Serial.begin(9600);  // 初始化串口通信
}

void loop() {
  // 1秒内采集数据
  for (int i = 0; i < sampleCount; i++) {
    adcValues[i] = analogRead(sensorPin);  // 读取 ADC 值
    delay(10);  // 等待 10ms 进行下一次采样
  }

  // 处理数据：去掉最大值和最小值
  int minVal = adcValues[0];
  int maxVal = adcValues[0];
  long sum = 0;

  // 查找最大值和最小值
  for (int i = 0; i < sampleCount; i++) {
    if (adcValues[i] < minVal) {
      minVal = adcValues[i];
    }
    if (adcValues[i] > maxVal) {
      maxVal = adcValues[i];
    }
    sum += adcValues[i];
  }

  // 计算去掉最大值和最小值后的平均值
  sum -= minVal;  // 去掉最小值
  sum -= maxVal;  // 去掉最大值
  int averageValue = sum / (sampleCount - 2);  // 平均值（去除最大值和最小值后的平均）

  // 将平均值作为新的 adcValue
  float voltage =5- averageValue * (5.0 / 1023.0);  // 将 ADC 值转换为电压（假设 5V 参考电压）
  
  // 计算 NTC 的实际阻值
  float ntcResistance = knownResistor * (5.0 / voltage - 1);

  // 使用 Steinhart-Hart 方程计算温度
  steinhart = ntcResistance / nominalResistance;
  steinhart = log(steinhart);  // 取对数
  steinhart /= beta;  // 除以贝塔值
  steinhart += 1.0 / (nominalTemperature + 273.15);  // 加上标称温度（开尔文）的倒数
  steinhart = 1.0 / steinhart;  // 求倒数，得到绝对温度
  steinhart -= 273.15;  // 转换为摄氏温度

  // 打印温度结果
  Serial.print("Temperature: ");
  Serial.print(steinhart);
  Serial.println(" °C");

  delay(1000);  // 等待 1 秒
}

void requestEvent() {
  // 将温度数据发送到 I2C 主设备（树莓派）
  Wire.write((byte*)&steinhart, sizeof(steinhart));
}
