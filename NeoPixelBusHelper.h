#pragma once


#include "internal/RgbColor.h"


class NeoPixelBusHelper

{

public:

	
	/*virtual ~NeoPixelBusHelper(void)
	{


	};*/
	virtual uint16_t PixelCount() const;
	virtual  void SetPixelColor(uint16_t indexPixel, RgbColor color);
	virtual RgbColor GetPixelColor(uint16_t indexPixel) const;



};

