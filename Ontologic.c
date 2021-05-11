#include "Standard.h"
#include "Platform.h"

internal
void Main(console* console, input_buffer* inputBuffer)
{
	bool quit = false;

	size i = 0;
	char buffer[1024];

	until (quit == true)
	{
		ConsoleWrite(console, buffer, i);

		InputBufferRead(inputBuffer);
		if (0 < inputBuffer->EventCount)
		{
			input_event* event;
			until((event = PopInputEventFrom(inputBuffer)) == NULL)
			{
				if (event->KeyUp && event->C == 'q')
					quit = true;

				else if (event->KeyDown)
					buffer[i++] = event->C;
			}
		}

		BlitConsole(console);
	}
}
