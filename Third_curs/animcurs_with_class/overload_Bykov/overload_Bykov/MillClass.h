#pragma once
#ifndef MILLCLASS_H
#define MILLCLASS_H
#define _USE_MATH_DEFINES
#include <math.h>
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>

struct CPoint {
	double x, y;
};
struct CAffineMatrix
{
	double m[3][3];
};

class MillClass
{
public:
	MillClass();
	~MillClass();
	double g_alpha = 1 * M_PI / 180.0;
	double g_move_x = 0.0;
	double g_move_y = 0.0;
	double g_zoom = 1.0;
	double g_znak = 1;
	double g_znak_x = 1;
	double g_znak_y = 1;
	void checkPerimetr(CPoint *object, int numOfElements, double height, double width);
	CPoint *Full_Movement(double g_alpha, double g_move_x, double g_move_y, double g_zoom, CPoint *object, int numberBasePoint, int numberOfElements);
private:

};

#endif