// EhkAdcConverter.h: interface for the EhkAdcConverter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EHKADCCONVERTER_H__7E406193_DC51_4766_A83F_0794B568AD02__INCLUDED_)
#define AFX_EHKADCCONVERTER_H__7E406193_DC51_4766_A83F_0794B568AD02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdint.h>

class EhkAdcConverter  
{
public:
  EhkAdcConverter() {};
  virtual ~EhkAdcConverter() {};

	// Misc Temperatures (That aren't Mux'ed)
	inline static float cnvVolts(int16_t sample);
	inline static float cnvLM61Tempr(int16_t sample);
	inline static float cnvLM50Tempr(int16_t sample);
	inline static float cnvPressure(int16_t sample);
  inline static float cnvInampTemp( int16_t sample );
  inline static float cnvMuxTemp( int muxId, int16_t sample );
};

float EhkAdcConverter::cnvVolts( int16_t sample )
{
  return (float)sample * 10.f / 32768.f;
}

float EhkAdcConverter::cnvLM50Tempr(int16_t sample)
{
	float volts = cnvVolts(sample);
	return 50.0f * volts - 50.0f;
}

float EhkAdcConverter::cnvLM61Tempr(int16_t sample)
{
	float volts = cnvVolts(sample);
	return 50.0f * volts - 60.0f;
}

float EhkAdcConverter::cnvPressure(int16_t sample)
{
	float volts = cnvVolts(sample);
	return 7.2516f * volts + 1.4503f;
}

float EhkAdcConverter::cnvInampTemp( int16_t sample )
{
  return -100.f * cnvVolts(sample) - 60.f;
}

float EhkAdcConverter::cnvMuxTemp( int muxId, int16_t sample )
{
  if ( muxId < 0 || muxId > 15 )
    return -9999.f;

 	// Mux temperature conversion factors (3rd order polynomial fit).
	double TCvt[16][4] = {
		{2.6455E-11, -6.8967E-07, 1.2988E-02, -68.590+2.5},
		{6.4505E-13, -1.6819E-08, 7.9958E-03, -61.100+0.7},
		{3.6382E-11, -8.5089E-07, 1.3568E-02, -67.326+4.9},
		{1.1131E-12, -3.4799E-08, 8.2028E-03, -63.399+8.5},
		{2.5306E-12, -7.8044E-08, 8.5779E-03, -61.997+6.5},
		{3.3271E-12, -1.0838E-07, 8.8455E-03, -64.075+1.3},
		{1.7255E-11, -4.9241E-07, 1.1910E-02, -68.901+1.7},
		{6.5980E-13, -2.2015E-08, 7.9905E-03, -59.132+0.9},
		{1.0320E-11, -3.2299E-07, 1.0697E-02, -70.039+10.0},
		{1.0169E-11, -3.0676E-07, 1.0478E-02, -66.511+0.0},
		{2.5583E-11, -6.7282E-07, 1.2811E-02, -68.162+0.4},
		{2.8057E-11, -7.6845E-07, 1.3840E-02, -74.207+7.6},
		{2.0029E-11, -5.7139E-07, 1.2453E-02, -71.008+8.8},
		{4.9364E-12, -1.5294E-07, 9.2122E-03, -62.809+6.0},
		{1.2135E-11, -3.6494E-07, 1.0985E-02, -68.377+8.5},
		{5.5305E-12, -1.7506E-07, 9.3890E-03, -64.355+8.0}};

	// Convert MUX temperatures.
  double dval = (double)sample;
	return (float)((((TCvt[muxId][0] * dval + TCvt[muxId][1]) * dval) + TCvt[muxId][2])
    * dval + TCvt[muxId][3]);
}

#endif // !defined(AFX_EHKADCCONVERTER_H__7E406193_DC51_4766_A83F_0794B568AD02__INCLUDED_)
