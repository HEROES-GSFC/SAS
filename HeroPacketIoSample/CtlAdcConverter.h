/*
 * CtlAdcConverter.h
 *
 *  Created on: Aug 31, 2009
 *      Author: wclevela
 */

#ifndef CTLADCCONVERTER_H_
#define CTLADCCONVERTER_H_

#include <cmath>
#include <stdint.h>

static const float PI = 3.14159265f;
static const float SHAFT_OFFSET = 0.0f;


class CtlAdcConverter
{
public:
	CtlAdcConverter() {};

	virtual ~CtlAdcConverter() {};

	// Convert to volts.
	inline static float cnvVolts(int16_t sample);

	// The MUX'ed temperature readings
	inline static float cnvMuxCelsius(uint8_t addr, int16_t sample);

	// The FGG
	inline static float cnvFggAzXAxis(int16_t sample);
	inline static float cnvFggAzYAxis(int16_t sample);
	inline static float cnvFggElXAxis(int16_t sample);
	inline static float cnvFggElYAxis(int16_t sample);

	// The Inclinometer
	inline static float cnvInclPitch(int16_t sample);
	inline static float cnvInclRoll(int16_t sample);

	// The Shaft
	inline static float cnvShaftEncoder(int16_t sample);
	inline static float cnvShaftEm19Current(int16_t sample);
	inline static float cnvWheelEm19Current(int16_t sample);
	inline static float cnvElEm19Current(int16_t sample);
	inline static float cnvAzWheelTach(int16_t sample);

	// Misc Temperatures (That aren't Mux'ed)
	inline static float cnvLM61Tempr(int16_t sample);
	inline static float cnvLM50Tempr(int16_t sample);
	inline static float cnvPressure(int16_t sample);

  // The High-rate gyro.
  inline static float cnvHrgAzAxis(int16_t sample);

  inline static void setAzXTweak( float slopeScale, float offsetAdd );
  inline static void setAzYTweak( float slopeScale, float offsetAdd );
  inline static void setElXTweak( float slopeScale, float offsetAdd );
  inline static void setElYTweak( float slopeScale, float offsetAdd );

  inline static void getAzXTweak( float &slopeScale, float &offsetAdd );
  inline static void getAzYTweak( float &slopeScale, float &offsetAdd );
  inline static void getElXTweak( float &slopeScale, float &offsetAdd );
  inline static void getElYTweak( float &slopeScale, float &offsetAdd );

protected:
  static float   azXSlopeScale;
  static float   azXOffsetAddition;
  static float   azYSlopeScale;
  static float   azYOffsetAddition;
  static float   elXSlopeScale;
  static float   elXOffsetAddition;
  static float   elYSlopeScale;
  static float   elYOffsetAddition;

private:
};

float CtlAdcConverter::cnvVolts(int16_t sample) {
	return (float)(sample / 32768E0) * 10.0f;
}

float CtlAdcConverter::cnvMuxCelsius(uint8_t addr, int16_t sample )
{
	addr &= 0x0f;
	float yIntercepts[16] = {
			-55.0f, -55.0f, -55.0f, -55.0f, -55.0f, -55.0f, -55.0f, -55.0f,
			-55.0f, -55.0f, -55.0f, -55.0f, -55.0f, -55.0f, -55.0f, -55.0f
	};

	float volts = cnvVolts(sample);
	return 25.123f * volts + yIntercepts[addr];
}

void CtlAdcConverter::setAzXTweak( float slopeScale, float offsetAdd )
{
  azXSlopeScale = slopeScale;
  azXOffsetAddition = offsetAdd;
}

void CtlAdcConverter::setAzYTweak( float slopeScale, float offsetAdd )
{
  azYSlopeScale = slopeScale;
  azYOffsetAddition = offsetAdd;
}

void CtlAdcConverter::setElXTweak( float slopeScale, float offsetAdd )
{
  elXSlopeScale = slopeScale;
  elXOffsetAddition = offsetAdd;
}

void CtlAdcConverter::setElYTweak( float slopeScale, float offsetAdd )
{
  elYSlopeScale = slopeScale;
  elYOffsetAddition = offsetAdd;
}

