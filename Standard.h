#ifndef __ONTOLOGIC_STANDARD_H__
#define __ONTOLOGIC_STANDARD_H__

#define NULL 0

/*
	BEGIN size_t DEFINITION
*/
#define _TYPEDEF_SIZE_T(T) \
	typedef T __ontologic_size_t;

#if defined(__SIZE_TYPE__)
	_TYPEDEF_SIZE_T(__SIZE_TYPE__)
#elif defined(_WIN64)
	_TYPEDEF_SIZE_T(unsigned long)
#elif defined(_WIN32)
	_TYPEDEF_SIZE_T(unsigned long int)
#else
	#error Unable to define size_t!
#endif

#define size __ontologic_size_t
/*
	END size_t DEFINITION
*/

/*
	BEGIN NUMERIC TYPE DEFINITION
*/
#if defined(_WIN64)
	typedef char				__ontologic_int8;
	typedef short				__ontologic_int16;
	typedef int					__ontologic_int32;
	typedef long				__ontologic_int64;

	typedef unsigned char		__ontologic_uint8;
	typedef unsigned short		__ontologic_uint16;
	typedef unsigned int		__ontologic_uint32;
	typedef unsigned long		__ontologic_uint64;
#elif defined(_WIN32)
	typedef char				__ontologic_int8;
	typedef short				__ontologic_int16;
	typedef int					__ontologic_int32;
	typedef long long			__ontologic_int64;

	typedef unsigned char		__ontologic_uint8;
	typedef unsigned short		__ontologic_uint16;
	typedef unsigned int		__ontologic_uint32;
	typedef unsigned long long	__ontologic_uint64;
#endif

typedef float		__ontologic_float32;
typedef double		__ontologic_float64;
typedef long double	__ontologic_float128;

#define byte	__ontologic_uint8
#define bool	__ontologic_uint8

#define u8		__ontologic_uint8
#define u16		__ontologic_uint16
#define u32		__ontologic_uint32
#define u64		__ontologic_uint64

#define i8		__ontologic_int8
#define i16		__ontologic_int16
#define i32		__ontologic_int32
#define i64		__ontologic_int64

#define f32		__ontologic_float32;
#define f64		__ontologic_float64;
#define f128	__ontologic_float128;
/*
	END NUMERIC TYPE DEFINITION
*/

/*
	BEGIN STATIC ALIAS MACROS
*/
#define global static
#define internal static
#define persist static
/*
	END STATIC ALIAS MACROS
*/

/*
	BEGIN MEMORY SIZE MACROS
*/
#define Kilobyte(N) ((N) * 1024ull)
#define Megabyte(N) (Kilobyte(N) * 1024ull)
#define Gigabyte(N) (Megabyte(N) * 1024ull)
/*
	END MEMORY SIZE MACROS
*/

/*
	BEGIN MACRO UTILITY MACROS
*/
#define __JOIN(A, B) A ## B
#define _JOIN(A, B) __JOIN(A, B)

#if defined(__COUNTER__)
	#define TEMP(P) _JOIN(P##_, __COUNTER__)
#elif defined(__LINE__) && defined(__FUNCTION__)
	#define TEMP(P) _JOIN(P##_, JOIN(__FUNCTION__, __LINE__))
#else
	#error Unable to define TEMP()!
#endif
/*
	END MACRO UTILITY MACROS
*/

/*
	BEGIN UTILITY MACROS
*/
#define forever while (1)
/*
	END UTILITY MACROS
*/

/*
	BEGIN STANDARD LIBRARY MACROS
*/
#define Abs(n) ((n * (n >= 0)) + (n * -1 * (n < 0)))

#define _Swap(T, a, b, t) \
	{ \
		T t = a; \
		a = b; \
		b = t; \
	}
#define Swap(T, a, b) _Swap(T, a, b, TEMP(t))
/*
	END STANDARD LIBRARY MACROS
*/

/*
	BEGIN VARIADIC FUNCTION MACROS
*/
typedef void* __ontologic_arg_list;
#define arg_list __ontologic_arg_list

#define SetupArgList(l, a) \
	l = (void*)((size)(&(a)) + sizeof((a)))

internal
void* _PopArg(arg_list* args, size argSize)
{
	void* arg = *args;
	*args = (void*)((size)*args + (size)argSize);
	return arg;
}

#define PeekArg(l, T) (*(T*)l)
#define PopArg(l, T) (*(T*)_PopArg(&l, sizeof(T)))

#define TeardownArgList(l) l = NULL
/*
	END VARIADIC FUNCTION MACROS
*/

/*
	BEGIN STANDARD PROCEDURES
*/
internal
size ItoA(char* buffer, size bufferSize, i32 n)
{
	if (bufferSize == 0) return 0;
	if (n == 0)
	{
		buffer[0] = '0';
		return 1;
	}

	unsigned int isNegative = n < 0;
	n = Abs(n);

	size i = 0;
	while (n != 0 && i < bufferSize)
	{
		buffer[i++] = (char)((n % 10) + 48);
		n /= 10;
	}

	buffer[i] = isNegative * '-';
	i += isNegative;

	size charsWritten = i;

	size start = 0, end = --i;
	while (start < end)
	{
		Swap(char, buffer[start], buffer[end]);
		start++; end--;
	}

	return charsWritten;
}

internal
size _FormatString(
	char* buffer,
	size bufferSize,
	const char* format,
	size formatSize,
	arg_list args
)
{
	size charsWritten = 0;
	for (size i = 0; i < formatSize; i++)
	{
		if (bufferSize <= charsWritten)
			break;

		char c = format[i];

		if (c != '%' || formatSize <= i + 1)
		{
			buffer[charsWritten++] = c;
		}

		else
		{
			switch (format[i+1])
			{
			case 'c':
			{
				buffer[charsWritten] = PopArg(args, char);
				charsWritten++;
			} break;

			case 'i':
			{
				charsWritten += ItoA(
					(char*)((size)buffer + charsWritten),
					bufferSize - charsWritten,
					PopArg(args, int)
				);
			} break;

			case 's':
			{
				char* s = PopArg(args, char*);
				size sLength = PopArg(args, size);

				for (size j = 0; j < sLength; j++)
				{
					if (charsWritten < bufferSize)
						buffer[charsWritten++] = s[j];

					else break;
				}
			} break;

			default:
			{
				buffer[i] = c;
			} break;
			}

			i++;
		}
	}

	return charsWritten;
}

internal
size FormatString(
	char* buffer,
	size bufferSize,
	const char* format,
	size formatSize,
	...
)
{
	arg_list args;
	SetupArgList(args, formatSize);
	
	size charsWritten = _FormatString(
		buffer,
		bufferSize,
		format,
		formatSize,
		args
	);

	TeardownArgList(args);

	return charsWritten;
}
/*
	END STANDARD PROCEDURES
*/

#endif