#include "Standard.h"
#include "Platform.h"

internal
void Main(console* console, input_buffer* inputBuffer)
{
	ConsoleWriteLineF(
		console,
		"Your secret number is: %i.", 26,
		42
	);

	ConsoleWriteLine(console, "Goodbye.", 8);

	forever
	{
		InputBufferRead(inputBuffer);

		if (0 < inputBuffer->EventCount)
			ConsoleWriteLine(console, "Key pressed!", 12);

		BlitConsole(console);
	}
}
