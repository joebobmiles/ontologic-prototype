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
/*
	END CONSOLE
*/

/*
	BEGIN INPUT EVENTS
*/
#define KEYCODES \
	_(KEY_NONE, '\0') \
	_(KEY_ESCAPE, '\0') \
	_(KEY_A, 'a') \
	_(KEY_B, 'b') \
	_(KEY_C, 'c') \
	_(KEY_D, 'd') \
	_(KEY_E, 'e') \
	_(KEY_F, 'f') \
	_(KEY_G, 'g') \
	_(KEY_H, 'h') \
	_(KEY_I, 'i') \
	_(KEY_J, 'j') \
	_(KEY_K, 'k') \
	_(KEY_L, 'l') \
	_(KEY_M, 'm') \
	_(KEY_N, 'n') \
	_(KEY_O, 'o') \
	_(KEY_P, 'p') \
	_(KEY_Q, 'q') \
	_(KEY_R, 'r') \
	_(KEY_S, 's') \
	_(KEY_T, 't') \
	_(KEY_U, 'u') \
	_(KEY_V, 'v') \
	_(KEY_W, 'w') \
	_(KEY_X, 'x') \
	_(KEY_Y, 'y') \
	_(KEY_Z, 'z')

typedef enum keycode
{
#define _(CODE, CHAR) CODE,
	KEYCODES
#undef _
}
keycode;

char KeycodeToChar(keycode);

typedef struct input_event
{
	keycode Key;
	bool KeyDown;
	bool KeyUp;
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
