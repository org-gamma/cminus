
import os
import shutil
import subprocess
import sys

# Configuration
BUILD_DIR = "build"
CMAKE_GENERATOR = "MinGW Makefiles"

def run_command(command, description):
    print(f"\n[BUILD] {description}")
    print(">", " ".join(command))

    try:
        subprocess.run(command, check=True)
        print(f"[SUCCESS] {description}")
        return True

    except FileNotFoundError:
        print(f"[ERROR] Command not found: {command[0]}")
        return False

    except subprocess.CalledProcessError as error:
        print(f"[FAILED] {description}")
        print(f"Exit code: {error.returncode}")
        return False


def clean_build():
    print("\n[CLEAN] Removing previous build...")

    if os.path.isdir(BUILD_DIR):
        shutil.rmtree(BUILD_DIR)
        print("[SUCCESS] Build directory removed.")
    else:
        print("[INFO] No previous build directory.")

    os.makedirs(BUILD_DIR, exist_ok=True)


def main():
    print("CMinus Build System")
    print("===================")

    # Step 1: Clean
    clean_build()

    # Step 2: Configure CMake
    if not run_command(
        ["cmake", "-S", ".", "-B", BUILD_DIR, "-G", CMAKE_GENERATOR],
        "Configuring CMinus"
    ):
        sys.exit(1)

    # Step 3: Build CMinus compiler
    if not run_command(
        ["cmake", "--build", BUILD_DIR],
        "Building CMinus compiler"
    ):
        sys.exit(1)

    # Step 4: Compile standard libraries
    if not run_command(
        [sys.executable, "CompileLibraries.py"],
        "Compiling CMinus libraries"
    ):
        sys.exit(1)

    print("\n===================")
    print("[SUCCESS] CMinus build completed!")
    print(f"Compiler: {BUILD_DIR}/cminus.exe")
    print("Libraries: lib/build/")
    print("===================")


if __name__ == "__main__":
    main()
