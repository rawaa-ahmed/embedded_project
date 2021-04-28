/**
 * @file BMP.h
 * @author Rawaa Ahmed (riristar519@gmail.com)
 * @brief This is a BMP library To read Temperature and Pressure from BMP180 sensor
 * @version 0.1
 * @date 2021-04-28
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <Wire.h>

/**
 * @brief Declaring ADDRESSES of Calibration Coefficients
 * 
 */
static short _AC1 = 0xAA;
static short _AC2 = 0xAC;
static short _AC3 = 0xAE;
static short _AC4 = 0xB0;
static short _AC5 = 0xB2;
static short _AC6 = 0xB4;
static short _B1 = 0xB6;
static short _B2 = 0xB8;
static short _MB = 0xBA;
static short _MC = 0xBC;
static short _MD = 0xBE;
static short BMP = 0x77;

/**
 * @brief Variables of Calibration Coefficients
 * 
 */
static short ac1, ac2, ac3, b1, b2, mb, mc, md;
static unsigned short ac4, ac5, ac6;
static long UT, UP, T, P, x1, x2, x3, b3, b5, b6;
static unsigned long b4, b7;
static short OSS = 0;

/**
 * @brief This function is used to read data from the slave
 * 
 * @param slv  The address of the slave
 * @param reg  The address of the register
 * @param n    Number of nibles
 * @return long 
 */
long Read_Data(int slv, int reg, int n)
{

  long val = 0;
  /**
   * @brief Wire.beginTransmission(slv) determines which slave to communicate with
   * 
   */
  Wire.beginTransmission(slv);
  /**
   * @brief Wire.write(reg) determines the register to read data from
   * 
   */
  Wire.write(reg);
  /**
   * @brief Wire.requestFrom(slv, n) determines number of nibles n,I want from slave
   * 
   */
  Wire.endTransmission();
  /**
   * @brief Wire.read() returns a nible of data
   * 
   */
  Wire.requestFrom(slv, n);
  if (Wire.available() == n)
  {
    val = Wire.read();
    for (int i = 1; i < n; i++)
    {
      val <<= 8;
      val |= Wire.read();
    }
  }
  return val;
}
long Temperature()
{
  return T;
}
long Pressure()
{
  return P;
}

/**
 * @brief Make necessary initializations
 * 
 */
void Initialize()
{
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(baudrate);
  //Reading Calibration Coefficients

  ac1 = Read_Data(BMP, _AC1, 2);
  ac2 = Read_Data(BMP, _AC2, 2);
  ac3 = Read_Data(BMP, _AC3, 2);
  ac4 = Read_Data(BMP, _AC4, 2);
  ac5 = Read_Data(BMP, _AC5, 2);
  ac6 = Read_Data(BMP, _AC6, 2);
  b1 = Read_Data(BMP, _B1, 2);
  b2 = Read_Data(BMP, _B2, 2);
  mb = Read_Data(BMP, _MB, 2);
  mc = Read_Data(BMP, _MC, 2);
  md = Read_Data(BMP, _MD, 2);
}

/**
 * @brief Compute the temperature and pressure
 * 
 */
void Compute()
{

  Wire.beginTransmission(BMP);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();
  delayMicroseconds(4500);

  UT = Read_Data(BMP, 0xF6, 2);

  Wire.beginTransmission(BMP);
  Wire.write(0xF4);
  Wire.write(0x34 | (OSS << 6));
  Wire.endTransmission();
  delayMicroseconds(4500);

  UP = Read_Data(BMP, 0xF6, 3) >> (8 - OSS);

  /**
 * @brief Calculate true temperature
 * 
 */
  x1 = (UT - ac6) * ac5 / pow(2, 15);
  x2 = mc * pow(2, 11) / (x1 + md);
  b5 = x1 + x2;
  T = (b5 + 8) / 16;

  /**
 * @brief Calculate true Pressure
 * 
 */
  b6 = b5 - 4000;
  x1 = (b2 * (b6 * b6 / pow(2, 12))) / pow(2, 11);
  x2 = ac2 * b6 / pow(2, 11);
  x3 = x1 + x2;
  b3 = (((ac1 * 4 + x3) << OSS) + 2) / 4;
  x1 = ac3 * b6 / pow(2, 13);
  x3 = ((x1 + x2) + 2) / 4;
  b4 = ac4 * (unsigned long)(x3 + 32768) / pow(2, 15);
  b7 = ((unsigned long)UP - b3) * (50000 >> OSS);
  if (b7 < 0x80000000)
  {
    P = (b7 * 2) / b4;
  }
  else
    P = (b7 / b4) * 2;
  x1 = (P / 256) * (P / 256);
  x1 = x1 * 3038 / pow(2, 16);
  x2 = -7357 * P / pow(2, 16);
  P += (x1 + x2 + 3791) / 16;
}
