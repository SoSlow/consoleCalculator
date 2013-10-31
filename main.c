#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

enum
{
	LT_NUMBER,
	LT_OPERATOR,
	LT_NONE
} LexType;

enum
{
	ANNERR_NONE,
	ANERR_SYNTAX,
	ANERR_DIVBYZERO,
	ANERR_BRACKETS,
	ANERR_NUM
} AnalError;


char token[256];
char *expr;
size_t i = 0;
size_t tokenType;

size_t getToken()
{
	size_t len = strlen(expr);
	size_t j = 0;
	size_t res = LT_NONE;

	if(expr[i] == '\0')
		return 0;

	while(isspace(expr[i]))
		++i;

	// number
	if(isdigit(expr[i]) || expr[i] == '.')
	{
		do
		{
			token[j] = expr[i];
			++i; ++j;
		}
		while(isdigit(expr[i]) || expr[i] == '.');
		token[j] = '\0';
		res = LT_NUMBER;
	}
	else //operator
	if(strchr("+-*/^()", expr[i]))
	{
		token[0] = expr[i];
		token[1] = '\0';
		res = LT_OPERATOR;
		++i;
	}
	else
	{
		printf("Error parsing first symbol\n");
	}

	tokenType = res;
	return res;
}


size_t proccessAtom(double *answ)
{
	char op;
	double t = 0;

	if(tokenType == LT_NUMBER)
	{
		*answ = atof(token);
		getToken();
	}
	else
		return ANERR_SYNTAX;
	return 0;
}


size_t proccessBrackets(double *answ)
{
	size_t r;
	char op = *token;

	if(op == '(')
	{
		getToken();
		if(r = proccessSum(answ))
			return r;
		op = *token;
		if(op != ')')
			return ANERR_BRACKETS;
		getToken();
	}
	else 
	if(r = proccessAtom(answ))
		return r;
	
	return 0;
}

size_t proccessUnary(double *answ)
{
	char op;
	size_t r;
	double t = 0;
	
	op = *token;
	if(op == '+' || op == '-')
	{
		getToken();
	}
	if(r = proccessBrackets(answ))
		return r;

	if(op == '-')
		(*answ) = -(*answ);
	
	return 0;
}

size_t proccessPow(double *answ)
{
	char op;
	size_t r;
	double t = 0;
	
	if(r = proccessUnary(answ))
		return r;

	op = *token;
	while(op == '^')
	{
		getToken();
		if(r = proccessUnary(&t))
			return r;
		*answ = pow(*answ, t);
		op = *token;
	}

	return 0;
}

size_t proccessMul(double *answ)
{
	char op;
	size_t r;
	double t = 0;
	
	if(r = proccessPow(answ))
		return r;

	op = *token;
	while(op == '*' || op == '/')
	{
		getToken();
		if(r = proccessPow(&t))
			return r;
		switch(op)
		{
			case '*':
				*answ *= t;
			break;

			case '/':
				if(t == 0)
					return ANERR_DIVBYZERO;
				*answ /= t;
			break;
		}
		op = *token;
	}

	return 0;
}

size_t proccessSum(double *answ)
{
	char op;
	double t = 0;
	size_t r;
	if(r = proccessMul(answ))
		return r;

	op = *token;
	while(op == '+' || op == '-')
	{
		getToken();
		if(r = proccessMul(&t))
			return r;
		switch(op)
		{
			case '+':
				*answ += t;
			break;

			case '-':
				*answ -= t;
			break;
		}
		
		op = *token;
	}

	return 0 ;
}




double calcExpression(char *str)
{
	double res = 0;
	size_t r = 0;
	expr = str;
	i = 0;
	getToken();
	if(r = proccessSum(&res))
	{
		char buf[64];
		switch(r)
		{
			case ANERR_SYNTAX:
				strcpy(buf, "Syntax error");
			break;
			case ANERR_DIVBYZERO:
				strcpy(buf, "Division by zero");
			break;
			case ANERR_BRACKETS:
				strcpy(buf, "Brackets pairing error");
			break;
		}
		printf("Calculation error %d (%s)\n", r, buf);
	}else
		printf("%s =  %lf\n", expr, res);
	return res;

}

void main()
{
	char buf[128];
	do
	{
		fgets(buf, sizeof(buf), stdin);
		calcExpression(buf);
	}while(*buf != '\0');
	getch();
}