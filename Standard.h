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
    BEGIN BOOLEAN CONSTANTS
*/

#define true	((bool)1)
#define false	((bool)0)

/*
    END BOOLEAN CONSTANTS
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

/**
 * Computes the size of N kilobytes in bytes.
 *
 * @param[in]	N	The number of kilobytes.
 *
 * @return	The number of bytes equal to N kilobytes.
 */
#define Kilobyte(N) ((N) * 1024ull)

/**
 * Computes the size of N megabytes in bytes.
 *
 * @param[in]	N	The number of megabytes.
 *
 * @return	The number of bytes equal to N megabytes.
 */
#define Megabyte(N) (Kilobyte(N) * 1024ull)

/**
 * Computes the size of N gigabytes in bytes.
 *
 * @param[in]	N	The number of gigabytes.
 *
 * @return	The number of bytes equal to N gigabytes.
 */
#define Gigabyte(N) (Megabyte(N) * 1024ull)

/*
    END MEMORY SIZE MACROS
*/

/*
    BEGIN MACRO UTILITY MACROS
*/

#define __JOIN(A, B) A ## B

/**
 * Joins two values together using the macro join operator. The nested macro is
 * necessary to prevent the preprocessor from leaving operands unexpanded.
 *
 * @param[in]	A	The first value to join.
 * @param[in]	B	The second value to join.
 *
 * @return	The values of A and B joined using the '##' operator.
 */
#define _JOIN(A, B) __JOIN(A, B)

#if defined(__COUNTER__)
    /**
     * Creates a temporary variable name for use in macros.
     *
     * @param[in]	prefix	The prefix to use for the temporary variable.
     * 
     * @return	A contextually unique identifier with the given prefix.
     */
    #define TEMP(P) _JOIN(P##_, __COUNTER__)
#elif defined(__LINE__) && defined(__FUNCTION__)
    /**
     * Creates a temporary variable name for use in macros.
     *
     * @param	prefix	The prefix to use for the temporary variable.
     * 
     * @return	A contextually unique identifier with the given prefix.
     */
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

#define forever		while (1)
#define until(P)	while (!(P))

/*
    END UTILITY MACROS
*/

/*
    BEGIN STANDARD LIBRARY MACROS
*/

/**
 * Returns the absolute value of n.
 *
 * @param[in]	n	The number to take the absolute value of.
 *
 * @return	The absolute value of n.
 */
#define Abs(n) ((n * (n >= 0)) + (n * -1 * (n < 0)))

/**
 * The underlying macro for performing swaps. This is not intended to be used
 * directly.
 *
 * @param[in]		T	The type of a, b, and t.
 * @param[in|out]	a	The first value to swap.
 * @param[in|out]	b	The second value to swap.
 * @param[in]		t	The name of the temporary variable to swap with.
 */
#define _Swap(T, a, b, t) \
    { \
        T t = a; \
        a = b; \
        b = t; \
    }

/**
 * Swaps the value of a and b.
 *
 * @param[in]		T	The type of a and b.
 * @param[in|out]	a	The first value to swap.
 * @param[in|out]	b	The second value to swap.
 */
#define Swap(T, a, b) _Swap(T, a, b, TEMP(t))

/*
    END STANDARD LIBRARY MACROS
*/

/*
    BEGIN VARIADIC FUNCTION MACROS
*/

typedef void* __ontologic_arg_list;
#define arg_list __ontologic_arg_list

/**
 * Initializes an arg_list variable.
 *
 * @param[in|out]	l	The arg_list to setup.
 * @param[in]		a	The last argument before the variable argument list.
 */
#define SetupArgList(l, a) \
    l = (void*)((size)(&(a)) + sizeof((a)))

/**
 * The underlying function that pops an argument off the argument list.
 * 
 * @param[in|out]	args	The argument list to pop from.
 * @param[in]		argSize	The size of the argument to pop.
 * 
 * @return	A pointer to the argument that was popped off the argument list.
 */
internal inline void* _PopArg(arg_list* args, size argSize)
{
    void* arg = *args;
    *args = (void*)((size)*args + (size)argSize);
    return arg;
}

/**
 * Pops an argument of type T off an argument list.
 *
 * @param[in|out]	l	The argument list to pop from.
 * @param[in]		T	The type of the argument to pop.
 *
 * @return	The value that was popped off the argument list.
 */
#define PopArg(l, T) (*(T*)_PopArg(&l, sizeof(T)))

/**
 * Peeks at the next argument in the argument list.
 * 
 * @param[in]	l	The argument list to peek.
 * @param[in]	T	The type of argument to peek.
 *
 * @return	The value that was popped off the argument list.
 */
#define PeekArg(l, T) (*(T*)l)

/**
 * Frees the given argument list to prevent it from being used.
 *
 * @param[in|out]	l	The argument list to teardown.
 */
#define TeardownArgList(l) l = NULL

/*
    END VARIADIC FUNCTION MACROS
*/

/*
    BEGIN STANDARD PROCEDURES
*/

/**
 * Converts an integer to a string stored in the given buffer.
 *
 * @param[in|out]	buffer		The buffer to fill with the integer as string.
 * @param[in]		bufferSize	The size of the string buffer.
 * @param[in]		n			The number to convert to string.
 *
 * @return	The number of characters written to the buffer.
 */
internal size ItoA(char* buffer, size bufferSize, i32 n)
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

/**
 * The underlying function used for formatting strings. Not intended to be used
 * on its own.
 *
 * @param[in|out]	buffer		The buffer to write the formatted text to.
 * @param[in]		bufferSize	The size of the buffer.
 * @param[in]		format		The format string.
 * @param[in]		formatSize	The size of the format string.
 * @param[in]		args		The args to splice into the format string.
 *
 * @return	The number of characters written to the buffer.
 */
internal size _FormatString(
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

/**
 * Fills the given buffer with the contents of the format string spliced with
 * arguments in the argument list, if applicable.
 *
 * @param[in|out]	buffer		The buffer to fill.
 * @param[in]		bufferSize	The size of the buffer.
 * @param[in]		format		The format string to use.
 * @param[in]		formatSize	The size of the format string.
 * @param[in]		...			The args to splice into the format string.
 *
 * @return	The number of characters written to the buffer.
 */
internal size FormatString(
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