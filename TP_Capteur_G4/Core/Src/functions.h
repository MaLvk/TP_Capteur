#include "main.h"
#include "Const.h"
#include <stdio.h>
#include "math.h"

void Init(I2C_HandleTypeDef* );
void Measure_T(I2C_HandleTypeDef*,double*);
void Measure_A(I2C_HandleTypeDef*,double* );
