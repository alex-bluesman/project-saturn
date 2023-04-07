// Copyright (C) 2023 Alexander Smirnov <alex.bluesman.smirnov@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// http://opensource.org/licenses/MIT
//
// Unless required by applicable law or agreed to in writing, software distributed 
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
// CONDITIONS OF ANY KIND, either express or implied. See the License for the 
// specific language governing permissions and limitations under the License.

#include "cmdline.hpp"

#include <core/iconsole>

namespace saturn {

namespace core {
	extern void VM_Start();
};

namespace apps {

using namespace core;

static const char* _prompt = "$ ";
static const size_t _cmd_max_len = 32;

// External API:
void TA_Start();

CommandLine::CommandLine()
{
	Info() << "command line application is started" << fmt::endl;
}

void CommandLine::Start_Loop(void)
{
	bool stopLoop = false;

	while (false == stopLoop)
	{
		Raw() << _prompt;

		char cmdInput[_cmd_max_len + 1] = {0,};
		size_t cursorPos = 0;
		bool pressEnter = false;

		do
		{
			char c = ConIO().GetChar();

			switch (c)
			{
			case 0x0d:				// '\n'
				pressEnter = true;
				Raw() << fmt::endl;
				break;

			case 0x08:				// '\b'
			case 0x7f:				// DEL for compatibility with MacBook
				if (cursorPos > 0)
				{
					Raw() << "\b \b";
					cursorPos--;
				}
				break;

			default:
				// Let's limit the scope only to human readable symbols from ASCII table
				if ((c >= 0x20) && (c < 0x7f))
				{	
					Raw() << c;
					if (cursorPos < _cmd_max_len)
					{
						cmdInput[cursorPos++] = c;
						cmdInput[cursorPos] = 0;
					}
				}
			}
		}
		while (false == pressEnter);

		stopLoop = Parse_Command(cmdInput);
	}
}

bool CommandLine::Parse_Command(char* cmdInput)
{
	bool doQuit = false;
	size_t n = 0;

	while (cmdInput[n] != 0)
	{
		if (cmdInput[n] == ' ')
		{
			cmdInput[n++] = 0;
			break;
		}

		n++;
	}

	char* cmdName = cmdInput;
	char* cmdArgs = &cmdInput[n];

	// Now we have:
	//  * cmdName is the command itself which should be parsed
	//  * cmdArgs are the command parameters

	// TBD: implement map to avoid ugly if else
	if (Str_Cmp(cmdName, "quit"))
	{
		doQuit = true;
	}
	else
	if (Str_Cmp(cmdName, "help"))
	{
		Do_Help();
	}
	else
	if (Str_Cmp(cmdName, "test"))
	{
		Do_Test_Adapter(cmdArgs);
	}
	else
	if (Str_Cmp(cmdName, "vm"))
	{
		Do_Vm(cmdArgs);
	}
	else
	{
		Do_Bad_Command();
	}

	return doQuit;
}

bool CommandLine::Str_Cmp(const char* firstLine, const char* secondLine)
{
	bool stopCmp = false;
	bool retVal = false;
	size_t n = 0;

	do
	{
		if (firstLine[n] != secondLine[n])
		{
			stopCmp = true;
		}
		else
		if (firstLine[n] == 0)
		{
			retVal = true;
			stopCmp = true;
		}
		else
		{
			n++;
		}
	}
	while (false == stopCmp);

	return retVal;
}

void CommandLine::Do_Help(void)
{
	Raw() << "Saturn Hypervisor console, please use the following commands:" << fmt::endl;
	Raw() << "  help        - display usage information" << fmt::endl;
	Raw() << "  quit        - stop console application" << fmt::endl;
	Raw() << "  test        - test adapter to run smoke tests" << fmt::endl;
	Raw() << "  vm          - virtual machine management" << fmt::endl;
	Raw() << fmt::endl;
}

void CommandLine::Do_Bad_Command(void)
{
	Raw() << "error: invalid command, please use 'help' to see list of support commands" << fmt::endl;
	Raw() << fmt::endl;
}

void CommandLine::Do_Test_Adapter(const char* args)
{
	if (Str_Cmp(args, "run"))
	{
		TA_Start();
	}
	else
	{
		Raw() << "error: 'test' arguments missed, please use 'run' to execute smoke tests" << fmt::endl;
	}

	Raw() << fmt::endl;
}

void CommandLine::Do_Vm(const char* args)
{
	if (Str_Cmp(args, "start"))
	{
		VM_Start();
	}
	else
	{
		Raw() << "error: 'vm' arguments missed, please use 'start' to run guest VM" << fmt::endl;
	}

	Raw() << fmt::endl;
}

}; // namespace apps
}; // namespace saturn
