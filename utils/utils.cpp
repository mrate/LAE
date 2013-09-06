#include "utils.h"
#include <cmath>

namespace LAE {
//bool lineIntersect(const SPoint l1[],const SPoint l2[], int& x, int& y)
//{
//	int A1 = l1[1].y - l1[0].y;
//	int B1 = l1[0].x - l1[1].x;
//	int C1 = A1*l1[0].x + B1*l1[0].y;
//	int A2 = l2[1].y - l2[0].y;
//	int B2 = l2[0].x - l2[1].x;
//	int C2 = A2*l2[0].x + B2*l2[0].y;
//	double det = A1*B2 - A2*B1;

//	if(det == 0) return false;

//	x = (B2*C1 - B1*C2)/det;
//	y = (A1*C2 - A2*C1)/det;
//	return (
//		std::min(l1[0].x, l1[1].x) <= x
//			&& x <= std::max(l1[0].x, l1[1].x)
//			&& std::min(l1[0].y, l1[1].y) <= y
//			&& y <= std::max(l1[0].y, l1[1].y)

//			&& std::min(l2[0].x, l2[1].x) <= x
//			&& x <= std::max(l2[0].x, l2[1].x)
//			&& std::min(l2[0].y, l2[1].y) <= y
//			&& y <= std::max(l2[0].y, l2[1].y)
//		);
//}

//bool pointInBox(const SBox& box, int x, int y)
//{
//	int ix, iy;
//	SPoint l1[2], l2[2];

//	l1[0].x = x;
//	l1[0].y = y;
//	l1[1].x = box.center.x;
//	l1[1].y = box.center.y;
//	for(int i=0;i<4;i++)
//	{
//		l2[0].x = box.p[i].x;
//		l2[0].y = box.p[i].y;
//		l2[1].x = box.p[(i+1)%4].x;
//		l2[1].y = box.p[(i+1)%4].y;

//		if(lineIntersect(l1, l2, ix, iy))
//			return false;
//	}
//	return true;
//}
}