void CtlAdcConverter::getAzXTweak( float &slopeScale, float &offsetAdd )
{
  slopeScale = azXSlopeScale;
  offsetAdd = azXOffsetAddition;
}

void CtlAdcConverter::getAzYTweak( float &slopeScale, float &offsetAdd )
{
  slopeScale = azYSlopeScale;
  offsetAdd = azYOffsetAddition;
}

void CtlAdcConverter::getElXTweak( float &slopeScale, float &offsetAdd )
{
  slopeScale = elXSlopeScale;
  offsetAdd = elXOffsetAddition;
}

void CtlAdcConverter::getElYTweak( float &slopeScale, float &offsetAdd )
{
  slopeScale = elYSlopeScale;
  offsetAdd = elYOffsetAddition;
}

float CtlAdcConverter::cnvFggAzXAxis(int16_t sample) {
//	return (float)(-6.1703E-5 * sample + 0.0129f);
	return (float)(6.2267E-5 * sample * azXSlopeScale + 0.0216 + azXOffsetAddition);
}

float CtlAdcConverter::cnvFggAzYAxis(int16_t sample) {
//	return (float)(-6.2985E-5 * sample + 0.0026f);
	return (float)(6.1415E-5 * sample * azYSlopeScale + 0.0343f + azYOffsetAddition);
}

float CtlAdcConverter::cnvFggElXAxis(int16_t sample) {
//	return (float)(-5.9903E-5 * sample + 0.0151f);
//	return (float)(6.2041E-5 * sample + 0.0502f);
	return (float)(6.2041E-5 * sample * elXSlopeScale - 0.0398f + elXOffsetAddition);
}
float CtlAdcConverter::cnvFggElYAxis(int16_t sample) {
	return (float)(6.3074E-5 * sample * elYSlopeScale + 0.0316f + elYOffsetAddition);
}
float CtlAdcConverter::cnvInclPitch(int16_t sample) {
//	float volts = cnvVolts(sample);
//	return (float)(asin(volts/19.979f) * 180.0f / PI);
  return (float)((8.715951 * sample)/3476.7);
}
float CtlAdcConverter::cnvInclRoll(int16_t sample) {
//	float volts = cnvVolts(sample);
//	return (float)(asin(volts/19.969f) * 180.0f / PI);
  return (float)((8.715951 * sample)/3476.7);
}
float CtlAdcConverter::cnvShaftEncoder(int16_t sample) {
	float volts = cnvVolts(sample);
	return (360.0f/8.2f) * volts + SHAFT_OFFSET;
}
float CtlAdcConverter::cnvShaftEm19Current(int16_t sample) {
	float volts = cnvVolts(sample);
//	return 1.4256f * volts - 0.050f;
  return 4.8984f * volts + 0.1f;
}
float CtlAdcConverter::cnvWheelEm19Current(int16_t sample) {
	float volts = cnvVolts(sample);
//	return 1.4256f * volts - 0.050f;
  return 4.8984f * volts + 0.1f;
}
float CtlAdcConverter::cnvElEm19Current(int16_t sample) {
	float volts = cnvVolts(sample);
//	return -1.4256f * volts + 0.009f;
  return 111.643f * volts + 0.3f;
}
float CtlAdcConverter::cnvAzWheelTach(int16_t sample) {
	float volts = cnvVolts(sample);
	return (0.9116f * volts) + 0.0052f;
}
float CtlAdcConverter::cnvLM61Tempr(int16_t sample) {
	float volts = cnvVolts(sample);
	return 50.0f * volts - 60.0f;
}
float CtlAdcConverter::cnvLM50Tempr(int16_t sample) {
	float volts = cnvVolts(sample);
	return 50.0f * volts - 50.0f;
}
float CtlAdcConverter::cnvPressure(int16_t sample) {
	float volts = cnvVolts(sample);
	return 7.2516f * volts + 1.4503f;
}

float CtlAdcConverter::cnvHrgAzAxis(int16_t sample)
{
  float volts = cnvVolts(sample);
  return 9.f * volts;
}

#endif /* CTL_DIAMONDMM_H_ */
