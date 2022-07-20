/*=========================== LIBRARIES & MACROS ============================*/

#include <stdlib.h> /* malloc, free, strtod */
#include <assert.h> /* assert */
#include <ctype.h> /* isdigit */
#include <math.h> /* pow */

#include "stack.h"
#include "calc.h"

#define MATH_ERR 0xDEADBEEF
#define NUM_OF_OPERATORS 128
#define STACK_CAPACITY 30

/*============================== DECLARATIONS ===============================*/

typedef enum state
{
	WAIT_FOR_NUM_STATE,
	WAIT_FOR_OP_STATE,
	ERROR_STATE,
	LAST_STATE
}state_t;

typedef enum event
{
	RECEIVE_PLUS_EVENT = '+',
	RECEIVE_MINUS_EVENT = '-',
	RECEIVE_MULT_EVENT = '*',
	RECEIVE_DIV_EVENT = '/',
	RECEIVE_POW_EVENT = '^',
	RECEIVE_CLOSE_PAR_EVENT = ')' ,
	RECEIVE_OPEN_PAR_EVENT = '(',
	RECIEVE_SPACE_EVENT = ' ',
	END_OF_STRING_EVENT = '\0'
} event_t;

typedef enum op_priority
{
	PLUS = 1,
	MINUS = 1,
	MUL = 2, 
	DIV = 2,
	POW = 3
}op_priority_t; 

typedef state_t(*EventHandler[LAST_STATE][NUM_OF_OPERATORS])(calc_t*, char**, double*);
typedef double(*op_action_t)(double, double);

typedef struct operator
{
	op_action_t op_func;
	op_priority_t op_priority;
}operator_t;

/* helper funcs */
static double CalculateSubStr(calc_t*);
static calc_status_t GetStatusFromCurrState(calc_t*, double*);
static void EmptyCalcStacks(calc_t*, double*);

/* handler funcs */
static state_t NumHandler(calc_t*, char**, double*);
static state_t OpHandler(calc_t*, char**, double*);
static state_t OpenParHandler(calc_t*, char**, double*);
static state_t CloseParHandler(calc_t*, char**, double*);
static state_t SpaceHandler(calc_t*, char**, double*);
static state_t ErrorHandler(calc_t*, char**, double*);
static state_t EndOfStringHandler(calc_t*, char**, double*);

/* LUT funcs */
static void InitLut(EventHandler);
static void InitOpLut(operator_t*);

/* math funcs */
static double Multiplication(double, double);
static double Division(double, double);
static double Sum(double, double);
static double Subtract(double, double);
static double Power(double, double);


struct calc
{
	state_t curr_state;
	stack_t *num_stack;
	stack_t *op_stack;
	EventHandler event_lut;
	operator_t op_lut[NUM_OF_OPERATORS];
};

/*========================== FUNCTION DEFINITIONS ===========================*/

calc_t *CalcCreate()
{
	calc_t *calc = (calc_t *)malloc(sizeof(calc_t));
	if (NULL == calc)
	{
		return (NULL);
	}
	calc->num_stack = StackCreate(STACK_CAPACITY, sizeof(double));
	if(NULL == calc->num_stack)
	{
		free(calc);
		return (NULL);
	}
	calc->op_stack = StackCreate(STACK_CAPACITY, sizeof(char));
	if (NULL == calc->op_stack)
	{
		free(calc->num_stack);
		free(calc);
		return (NULL);
	}

	calc->curr_state = WAIT_FOR_NUM_STATE;
	InitLut(calc->event_lut);
	InitOpLut(calc->op_lut);

	return (calc);
}

void CalcDestroy(calc_t *calc)
{
	assert (NULL != calc);
	StackDestroy(calc->num_stack);
	StackDestroy(calc->op_stack);
	free(calc);
}

