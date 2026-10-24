import subprocess
import sys
import time

import pylink

DEVICE     = "STM32F411CE"
MAGIC      = 0x7E57C0DE
STATE_DONE = 2
TIMEOUT_S  = 5

def symbol_address_get(elf, name):
    nm = subprocess.run(["arm-none-eabi-nm", elf], capture_output=True, text=True, check=True)

    for line in nm.stdout.splitlines():
        fields = line.split()
        if fields[-1] == name:
            return int(fields[0], 16)

    print("Symbol", name, "not found in", elf)
    sys.exit()

def main():
    if len(sys.argv) != 2:
        print("Error in use")
        sys.exit()

    elf = sys.argv[1]
    results = symbol_address_get(elf, "ttest_results")
    print(hex(results))



if __name__ == "__main__":
    main()