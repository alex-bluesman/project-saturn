# Configuration

For now the QEMU BSP doesn't support SMP and therefor can't run multiple guests. That's why let's
use some pre-defined configuration for testing and development purposes:

 - Number of CPUs: 1
 - Size of QEMU machine RAM: 1024MB

# Memory Layout

 0x00000000 +----------------+     Memory _START_
            |                |
                    ...            I/O device memory
            |                |
 0x40000000 +----------------+ 
            |                |
                    ...            Guest OS memory: 1020MB
            |                |
 0x7fc00000 +----------------+
            |                |
            |     Saturn     |     Hypervisor kernel memory: 4MB
            |                |
 0x80000000 +----------------+     Memory _END_
