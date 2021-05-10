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
		input_event* event = PopInputEventFrom(inputBuffer);

		if (event)
		{
			if (event->KeyDown)
				ConsoleWriteLine(console, "Key down!", 9);

			else if (event->KeyUp)
				ConsoleWriteLine(console, "Key up!", 7);
		}


		BlitConsole(console);
	}
}
