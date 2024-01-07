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

#pragma once

namespace saturn {
namespace apps {

class CommandLine
{
public:
	CommandLine();

public:
	void Start_Loop(void);

private:
	bool Parse_Command(char*);
	bool Str_Cmp(const char*, const char*);

// Commands
private:
	void Do_Help(void);
	void Do_Bad_Command(void);
	void Do_Vm(const char*);

#ifdef ENABLE_TESTING
private:
	void Do_Test_Adapter(const char*);
#endif // ENABLE_TESTING
};

}; // namespace apps
}; // namespace saturn
