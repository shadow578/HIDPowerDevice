# pre- build script to patch the arduino core to allow for disabling CDC
# based on https://github.com/arduino/ArduinoCore-avr/pull/383

from os.path import join, isfile

Import("env")

print("applying core patches...")

#PATCH_EXE = "patch" # general
PATCH_EXE = "\"C:\\Program Files\\Git\\usr\\bin\\patch.exe\"" # wingit

CORE_DIR = join(env.PioPlatform().get_package_dir("framework-arduino-avr"), "cores", "arduino")
patchflag_path = join(CORE_DIR, ".cdc_patch")

def apply_patch(original_file, patch_file):
    assert isfile(original_file) and isfile(patch_file)
    env.Execute("%s %s %s" % (PATCH_EXE, original_file, patch_file))

    def _touch(path):
        with open(path, "w") as fp:
            fp.write("")

    env.Execute(lambda *args, **kwargs: _touch(patchflag_path))

# patch files only if we didn't do it before
if not isfile(patchflag_path):
    apply_patch(join(CORE_DIR, "CDC.cpp"), join("patches", "CDC.cpp.patch"))
    apply_patch(join(CORE_DIR, "USBCore.cpp"), join("patches", "USBCore.cpp.patch"))
    apply_patch(join(CORE_DIR, "USBDesc.h"), join("patches", "USBDesc.h.patch"))

print("done patching!")