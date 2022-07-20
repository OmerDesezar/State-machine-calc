#ifndef __CALC_H__
#define __CALC_H__

typedef enum
{
    SUCCESS,
    MATH_ERROR,
    SYNTAX_ERROR
} calc_status_t;

typedef struct calc calc_t;

calc_t *CalcCreate();

void CalcDestroy(calc_t *calc);

calc_status_t Calculate(calc_t *calc, const char *expression, double *res);

#endif /*__CALC_H__*/
