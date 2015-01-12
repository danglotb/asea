#ifndef _UTILITY_H
#define _UTILITY_H

#define PRINT_FATAL_ERROR(MSG) \
	{ \
		fprintf(stderr, MSG "\n"); \
		fflush(stdout); \
		fflush(stderr); \
		exit(EXIT_FAILURE); \
	}

#define PRINT_DEFINE_ERROR_VALUE(VARIABLE, VALUE) \
	{ \
		fprintf(stderr, "Please rebuild library with defined variable %s with value %d in file %s\n", #VARIABLE, VALUE, __FILE__); \
		fflush(stdout); \
		fflush(stderr); \
		exit(EXIT_FAILURE); \
	}

#define PRINT_ASSERT_ERROR_MSG(COND, MSG) \
	if(!(COND)) {\
		PRINT_FATAL_ERROR(MSG) \
	}

#endif
