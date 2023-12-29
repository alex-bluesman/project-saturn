#!/usr/bin/env python3

# Helper script to compile Saturn configuration binary
# Copyright (C) 2023 Alexander Smirnov <alex.bluesman.smirnov@gmail.com>

import argparse
import json
import sys

def os_to_id(name):
    return {
        'linux'    : 'Linux',
        'asteroid' : 'Default',
    }[name]

def mmap_to_id(name):
    return {
        'device' : 'Device',
        'normal' : 'Normal',
    }[name]

def parse_vm_configuration(partition):
    content = 'static void VM_Configuration(core::IVirtualMachineConfig& vmConfig)\n'
    content += '{\n'

    content += '    // IPA memory mapping\n\n'

    for mem in partition['memory']:
        content += '    vmConfig.VM_Assign_Memory_Region({'
        content += mem['pa'] + ', '
        content += mem['va'] + ', '
        content += mem['size'] + ', '
        content += 'core::MMapType::' + mmap_to_id(mem['type'])
        content += '});\n'

    content += '\n    // INT mapping\n\n'

    for intr in partition['interrupts']:
        content += '    vmConfig.VM_Assign_Interrupt('
        content += str(intr['nr'])
        content += ');\n'

    content += '\n    // Boot address\n'
    content += '    vmConfig.VM_Set_Entry_Address('
    content += partition['entry']
    content += ');\n'

    content += '}\n\n'

    return content

def parse_os_storage(partition):
    content = 'static void OS_Storage_Configuration(OS_Storage& osStorage)\n'
    content += '{\n'

    content += '    // OS storage table\n'

    for img in partition['images']:
        content += '    osStorage.Add_Image('
        content += img['store'] + ', '
        content += img['boot'] + ', '
        content += img['size']
        content += ');\n'

    content += '\n    // Guest OS type\n'
    content += '    osStorage.Set_OS_Type(OS_Type::'
    content += os_to_id(partition['system'])
    content += ');\n'

    content += '}\n\n'

    return content

def parse_partition(partition):
    try:
        print ('[GEN]    partition os: ' + partition['system'])

        content = parse_vm_configuration(partition)
        content += parse_os_storage(partition)

    except KeyError:
        sys.exit ('error: failed to parse partition cofiguration')

    return content

def parse_input(inputfile):
    content = ''

    with open(inputfile) as f:
        data = json.load(f)

        try:
            version = data['version']
            print ('[GEN] found configuration version: ' + version)
        except KeyError:
            sys.exit ('error: cannot find configuration file version')

        try:
            nr_partitions = 0
            for partition in data['partitions']:
                nr_partitions += 1
                print('[GEN] parsing partition ' + str(nr_partitions))
                content += parse_partition(partition)
        except KeyError:
            sys.exit ('error: cannot find partition configuration')

    return content

def license_header():
    content  = '// Copyright (C) 2023 Alexander Smirnov <alex.bluesman.smirnov@gmail.com>\n'
    content += '//\n'
    content += '// Licensed under the MIT License (the "License"); you may not use this file except\n'
    content += '// in compliance with the License. You may obtain a copy of the License at\n'
    content += '//\n'
    content += '// http://opensource.org/licenses/MIT\n'
    content += '//\n'
    content += '// Unless required by applicable law or agreed to in writing, software distributed\n'
    content += '// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR\n'
    content += '// CONDITIONS OF ANY KIND, either express or implied. See the License for the\n'
    content += '// specific language governing permissions and limitations under the License.\n'
    content += '\n'

    return content

def cpp_header():
    content  = '#pragma once\n\n'
    content += '// (!) GENERATED CODE, DO NOT MODIFY (!)\n\n'
    content += 'namespace saturn {\n'
    content += 'namespace bsp {\n'
    content += 'namespace generated {\n\n'

    return content

def cpp_footer():
    content  = '}; // namespace generated\n'
    content += '}; // namespace bsp\n'
    content += '}; // namespace saturn'

    return content

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--input', help='specify configuration input file', required=True)
    parser.add_argument('-o', '--output', help='specify configuration output file', default='saturn_config.hpp')

    args = parser.parse_args()

    content = parse_input(args.input)
    with open(args.output, "w") as f:
        f.write(license_header())
        f.write(cpp_header())
        f.write(content)
        f.write(cpp_footer())
