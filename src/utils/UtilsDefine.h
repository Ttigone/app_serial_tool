#pragma once

#define EPSILON 0.000001

#define FLOAT_LESS(a,b) ((a) < (b-EPSILON))
#define FLOAT_LESS_EQUAL(a,b) ((a) < (b+EPSILON))

#define FLOAT_GREATER(a,b) ((a) > (b+EPSILON))
#define FLOAT_GREATER_EQUAL(a,b) ((a) > (b-EPSILON))
