#include "Color.h"

float Color::operator=(float val)
{
	val_ = val;
	if (val_ > 1.0f) { val_ -= (int)val_; }
	if (val_ < 0.0f) { val_ += 1.0f + (int)val_; }
	return val_;
}