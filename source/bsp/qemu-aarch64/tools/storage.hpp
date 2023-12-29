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

#include <bsp/os_storage>

namespace saturn {
namespace bsp {

class OS_Storage
{
public:
	OS_Storage();
	~OS_Storage();

public:
	void Add_Image(uint64_t source, uint64_t target, size_t size);
	void Load_Images(void);
	void Set_OS_Type(OS_Type type);
	OS_Type Get_OS_Type(void);

private:
	// OS storage configuration
	size_t nrImages;
	OS_Storage_Entry (&osImages)[];
	OS_Type osType;
};

}; // namespace bsp
}; // namespace saturn
