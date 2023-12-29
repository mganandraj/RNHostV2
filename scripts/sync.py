import os
import shutil

OMR = "D:\\ogit6\\src"
RNH = "D:\\github\\RNHostv2"

# add an enum to specify the direction of the copy
class Direction:
    OMR_TO_RNH = 1
    RNH_TO_OMR = 2

direction = Direction.OMR_TO_RNH
# direction = Direction.RNH_TO_OMR

# List of entries organized as [OMR Path, RNH Path, [list of exclusions]]
DIR_ENTRIES = [
    ["reactnativehost\\androidjava\\src\\com\\microsoft\\office\\reacthost", "reacthost\\src\\main\\java\\com\\microsoft\\office\\reacthost", [""]],
    ["reactnative\\android\\host\\java\\src\\com\\microsoft\\office\\reactnative\\host", "rnxreacthost\\src\\main\\java\\com\\microsoft\\office\\reactnative\\host", [""]],
    ["reactnative\\android\\host\\cpp", "rnxreacthost\\src\\main\\cpp", ["souces", "version.expmap", "precomp.h"]],
    ["reactnative\\android\\reka\\java\\src\\com\\microsoft\\office\\reactnative\\reka", "reactreka\\src\\main\\java\\com\\microsoft\\office\\reactnative\\reka", [""]],
    ["reactnative\\android\\reka\\cpp", "reactreka\\src\\main\\cpp", ["souces", "version.expmap", "msoFolly", "precomp.h"]],
]

FILE_ENTRIES = [
    ["reactnative\\androidhost\\cpp\\ReactInstanceAndroid.cpp", "reacthost\\src\\main\\cpp\\ReactInstanceAndroid.cpp"],
    ["reactnativehost\\ReactInstanceAndroidDeferred.cpp", "reacthost\\src\\main\\cpp\\ReactInstanceAndroidDeferred.cpp"],
    ["reactnativehost\\ReactInstanceAndroidDeferred.h", "reacthost\\src\\main\\cpp\\ReactInstanceAndroidDeferred.h"],
]

# List of RN binaries to copy
RN_BINARY_SOURCE_DIR = "D:\\nugetcache\\microsoft.office.reactnative.android.0.72.11-office-ndk25\\lib\\debug\\droidx86"
RN_BINARY_TARGET_DIR = "D:\\github\\RNHostv2\\rnxreacthost\\src\\main\\jniLibs\\x86"
RN_BINARIES = ['libfb.so', 'libglog_init.so', 'liblogger.so', 'libmapbufferjni.so', 'libreactnativeblob.so', 'libreactperfloggerjni.so', 'libreact_config.so', 'libreact_render_attributedstring.so', 'libreact_render_leakchecker.so', 'libreact_render_runtimescheduler.so', 'libreact_render_telemetry.so', 'libreact_render_templateprocessor.so', 'libreact_render_textlayoutmanager.so', 'libreact_utils.so', 'librrc_scrollview.so', 'librrc_text.so', 'librrc_textinput.so', 'librrc_unimplementedview.so']

def main():
   
    # for each entry
    for entry in DIR_ENTRIES:
        # copy files from OMR to RNH
        if direction == Direction.OMR_TO_RNH:
            # copy files from OMR to RNH
            shutil.copytree(os.path.join(OMR, entry[0]), os.path.join(RNH, entry[1]), ignore=shutil.ignore_patterns(*entry[2]), dirs_exist_ok=True)
        # copy files from RNH to OMR
        elif direction == Direction.RNH_TO_OMR:
            # copy files from RNH to OMR
            shutil.copytree(os.path.join(RNH, entry[1]), os.path.join(OMR, entry[0]), ignore=shutil.ignore_patterns(*entry[2]), dirs_exist_ok=True)
        else:
            print("Invalid direction")
            return

    # for each file entry
    for entry in FILE_ENTRIES:
        # copy files from OMR to RNH
        if direction == Direction.OMR_TO_RNH:
            # copy files from OMR to RNH
            shutil.copy(os.path.join(OMR, entry[0]), os.path.join(RNH, entry[1]))
        # copy files from RNH to OMR
        elif direction == Direction.RNH_TO_OMR:
            # copy files from RNH to OMR
            shutil.copy(os.path.join(RNH, entry[1]), os.path.join(OMR, entry[0]))
        else:
            print("Invalid direction")
            return

    if direction == Direction.OMR_TO_RNH:
        # copy RN binaries
        for binary in RN_BINARIES:
            shutil.copy(os.path.join(RN_BINARY_SOURCE_DIR, binary), os.path.join(RN_BINARY_TARGET_DIR, binary))


    print("Hello World!")
    pass

if __name__ == "__main__":
    main()
