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

#ifndef __ASM_SYS_CTRL_H__
#define __ASM_SYS_CTRL_H__

#define MPIDR_UP	(1 << 30)		/* Multiprocessing extension:
						 *   0 - Processor is part of cluster
						 *   1 - Processor is uniprocessor
						 */
#define MPIDR_CPUID	(0x3)			/* Indicates the processor number */

#endif /* __ASM_SYS_CTRL_H__ */
