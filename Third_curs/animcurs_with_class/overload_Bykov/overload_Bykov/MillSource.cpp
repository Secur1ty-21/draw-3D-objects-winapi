#include "MillClass.h"

MillClass::MillClass()
{
}

MillClass::~MillClass()
{
}

void MillClass::checkPerimetr(CPoint *object, int numOfElements, double height, double width) {
	for (int i = 0; i < 13; i++)
	{
		if (object[i].x > width || object[i].x < 0)
		{
			if (object[i].x > width)
				g_znak_x = -1;
			if (object[i].x < 0)
				g_znak_x = 1;
			g_move_x += 4 * g_znak_x;
			break;
		}
		if (object[i].y > height || object[i].y < 0)
		{
			if (object[i].y > height)
				g_znak_y = -1;
			if (object[i].y < 0)
				g_znak_y = 1;
			g_move_y += 4 * g_znak_y;
			break;
		}
	}
}

CAffineMatrix operator * (const CAffineMatrix& m1, const CAffineMatrix& m2)
{
	CAffineMatrix mRes;
	ZeroMemory(&mRes, sizeof(mRes));
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++)
				mRes.m[i][j] += m1.m[i][k] * m2.m[k][j];

	return mRes;
}

CPoint operator * (const CPoint& p, const CAffineMatrix& m)
{
	CPoint pRes;
	pRes.x = p.x * m.m[0][0] + p.y * m.m[1][0] + m.m[2][0];
	pRes.y = p.x * m.m[0][1] + p.y * m.m[1][1] + m.m[2][1];

	return pRes;
}

CPoint *MillClass::Full_Movement(double g_alpha, double g_move_x, double g_move_y, double g_zoom, CPoint *object, int numberBasePoint, int numberOfElements) {
	CAffineMatrix rotation, move, zoom;
	ZeroMemory(&rotation, sizeof(rotation));
	ZeroMemory(&move, sizeof(move));
	ZeroMemory(&zoom, sizeof(zoom));
	rotation.m[0][0] = rotation.m[1][1] = cos(g_alpha);
	rotation.m[2][2] = 1.0;
	rotation.m[0][1] = sin(g_alpha);
	rotation.m[1][0] = -sin(g_alpha);
	move.m[0][0] = move.m[1][1] = move.m[2][2] = 1.0;
	move.m[2][0] = g_move_x;
	move.m[2][1] = g_move_y;
	zoom.m[0][0] = zoom.m[1][1] = g_zoom;
	zoom.m[2][2] = 1.0;
	CPoint center_lop = object[numberBasePoint];
	for (int i = 0; i < numberOfElements; i++) {
		if (i >= numberBasePoint) {
			object[i].x = object[i].x - center_lop.x;
			object[i].y = object[i].y - center_lop.y;
			object[i] = object[i] * rotation;
			object[i].x = center_lop.x + object[i].x;
			object[i].y = center_lop.y + object[i].y;
		}
		object[i] = object[i] * move * zoom;
	}
	return (object);
}