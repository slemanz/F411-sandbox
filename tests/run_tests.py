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

def pc_read(jlink):
    for index in jlink.register_list():
        if jlink.register_name(index).startswith("R15"):
            return jlink.register_read(index)

def main():
    if len(sys.argv) != 2:
        print("Error in use")
        sys.exit()

    elf = sys.argv[1]
    results = symbol_address_get(elf, "ttest_results")

    jlink = pylink.JLink()
    jlink.open()
    jlink.set_tif(pylink.enums.JLinkInterfaces.SWD)
    jlink.connect(DEVICE, speed=4000)

    jlink.flash_file(elf.replace(".elf", ".bin"), 0x08000000)

    jlink.reset(halt=True)
    jlink.memory_write32(results, [0, 0]) # clear magic/state lef by any previous run
    jlink.restart()

    deadline = time.time() + TIMEOUT_S
    while (time.time() < deadline):
        magic, state = jlink.memory_read32(results, 2)
        if magic == MAGIC and state == STATE_DONE:
            break
        time.sleep(1)
    else:
        jlink.halt()

        print("TIMEOUT: stuck in", hex(pc_read(jlink)))

    print("END")



if __name__ == "__main__":
    main()