calc_status_t Calculate(calc_t *calc, const char *expression, double *res)
{
	assert(NULL != calc);
	assert(NULL != expression);
	assert(NULL != res);

	calc->curr_state = WAIT_FOR_NUM_STATE;

	while (ERROR_STATE != calc->curr_state && LAST_STATE != calc->curr_state)
	{ 
		calc->curr_state = 
		(*calc->event_lut[calc->curr_state][(int)*expression])
		(calc, (char **)&expression, res);
	}

	EmptyCalcStacks(calc, res);
	
	return (GetStatusFromCurrState(calc, res));
}

/* ====== HELPER FUNCS ====== */

static double CalculateSubStr(calc_t *calc)
{
	double num1 = 0, num2 = 0, ans = 0;
	unsigned char op = ' ';

	op = *(char *)StackPop(calc->op_stack);
	num2 = *(double *)StackPop(calc->num_stack);
	num1 = *(double *)StackPop(calc->num_stack);
	ans = calc->op_lut[op].op_func(num1, num2);
	StackPush(calc->num_stack, &ans);
	return (ans);
}

static calc_status_t GetStatusFromCurrState(calc_t *calc, double* res)
{
	if (ERROR_STATE == calc->curr_state)
	{
		if (MATH_ERR == *res)
		{
			*res = 0;
		    return (MATH_ERROR);   
		}
		*res = 0;
		return (SYNTAX_ERROR);
	}
	else
	{
		return (SUCCESS);
	}
}

static void EmptyCalcStacks(calc_t *calc, double *res)
{
	if(!StackIsEmpty(calc->num_stack))
	{
		*res = *(double *)StackPop(calc->num_stack);
	}
	while(!StackIsEmpty(calc->num_stack))
	{
		StackPop(calc->num_stack);
	}
	while(!StackIsEmpty(calc->op_stack))
	{
		StackPop(calc->op_stack);
	}
}

/* ====== HANDLER FUNCS ====== */

static state_t NumHandler(calc_t *calc, char **expression, double *res)
{
	double num = strtod(*expression, expression);
	(void)res;
	
	StackPush(calc->num_stack, &num);
	return (WAIT_FOR_OP_STATE);
}

static state_t OpHandler(calc_t *calc, char **expression, double *res)
{
	(void)res;

	while ((!StackIsEmpty(calc->op_stack)) && 
			(calc->op_lut[(int)(**expression)].op_priority <= 
			calc->op_lut[(int)(*(char *)StackPeek(calc->op_stack))].op_priority))
	{ 
		if(CalculateSubStr(calc) == MATH_ERR)
		{
			return (ERROR_STATE);
		}
	}

	StackPush(calc->op_stack, *expression);
	++(*expression);
	return (WAIT_FOR_NUM_STATE);
}


static state_t OpenParHandler(calc_t *calc, char **expression, double *res)
{
	(void)res;

	StackPush(calc->op_stack, *expression);
	++(*expression);
	return (WAIT_FOR_NUM_STATE);
}

static state_t CloseParHandler(calc_t *calc, char **expression, double *res)
{
	(void)res;

	while ((!StackIsEmpty(calc->op_stack)) && 
		    *(char *)StackPeek(calc->op_stack) != '(')
	{
		if(CalculateSubStr(calc) == MATH_ERR)
		{
		    return (ERROR_STATE);
		}
	}

	if (StackIsEmpty(calc->op_stack))
	{
		return (ERROR_STATE);
	}

	StackPop(calc->op_stack);
	++(*expression);
	return (WAIT_FOR_OP_STATE);
}

static state_t SpaceHandler(calc_t *calc, char **expression, double *res)
{
	(void)res;
	while (' ' == **expression)
	{
		++(*expression);
	}
	
	return(calc->curr_state);
}

static state_t ErrorHandler(calc_t *calc, char **expression, double *res)
{
    (void)calc;
    (void)expression;
    (void)res;
    return (ERROR_STATE);   
}

