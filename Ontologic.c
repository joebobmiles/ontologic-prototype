#include "Standard.h"
#include "Platform.h"

/**
 * This is the main function that runs the Ontologic runtime.
 * 
 * @param[in] console		A pointer to the console instance to write to.
 * @param[in] inputBuffer	A pointer to the buffer used for receiving input events.
 */
internal void Main(console* console, input_buffer* inputBuffer)
{
	bool quit = false;

	size i = 0;
	char buffer[1024];

	until (quit == true)
	{
		ClearConsole(console);

		ConsoleWrite(console, buffer, i);

		InputBufferRead(inputBuffer);
		if (0 < inputBuffer->EventCount)
		{
			input_event* event;
			until((event = PopInputEventFrom(inputBuffer)) == NULL)
			{
				if (event->KeyUp && event->Key == KEY_ESCAPE)
					quit = true;

				else if (event->KeyDown)
				{
					if (event->Key == KEY_BACKSPACE)
						buffer[0 < i ? --i : i] = '\0';

					else
						buffer[i++] = event->Character;
				}
			}
		}

		BlitConsole(console);
	}
}
