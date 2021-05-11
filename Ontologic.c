#include "Standard.h"
#include "Platform.h"

internal
void Main(console* console, input_buffer* inputBuffer)
{
	bool quit = false;

	until (quit == true)
	{
		InputBufferRead(inputBuffer);

		if (0 < inputBuffer->EventCount)
		{
			input_event* event = PopInputEventFrom(inputBuffer);

			if (event->KeyUp && event->C == 'q')
				quit = true;
		}

		BlitConsole(console);
	}
}
