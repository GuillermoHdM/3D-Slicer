#include <algorithm>
#include "OutputFormatting.h"

void RGB565::SetR(uint8_t red)
{
	red = std::min(red, uint8_t(31));
	Value = (Value & 0x07FF) | (red << 11);
}
void RGB565::SetG(uint8_t green)//green has an extra bit!
{
	green = std::min(green, uint8_t(63));
	Value = (Value & 0xF81F) | (green << 5);
}
void RGB565::SetB(uint8_t blue)
{
	blue = std::min(blue, uint8_t(31));
	Value = (Value & 0xFFE0) | blue;
}
uint16_t RGB565::GetValue() const
{
	return Value;
}