static state_t EndOfStringHandler(calc_t *calc, char **expression, double *res)
{
	(void)expression;
	(void)res;

	if (1 != (StackGetSize(calc->num_stack) - StackGetSize(calc->op_stack)))
	{
		return (ERROR_STATE);   
	}
	while (1 < StackGetSize(calc->num_stack))
	{
		if(CalculateSubStr(calc) == MATH_ERR)
		{
		    return (ERROR_STATE);
		}
	}
	return (LAST_STATE);
}

/* ====== LUT FUNCS ====== */

static void InitLut(EventHandler event_lut)
{
	size_t i = 0, j = 0;

	for (i = 0; i < LAST_STATE; ++i)
	{
		for (j = 0; j < NUM_OF_OPERATORS; ++j)
		{
			event_lut[i][j] = ErrorHandler;
		}
	}

	for (i = 0; i < 10; ++i)
	{
		event_lut[WAIT_FOR_NUM_STATE][48 + i] = NumHandler;
	}

	event_lut[WAIT_FOR_NUM_STATE][RECEIVE_PLUS_EVENT] = NumHandler;
	event_lut[WAIT_FOR_NUM_STATE][RECEIVE_MINUS_EVENT] = NumHandler;
	event_lut[WAIT_FOR_NUM_STATE][RECEIVE_OPEN_PAR_EVENT] = OpenParHandler;
	event_lut[WAIT_FOR_NUM_STATE][RECIEVE_SPACE_EVENT] = SpaceHandler;
	event_lut[WAIT_FOR_NUM_STATE][END_OF_STRING_EVENT] = EndOfStringHandler;

	event_lut[WAIT_FOR_OP_STATE][RECEIVE_PLUS_EVENT] = OpHandler;
	event_lut[WAIT_FOR_OP_STATE][RECEIVE_MINUS_EVENT] = OpHandler;
	event_lut[WAIT_FOR_OP_STATE][RECEIVE_MULT_EVENT] = OpHandler;
	event_lut[WAIT_FOR_OP_STATE][RECEIVE_DIV_EVENT] = OpHandler;
	event_lut[WAIT_FOR_OP_STATE][RECEIVE_POW_EVENT] = OpHandler;
	event_lut[WAIT_FOR_OP_STATE][RECEIVE_OPEN_PAR_EVENT] = OpenParHandler;
	event_lut[WAIT_FOR_OP_STATE][RECEIVE_CLOSE_PAR_EVENT] = CloseParHandler;
	event_lut[WAIT_FOR_OP_STATE][RECIEVE_SPACE_EVENT] = SpaceHandler;
	event_lut[WAIT_FOR_OP_STATE][END_OF_STRING_EVENT] = EndOfStringHandler;
}

static void InitOpLut(operator_t *op_lut)
{ 
	int i = 0;
	for(i = 0; i < NUM_OF_OPERATORS; ++i)
	{
		op_lut[i].op_func = NULL;
		op_lut[i].op_priority = 0;
	}

	op_lut['+'].op_func = Sum;
	op_lut['+'].op_priority = PLUS;

	op_lut['-'].op_func = Subtract;
	op_lut['-'].op_priority = MINUS;

	op_lut['*'].op_func = Multiplication;
	op_lut['*'].op_priority = MUL;

	op_lut['/'].op_func = Division;
	op_lut['/'].op_priority = DIV;

	op_lut['^'].op_func = Power;
	op_lut['^'].op_priority = POW;
}

/* ====== MATH FUNCS ====== */

static double Multiplication(double num1, double num2)
{
	return (num1 * num2);
}

static double Division(double num1, double num2)
{
	return ((0 == num2) ? (MATH_ERR) : (num1 / num2));
}

static double Sum(double num1, double num2)
{
	return (num1 + num2);
}

static double Subtract(double num1, double num2)
{
	return (num1 - num2);
}

static double Power(double base, double exp)
{
	return(pow(base, exp));
}


