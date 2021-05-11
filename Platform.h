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

void Exit(exit_code);
/*
	END EXIT
*/

/*
	BEGIN ASSERT & ABORT
*/
void _Assert(
	const bool, const char*, const size, const char*, const size, const size
);
#define Assert(P) \
	_Assert( \
		(P), #P, sizeof(#P)-1, __FILE__, sizeof(__FILE__)-1, __LINE__ \
	)

void _AssertWithMessage(
	const bool, const char*, const size
);
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

typedef struct input_event
{
	keycode Key;
	bool KeyDown;
	bool KeyUp;

	char Character;
}
input_event;

typedef struct input_buffer
{
	input_event* Events;
	size EventCount;
	size MaxEventCount;

	size HeadIndex;
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
