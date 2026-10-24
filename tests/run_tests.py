import subprocess
import sys
import time

import pylink

DEVICE     = "STM32F411CE"
MAGIC      = 0x7E57C0DE
STATE_DONE = 2
TIMEOUT_S  = 5

def main():
    if len(sys.argv) != 2:
        print("Error in use")
        sys.exit()

    elf = sys.argv[1]
    print(elf)
    print("Hello")

if __name__ == "__main__":
    main()