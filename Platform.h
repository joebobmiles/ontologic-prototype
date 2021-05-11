#ifndef __ONTOLOGIC_PLATFORM_H__
#define __ONTOLOGIC_PLATFORM_H__

#include "Standard.h"

struct platform;

/*
	BEGIN EXIT
*/

typedef enum exit_code
{
	EXIT_NORMAL,
	EXIT_ASSERT_FAILED,
	EXIT_SYSTEM_OUT_OF_MEMORY,
	EXIT_COULD_NOT_SET_ACTIVE_SCREEN_BUFFER,
	EXIT_COULD_NOT_GET_SCREEN_BUFFER_INFO,
}
exit_code;

/**
 * Exits the process with the given exit code.
 * 
 * @param[in]	exitCode	The code to return with.
 */
void Exit(exit_code);

/*
	END EXIT
*/

/*
	BEGIN ASSERT & ABORT
*/

/**
 * The underlying function to use when Asserts are enabled. This is not intended
 * to be invoked on its own. Instead, use the Assert macro.
 * 
 * @param[in]	predicate				The predicate to verify.
 * @param[in]	predicateString			The predicate as string.
 * @param[in]	predicateStringLength	The length of the predicate string.
 * @param[in]	filepath				The path to the file containing the assertion.
 * @param[in]	filepathLength			The length of the filepath string.
 * @param[in]	line					The line of the file the assertion is on.
 */
void _Assert(
	const bool, const char*, const size, const char*, const size, const size
);

/**
 * Asserts that the given predicate is true. If false, causes the process to 
 * exit with ERROR_ASSERT_FAILED.
 * 
 * @param[in]	predicate	The predicate to verify.
 */
#define Assert(P) \
	_Assert( \
		(P), #P, sizeof(#P)-1, __FILE__, sizeof(__FILE__)-1, __LINE__ \
	)

/**
 * The underlying function to use when assertions are enabled. This is not
 * intended to be invoked on its own. Instead, use the AssertWithMessage macro.
 *
 * @param[in]	predicate		The predicate to verify.
 * @param[in]	message			The message to display on failed assertion.
 * @param[in]	messageLength	The length of the given message.
 */
void _AssertWithMessage(
	const bool, const char*, const size
);

/**
 * Asserts that the given predicate is true. If false, the process will exit
 * with ERROR_ASSERT_FAILED and print out the given message.
 * 
 * @param[in]	predicate	The predicate to verify.
 * @param[in]	message		The message to display on assertion failure.
 */
#define AssertWithMessage(P, M) \
	_AssertWithMessage( \
		(P), "Assertion failed: " ## M, sizeof("Assertion failed: " ## M) \
	)

void _Abort(exit_code, const char*, const size);
#define Abort(C, M) _Abort(C, "Aborted: " ## M, sizeof("Aborted: " ## M))
/*
	END ASSERT & ABORT
*/

/*
	BEGIN MEMORY
*/
typedef struct memory_arena
{
	void* Start;
	void* Cursor;
	size Size;
}
memory_arena;

void SetupMemoryArena(memory_arena*, const size);
void TeardownMemoryArena(memory_arena*);

void* Allocate(const size allocationSize);
/*
	END MEMORY
*/

/*
	BEGIN CONSOLE
*/
typedef struct console
{
	char* Buffer;
	size BufferWidth;
	size BufferHeight;

	i16 CursorTop;
	i16 CursorLeft;
}
console;

i32 ConsoleWrite(console*, const char*, const size);
i32 ConsoleWriteF(console*, const char*, const size, ...);
i32 ConsoleWriteLine(console*, const char*, const size);
i32 ConsoleWriteLineF(console*, const char*, const size, ...);

void BlitConsole(console*);
void ClearConsole(console*);
/*
	END CONSOLE
*/

/*
	BEGIN INPUT EVENTS
*/
#define KEYCODES \
	_(KEY_NONE) \
	_(KEY_ESCAPE) \
	_(KEY_SPACE) \
	_(KEY_BACKSPACE) \
\
	_(KEY_A) \
	_(KEY_B) \
	_(KEY_C) \
	_(KEY_D) \
	_(KEY_E) \
	_(KEY_F) \
	_(KEY_G) \
	_(KEY_H) \
	_(KEY_I) \
	_(KEY_J) \
	_(KEY_K) \
	_(KEY_L) \
	_(KEY_M) \
	_(KEY_N) \
	_(KEY_O) \
	_(KEY_P) \
	_(KEY_Q) \
	_(KEY_R) \
	_(KEY_S) \
	_(KEY_T) \
	_(KEY_U) \
	_(KEY_V) \
	_(KEY_W) \
	_(KEY_X) \
	_(KEY_Y) \
	_(KEY_Z) \
\
	_(KEY_0) \
	_(KEY_1) \
	_(KEY_2) \
	_(KEY_3) \
	_(KEY_4) \
	_(KEY_5) \
	_(KEY_6) \
	_(KEY_7) \
	_(KEY_8) \
	_(KEY_9)

typedef enum keycode
{
#define _(CODE) CODE,
	KEYCODES
#undef _
}
keycode;

/*
Represents a single input event received from the host system.
*/
typedef struct input_event
{
	/* The virtual key code that we received. */
	keycode Key;
	/* True when the key is down, false when the key is up. */
	bool KeyDown;
	/* True when the key is up, false when the key is down. */
	bool KeyUp;

	/* The character that was typed. */
	char Character;
}
input_event;

/*
A circular buffer that stores input events received from the host system.
*/
typedef struct input_buffer
{
	/* The events we received from the host. */
	input_event* Events;
	/* The number of events we are storing. */
	size EventCount;
	/* The maximum number of events that can be stored in the buffer. */
	size MaxEventCount;

	/* The index to the head of the buffer. */
	size HeadIndex;
	/* The index to the tail of the buffer. */
	size TailIndex;
}
input_buffer;

i32 InputBufferRead(input_buffer*);
input_event* PopInputEventFrom(input_buffer*);
input_event* PeekInputEventFrom(input_buffer*);
/*
	END INPUT EVENTS
*/

#endif
