#include <stdio.h>
#include <stdlib.h>

#include "calc.h"

static void TestAllFuncs();

int main()
{
	TestAllFuncs();
	return 0;
}

static void TestAllFuncs()
{ 
    calc_status_t status = -1;
    double res = 0;
    char str1[100] = "7+8";              /* Result = 15 status = SUCCESS       */
    char str2[100] = "8+8*3+-2^5";         /* Result =  0 status = SUCCESS      */
    char str3[100] = "8+8*3-2^";         /* Result =  0 status = SYNTAX_ERROR */
    char str4[100] = "2/0";             /* Result =  0 status = MATH_ERROR         */
    char str5[100] = "8++8*((3-2)*5)";     /* Result = 48 status = SUCCESS       */
    char str6[100] = "3-2)*5";         /* Result =  0 status = SYNTAX_ERROR */
    char str7[100] = "(3-2)*5+ 5*(4+4+4";    /* Result =  0 status = SYNTAX_ERROR */
    char str8[100] = "((7*(2+5)+3))";
    char str9[100] = ") ";

    calc_t *calc = CalcCreate();
    
    status = Calculate(calc, str1, &res);
    printf("problem: %s\n", str1);
    printf ("ans: %f\n", res);
    printf ("status: %d\n\n", status);
    
    status = Calculate(calc, str2, &res);
    printf("problem: %s\n", str2);
    printf ("ans: %f\n", res);
    printf ("status: %d\n\n", status);
    
    status = Calculate(calc, str3, &res);
    printf("problem: %s\n", str3);
    printf ("ans: %f\n", res);
    printf ("status: %d\n\n", status);
    
    status = Calculate(calc, str4, &res);
    printf("problem: %s\n", str4);
    printf ("ans: %f\n", res);
    printf ("status: %d\n\n", status);

    status = Calculate(calc, str5, &res);
    printf("problem: %s\n", str5);
    printf ("ans: %f\n", res);
    printf ("status: %d\n\n", status);
    
    status = Calculate(calc, str6, &res);
    printf("problem: %s\n", str6);
    printf ("ans: %f\n", res);
    printf ("status: %d\n\n", status);
    
    status = Calculate(calc, str7, &res);
    printf("problem: %s\n", str7);
    printf ("ans: %f\n", res);
    printf ("status: %d\n\n", status);
    
    status = Calculate(calc, str8, &res);
    printf("problem: %s\n", str8);
    printf ("ans: %f\n", res);
    printf ("status: %d\n\n", status);

	status = Calculate(calc, str9, &res);
    printf("problem: %s\n", str9);
    printf ("ans: %f\n", res);
    printf ("status: %d\n\n", status);    
    CalcDestroy(calc);
}

