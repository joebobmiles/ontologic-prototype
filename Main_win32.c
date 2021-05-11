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

internal
void _Abort(exit_code code, const char* message, const size messageSize)
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

	Exit(code);
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
		Abort(
			EXIT_SYSTEM_OUT_OF_MEMORY,
			"Ran out of available memory."
		);
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
void ClearConsole(console* console)
{
	for (size i = 0; i < console->BufferHeight * console->BufferWidth; i++)
		console->Buffer[i] = '\0';
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
	persist INPUT_RECORD inputRecords[64];

	u32 numberOfEvents;
	GetNumberOfConsoleInputEvents(Platform.hStandardInput, &numberOfEvents);

	u32 eventsRead;
	ReadConsoleInputA(
		Platform.hStandardInput,
		inputRecords,
		numberOfEvents,
		&eventsRead
	);

	inputBuffer->EventCount = 0;

	for (size i = 0; i < eventsRead; i++)
	{
		if (
			inputBuffer->EventCount < inputBuffer->MaxEventCount
			&& inputRecords[i].EventType == KEY_EVENT
		)
		{
			inputBuffer->Events[inputBuffer->EventCount] = (input_event){
				.Key = VirtualKeyToKeyCode(
					inputRecords[i].Event.KeyEvent.wVirtualKeyCode
				),
				.KeyDown = inputRecords[i].Event.KeyEvent.bKeyDown,
				.KeyUp = !inputRecords[i].Event.KeyEvent.bKeyDown,
			};

			inputBuffer->EventCount++;
		}
		else
			break;
	}

	return eventsRead;
}

internal
keycode VirtualKeyToKeyCode(WORD virtualKey)
{
	switch (virtualKey)
	{
	case VK_ESCAPE:
		return KEY_ESCAPE;

	case VK_SPACE:
		return KEY_SPACE;
	
	case VK_BACK:
		return KEY_BACKSPACE;

	case 0x30:
		return KEY_0;

	case 0x31:
		return KEY_1;

	case 0x32:
		return KEY_2;

	case 0x33:
		return KEY_3;

	case 0x34:
		return KEY_4;

	case 0x35:
		return KEY_5;

	case 0x36:
		return KEY_6;

	case 0x37:
		return KEY_7;

	case 0x38:
		return KEY_8;

	case 0x39:
		return KEY_9;

	case 0x41:
		return KEY_A;

	case 0x42:
		return KEY_B;

	case 0x43:
		return KEY_C;

	case 0x44:
		return KEY_D;

	case 0x45:
		return KEY_E;

	case 0x46:
		return KEY_F;

	case 0x47:
		return KEY_G;

	case 0x48:
		return KEY_H;

	case 0x49:
		return KEY_I;

	case 0x4a:
		return KEY_J;

	case 0x4b:
		return KEY_K;

	case 0x4c:
		return KEY_L;

	case 0x4d:
		return KEY_M;

	case 0x4e:
		return KEY_N;

	case 0x4f:
		return KEY_O;

	case 0x50:
		return KEY_P;

	case 0x51:
		return KEY_Q;

	case 0x52:
		return KEY_R;

	case 0x53:
		return KEY_S;

	case 0x54:
		return KEY_T;

	case 0x55:
		return KEY_U;

	case 0x56:
		return KEY_V;

	case 0x57:
		return KEY_W;

	case 0x58:
		return KEY_X;

	case 0x59:
		return KEY_Y;

	case 0x5a:
		return KEY_Z;

	default:
		return KEY_NONE;
	}
}

internal
char KeycodeToChar(keycode key)
{
	switch (key)
	{
	#define _(CODE, CHAR) case CODE: return CHAR;
		KEYCODES
	#undef _
	default: return '\0';
	}
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
				.Events = Allocate(sizeof(input_event) * 32),
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
		{
			Abort(
				EXIT_COULD_NOT_GET_SCREEN_BUFFER_INFO,
				"Unable to retreive screen buffer size."
			);
		}
	}
	else
	{
		Abort(
			EXIT_COULD_NOT_SET_ACTIVE_SCREEN_BUFFER,
			"Unable to change screen buffer from standard input."
		);
	}
	
	CloseHandle(hConsole);

	CloseHandle(hStandardOutput);
	CloseHandle(hStandardInput);
	CloseHandle(hStandardError);

	return EXIT_NORMAL;
}