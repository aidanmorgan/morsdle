//
// Created by aidan on 20/04/2022.
//

#ifndef ALLOCATIONS_H
#define ALLOCATIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INLINE_MALLOC(T, ...)       \
  memcpy(malloc(sizeof(T)),         \
         &(T const){ __VA_ARGS__ }, \
         sizeof(T))

#endif //ALLOCATIONS_H
