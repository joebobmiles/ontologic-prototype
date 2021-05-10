#include "Standard.h"
#include "Platform.h"

internal
void Main(console* console)
{
	ConsoleWriteLineF(
		console,
		"Your secret number is: %i.", 26,
		42
	);

	ConsoleWriteLine(console, "Goodbye.", 8);

	while (1)
	{
		BlitConsole(console);
	}
}
