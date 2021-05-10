#include "Standard.h"
#include "Platform.h"

#include "./Ontologic.c"

#include <Windows.h>

typedef struct platform
{
	HANDLE hStandardOutput;
	HANDLE hStandardError;
	HANDLE hStandardInput;

	HANDLE hConsole;
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
			Platform.hStandardOutput,
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
			Platform.hStandardOutput,
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
void BlitConsole(console* console)
{
	i32 charactersWritten;

	for (size y = 0; y < console->BufferHeight; y++)
	{
		for (size x = 0; x < console->BufferWidth; x++)
		{
			WriteConsoleOutputCharacterA(
				Platform.hConsole,
				&(console->Buffer[y * console->BufferWidth + x]),
				1,
				(COORD) { .X = x, .Y = y, },
				&charactersWritten
			);
		}
	}
}

internal
i32 ConsoleWrite(
	console* console,
	const char* string,
	const size stringLength
)
{
	i32 charactersWritten = 0;

	size cursorLeft = console->CursorLeft;
	size cursorTop = console->CursorTop;
	
	for (size i = 0; i < stringLength; i++)
	{
		size left = cursorLeft + i;
		size top = cursorTop;

		if (left < console->BufferWidth)
		{
			console->Buffer[top * console->BufferWidth + left] = string[i];
			charactersWritten++;
		}

		else
			break;
	}

	return charactersWritten;
}

internal
i32 ConsoleWriteLine(
	console* console,
	const char* string,
	const size stringLength
)
{
	i32 charactersWritten = ConsoleWrite(console, string, stringLength);

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

internal
i32 InputBufferRead(input_buffer* inputBuffer)
{
	u32 numberOfEvents;
	GetNumberOfConsoleInputEvents(Platform.hStandardInput, &numberOfEvents);

	persist PINPUT_RECORD inputRecords = NULL;
	
	if (inputRecords == NULL)
		inputRecords = Allocate(sizeof(INPUT_RECORD) * numberOfEvents);

	u32 eventsRead;
	ReadConsoleInputA(
		Platform.hStandardInput,
		inputRecords,
		numberOfEvents,
		&eventsRead
	);

	for (size i = 0; i < eventsRead; i++)
	{
		if (
			inputBuffer->EventCount < inputBuffer->MaxEventCount
			&& inputRecords[i].EventType == KEY_EVENT
		)
		{
			inputBuffer->Events[inputBuffer->EventCount].C =
				inputRecords[i].Event.KeyEvent.uChar.AsciiChar;
			inputBuffer->Events[inputBuffer->EventCount].KeyDown =
				inputRecords[i].Event.KeyEvent.bKeyDown;
			inputBuffer->Events[inputBuffer->EventCount].KeyUp =
				!inputRecords[i].Event.KeyEvent.bKeyDown;

			inputBuffer->EventCount++;
		}
		else
			break;
	}

	return eventsRead;
}

internal
input_event* PopInputEventFrom(input_buffer* inputBuffer)
{
	return inputBuffer->EventCount
		? &inputBuffer->Events[--inputBuffer->EventCount]
		: NULL;
}

i32 wmain(void)
{
	HANDLE hStandardOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hStandardInput = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hStandardError = GetStdHandle(STD_ERROR_HANDLE);

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
				.hStandardOutput = hStandardOutput,
				.hStandardInput = hStandardInput,
				.hStandardError = hStandardError,

				.hConsole = hConsole,
			};

			SetupMemoryArena(&MemoryArena, Kilobyte(10));

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

			input_buffer inputBuffer = (input_buffer){
				.Events = Allocate(sizeof(input_event) * 1024),
				.EventCount = 0,
				
				.MaxEventCount = 1024,

				.HeadIndex = 0,
				.TailIndex = 0,
			};

			Main(&c, &inputBuffer);

			TeardownMemoryArena(&MemoryArena);

			SetConsoleActiveScreenBuffer(hStandardOutput);
		}

		else
			return EXIT_COULD_NOT_GET_SCREEN_BUFFER_INFO;
	}
	else
		return EXIT_COULD_NOT_SET_ACTIVE_SCREEN_BUFFER;
	
	CloseHandle(hConsole);

	CloseHandle(hStandardOutput);
	CloseHandle(hStandardInput);
	CloseHandle(hStandardError);

	return EXIT_NORMAL;
}