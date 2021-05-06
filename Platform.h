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
/*
	END CONSOLE
*/

#endif
