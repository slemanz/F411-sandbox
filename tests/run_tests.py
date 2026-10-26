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
    sys.exit(1)

def pc_read(jlink):
    for index in jlink.register_list():
        if jlink.register_name(index).startswith("R15"):
            return jlink.register_read(index)

def function_at(elf, address):
    addr2line = subprocess.run(["arm-none-eabi-addr2line", "-f", "-e", elf, hex(address)],
                                capture_output=True, text=True, check=True)

    return addr2line.stdout.splitlines()[0]

def mem_string_read(jlink, address):
    data = bytes(jlink.memory_read8(address, 64))
    return data.split(b"\0")[0].decode()


def main():
    if len(sys.argv) != 2:
        print("Error in use")
        sys.exit(1)

    elf = sys.argv[1]
    results = symbol_address_get(elf, "ttest_results")

    jlink = pylink.JLink()
    jlink.open()
    jlink.set_tif(pylink.enums.JLinkInterfaces.SWD)
    jlink.connect(DEVICE, speed=4000)

    jlink.flash_file(elf.replace(".elf", ".bin"), 0x08000000)

    jlink.reset(halt=True)
    jlink.memory_write32(results, [0, 0]) # clear magic/state left by any previous run
    jlink.restart()

    deadline = time.time() + TIMEOUT_S
    while (time.time() < deadline):
        magic, state = jlink.memory_read32(results, 2)
        if magic == MAGIC and state == STATE_DONE:
            break
        time.sleep(1)
    else:
        jlink.halt()
        pc = pc_read(jlink)

        print(f"TIMEOUT: stuck in {function_at(elf, pc)} (pc = 0x{pc:08X})")
        sys.exit(1)

    _, _, total, failed, test, file, line, expected, actual = jlink.memory_read32(results, 9)

    if failed:
        print(f"{mem_string_read(jlink, file)}:{line}: {mem_string_read(jlink, test)}: "
              f"expected 0x{expected:08X}, got 0x{actual:08X}")

    print(f"{total} tests, {failed} failed")
    sys.exit(1 if failed else 0)



if __name__ == "__main__":
    main()