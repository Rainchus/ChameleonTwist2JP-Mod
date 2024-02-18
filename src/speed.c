#include "ct2.h"

extern float _sqrtf(float value);

#define SQ(x) (x)*(x)

float CalculateDistance(Vec3f* p1, Vec3f* p2) {
    return _sqrtf(SQ(p2->x - p1->x) + SQ(p2->z - p1->z));
}
