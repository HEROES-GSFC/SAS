/*
 * AspAdcConverter.h
 *
 *  Created on: Sep 14, 2009
 *      Author: kdietz
 */

#ifndef ASPADCCONVERTER_H_
#define ASPADCCONVERTER_H_

class AspAdcConverter
{
public:
	AspAdcConverter() {};
	virtual ~AspAdcConverter() {};

	// Misc Temperatures (That aren't Mux'ed)
	inline static float cnvVolts(int16_t sample);
	inline static float cnvLM61Tempr(int16_t sample);
	inline static float cnvLM50Tempr(int16_t sample);
	inline static float cnvPressure(int16_t sample);
	inline static float cnvLM61BTempr(int16_t sample);
	inline static float cnvFocusPos(int16_t sample);

};

float AspAdcConverter::cnvVolts(int16_t sample)
{
	return (sample / 4096.0f) * 10.0f;
}

float AspAdcConverter::cnvLM61Tempr(int16_t sample)
{
	float volts = cnvVolts(sample);
	return 50.0f * volts - 60.0f;
}
float AspAdcConverter::cnvLM50Tempr(int16_t sample)
{
	float volts = cnvVolts(sample);
	return 50.0f * volts - 50.0f;
}

float AspAdcConverter::cnvLM61BTempr(int16_t sample)
{
	return 58.0f * cnvVolts(sample) - 69.0f;
}

float AspAdcConverter::cnvPressure(int16_t sample)
{
	float volts = cnvVolts(sample);
	return 7.2516f * volts + 1.4503f;
}

float AspAdcConverter::cnvFocusPos(int16_t sample)
{
	return -1259.857f * cnvVolts(sample) + 10438.849f;
}

#endif /* ASPADCCONVERTER_H_ */
