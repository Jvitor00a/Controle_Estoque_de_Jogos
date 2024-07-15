#ifndef MACRO_KIT_H
#define MACRO_KIT_H

/* macro_kit.h */

#define _CAT2(x, y) x##y

// Concatenate 2 tokens x and y
#define CAT2(x, y) _CAT2(x, y)
// Concatenate 3 tokens x, y and z
#define CAT3(x, y, z) CAT2(x, CAT2(y, z))

// Join 2 tokens x and y with '_' = x_y
#define JOIN2(x, y) CAT3(x, _, y)
// Join 3 tokens x, y and z with '_' = x_y_z
#define JOIN3(x, y, z) JOIN2(x, JOIN2(y, z))
// Join 2 tokens x and y with '_' = x_y
#define JOIN_SEP2(x, y, sep) CAT3(x, sep, y)
// Join 3 tokens x, y and z with '_' = x_y_z
#define JOIN_SEP3(x, y, z, sep) JOIN_SEP2(x, JOIN_SEP2(y, z, sep), sep)
// Compute the memory footprint of n T's
#define SPAN(n, T) ((n) * sizeof(T))

#endif
