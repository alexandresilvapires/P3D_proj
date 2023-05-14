#ifndef COLOR_H
#define COLOR_H

#include "color.h"
#include "vector.h"

Vector Color::to_vec() const {
    return Vector(R, G, B);
}

#endif