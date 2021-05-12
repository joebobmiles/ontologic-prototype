#ifndef __ONTOLOGIC_PLATFORM_H__
#define __ONTOLOGIC_PLATFORM_H__

#include "Standard.h"

/**
 * A struct used to collect together data relevant to the current platform.
 * This is only used by functions that need to invoke the platform directly,
 * and intended to be stored as a global.
 *
 * Since the contents of this struct will need to change on a per-platform basis,
 * it is left only declared right now, but not defined.
 */
struct platform;

/*
	BEGIN EXIT
*/

/* Defines exit codes that are used when terminating the process. */
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

/**
 * The underlying function invoked when an abort is performed.
 * 
 * @param[in]	exitCode		The code to exit with.
 * @param[in]	message			The message to report after abort.
 * @param[in]	messageLength	The length of the abort message.
 */
void _Abort(exit_code, const char*, const size);

/**
 * Exits the process with the given error code and reports the given message.
 * 
 * @param[in]	exitCode	The code to exit with.
 * @param[in]	message		The message, as string literal, to report.
 */
#define Abort(C, M) _Abort(C, "Aborted: " ## M, sizeof("Aborted: " ## M))
/*
	END ASSERT & ABORT
*/

/*
	BEGIN MEMORY
*/

/* A global memory pool used for internal allocations. */
typedef struct memory_arena
{
	void* Start;
	void* Cursor;
	size Size;
}
memory_arena;

/**
 * Initializes a memory_arena to the specified size.
 * 
 * @param[in|out]	memoryArena	The arena to setup.
 * @param[in]		size		The size to initialize the arena as.
 */
void SetupMemoryArena(memory_arena*, const size);

/**
 * Frees the memory allocated to the memory_arena.
 *
 * @param[in|out]	memoryArena	The arena to teardown.
 */
void TeardownMemoryArena(memory_arena*);

/**
 * Allocates memory in the (global) memory arena for use elsewhere.
 *
 * @param[in]	allocationSize	The amount of memory, in bytes, to allocate.
 *
 * @return	A pointer to the allocated memory.
 */
void* Allocate(const size allocationSize);

/*
	END MEMORY
*/

/*
	BEGIN CONSOLE
*/

/* Defines a platform-independent console for use by the rest of process. */
typedef struct console
{
	char* Buffer;
	size BufferWidth;
	size BufferHeight;

	i16 CursorTop;
	i16 CursorLeft;
}
console;

/**
 * Writes the given string to the console at the current cursor position.
 * 
 * @param[in|out]	console			The console to write to.
 * @param[in]		string			The string to write to the console.
 * @param[in]		stringLength	The length of the string being written.
 *
 * @return	The number of characters written to the console.
 */
i32 ConsoleWrite(console*, const char*, const size);

/**
 * Writes the given format string to the console using the arguments at the tail
 * of the argument list.
 * 
 * @param[in|out]	console			The console to write to.
 * @param[in]		format			The format string to use.
 * @param[in]		formatLength	The length of the format string.
 * @param[in]		...				The arguments to use during formatting.
 * 
 * @return	The number of characters written to the console.
 */
i32 ConsoleWriteF(console*, const char*, const size, ...);

/**
 * Writes the given string to the console at the current cursor position. Once
 * finished, it moves the cursor down one line.
 *
 * @param[in|out]	console			The console to write to.
 * @param[in]		string			The string to write to the console.
 * @param[in]		stringLength	The length of the string being written.
 *
 * @return	The number of characters written to the console.
 */
i32 ConsoleWriteLine(console*, const char*, const size);

/**
 * Writes the given format string to the console at the current cursor position,
 * using the arguments at the tail of the argument list. Once finished, it moves
 * the cursor down one line.
 *
 * @param[in|out]	console			The console to write to.
 * @param[in]		format			The string to write to the console.
 * @param[in]		formatLength	The length of the string being written.
 * @param[in]		...				The arguments to use during formatting.
 *
 * @return	The number of characters written to the console.
 */
i32 ConsoleWriteLineF(console*, const char*, const size, ...);

/**
 * "Blits" the console buffer to the platform's console.
 *
 * @param[in]	console	The console to blit.
 */
void BlitConsole(console*);

/**
 * Clears the console buffer with '\0'.
 *
 * @param[in|out]	console	The console to clear.
 */
void ClearConsole(console*);

/*
	END CONSOLE
*/

/*
	BEGIN INPUT EVENTS
*/

/* Platform independent keycodes. */
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

/* Platform independent keycodes. */
typedef enum keycode
{
#define _(CODE) CODE,
	KEYCODES
#undef _
}
keycode;

/* Represents a single input event received from the host system. */
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

/* A circular buffer that stores input events received from the host system. */
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

/**
 * Reads input events from the host system and places them in the given input
 * buffer.
 *	
 * @param[in|out]	inputBuffer	The buffer to store events in.
 * 
 * @return	The number of events written to the buffer.
 */
i32 InputBufferRead(input_buffer*);

/**
 * Pops the first event off the input buffer.
 * 
 * @param[in|out]	inputBuffer	The buffer to get the event from.
 * 
 * @return	A pointer to the input event popped off the buffer.
 */
input_event* PopInputEventFrom(input_buffer*);

/**
 * Gives a reference to the input event at the front of the input buffer.
 *
 * @param[in]	inputBuffer The buffer to get the event from.
 *
 * @return	A pointer to the input event at the front of the buffer.
 */
input_event* PeekInputEventFrom(input_buffer*);

/*
	END INPUT EVENTS
*/

#endif
