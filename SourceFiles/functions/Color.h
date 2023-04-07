#pragma once

class Color
{
private:
	float val_ = 1.0f;
public:
	float operator=(float val);
	void operator+=(float val) { operator=(val_ + val); }
	void operator-=(float val) { operator=(val_ - val); }
	operator float(){ return val_; }
};

struct ColorRGBA
{
public:
	Color r, g, b, a;
	ColorRGBA() {}
	ColorRGBA(float red, float green, float blue, float alpha = 1.0f) { r = red, g = green, b = blue, a = alpha; }
};

struct ColorRGB
{
public:
	Color r, g, b;
	ColorRGB() {}
	ColorRGB(float red, float green, float blue) { r = red, g = green, b = blue; }
};