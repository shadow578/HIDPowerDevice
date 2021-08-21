Import("env")

print("patching hwid...")
env.BoardConfig().update("build.hwids", [
    ["0x0463", "0xffff"] # VID, PID
])

print("done.")
