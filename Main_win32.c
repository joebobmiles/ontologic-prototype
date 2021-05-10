#include "Standard.h"
#include "Platform.h"

#include "./Ontologic.c"

#include <Windows.h>

typedef struct platform
{
	HANDLE StandardConsoleHandle;
	HANDLE ConsoleHandle;
}
platform;

global platform Platform;

internal
void Exit(exit_code code)
{
	ExitProcess(code);
}

internal
void _Assert(
	const bool predicate,
	const char* expression,
	const size expressionLength,
	const char* fileName,
	const size fileNameLength,
	const size line
)
{
	if (!predicate)
	{
		char buffer[1024];
		char message[] =
			"Assertion \"%s\" on line %i in file \"%s\" failed.\n\0";

		size messageLength = FormatString(
			buffer, 1024,
			message, sizeof(message),
			expression, expressionLength,
			line,
			fileName, fileNameLength
		);

		OutputDebugStringA(buffer);

		u32 charactersWritten;
		WriteConsoleA(
			Platform.StandardConsoleHandle,
			buffer,
			messageLength,
			&charactersWritten,
			NULL
		);


		Exit(EXIT_ASSERT_FAILED);
	}
}

internal
void _AssertWithMessage(
	bool predicate,
	const char* message,
	const size messageSize
)
{
	if (!predicate)
	{
		OutputDebugStringA(message);

		u32 charactersWritten;
		WriteConsoleA(
			Platform.StandardConsoleHandle,
			message,
			messageSize,
			&charactersWritten,
			NULL
		);

		Exit(EXIT_ASSERT_FAILED);
	}
}

global memory_arena MemoryArena;

internal
void SetupMemoryArena(memory_arena* arena, const size arenaSize)
{
	arena->Start = VirtualAlloc(
		NULL,
		arenaSize,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE
	);

	AssertWithMessage(
		arena->Start != NULL,
		"Not enough memory available to initialize."
	);

	arena->Cursor = arena->Start;
	arena->Size = arenaSize;
}

internal
void TeardownMemoryArena(memory_arena* arena)
{
	VirtualFree(arena->Start, 0, MEM_RELEASE);
}

internal
void* Allocate(const size allocationSize)
{
	size currentlyAllocatedSize =
		(size)MemoryArena.Cursor - (size)MemoryArena.Start;
	size newAllocationSize = currentlyAllocatedSize + allocationSize;

	if (newAllocationSize <= MemoryArena.Size)
	{
		void* oldCursor = MemoryArena.Cursor;
		MemoryArena.Cursor = (void*)
			((size)MemoryArena.Cursor + allocationSize);

		return oldCursor;
	}
	else
	{
		Exit(EXIT_SYSTEM_OUT_OF_MEMORY);
	}
}

internal
i32 ConsoleWrite(
	console* console,
	const char* string,
	const size stringLength
)
{
	i32 charactersWritten;

	WriteConsoleOutputCharacterA(
		Platform.ConsoleHandle,
		string,
		stringLength,
		(COORD) {
			.X = console->CursorLeft,
			.Y = console->CursorTop,
		},
		&charactersWritten
	);

	return charactersWritten;
}

internal
i32 ConsoleWriteLine(
	console* console,
	const char* string,
	const size stringLength
)
{
	i32 charactersWritten;

	WriteConsoleOutputCharacterA(
		Platform.ConsoleHandle,
		string,
		stringLength,
		(COORD) {
			.X = console->CursorLeft,
			.Y = console->CursorTop,
		},
		&charactersWritten
	);

	console->CursorTop++;

	return charactersWritten;
}

internal
i32 ConsoleWriteF(
	console* console,
	const char* format,
	const size formatSize,
	...
)
{
	arg_list args;
	SetupArgList(args, formatSize);

	char buffer[128];
	size stringLength = _FormatString(
		buffer, 128,
		format, formatSize,
		args
	);

	TeardownArgList(args);

	return ConsoleWrite(console, buffer, stringLength);
}

internal
i32 ConsoleWriteLineF(
	console* console,
	const char* format,
	const size formatSize,
	...
)
{
	arg_list args;
	SetupArgList(args, formatSize);

	char buffer[128];
	size stringLength = _FormatString(
		buffer, 128,
		format, formatSize,
		args
	);

	TeardownArgList(args);

	return ConsoleWriteLine(console, buffer, stringLength);
}

i32 wmain(void)
{
	HANDLE hStandardConsole = GetStdHandle(
		STD_OUTPUT_HANDLE | STD_ERROR_HANDLE
	);
	HANDLE hConsole = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
	
	if (SetConsoleActiveScreenBuffer(hConsole))
	{
		CONSOLE_SCREEN_BUFFER_INFO bufferInfo;

		if (GetConsoleScreenBufferInfo(hConsole, &bufferInfo))
		{
			CONSOLE_CURSOR_INFO cursorInfo;
			GetConsoleCursorInfo(hConsole, &cursorInfo);

			cursorInfo.bVisible = 0;
			SetConsoleCursorInfo(hConsole, &cursorInfo);

			Platform = (struct platform)
			{
				.StandardConsoleHandle = hStandardConsole,
				.ConsoleHandle = hConsole,
			};

			SetupMemoryArena(&MemoryArena, Kilobyte(3));

			char* consoleBuffer = Allocate(
				sizeof(char)
				* bufferInfo.dwMaximumWindowSize.X
				* bufferInfo.dwMaximumWindowSize.Y
			);

			console c = (console){
				.Buffer = consoleBuffer,

				.BufferWidth = bufferInfo.dwMaximumWindowSize.X,
				.BufferHeight = bufferInfo.dwMaximumWindowSize.Y,

				.CursorLeft = bufferInfo.dwCursorPosition.X,
				.CursorTop = bufferInfo.dwCursorPosition.Y
			};

			Main(&c);

			TeardownMemoryArena(&MemoryArena);

			SetConsoleActiveScreenBuffer(hStandardConsole);
		}

		else
			return EXIT_COULD_NOT_GET_SCREEN_BUFFER_INFO;
	}
	else
		return EXIT_COULD_NOT_SET_ACTIVE_SCREEN_BUFFER;
	
	CloseHandle(hConsole);
	CloseHandle(hStandardConsole);

	return EXIT_NORMAL;
}