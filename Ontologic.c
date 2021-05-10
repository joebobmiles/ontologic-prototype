#include "Standard.h"
#include "Platform.h"

internal
void Main(console* console, input_buffer* inputBuffer)
{
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
