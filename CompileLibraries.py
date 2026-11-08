
import os
import subprocess
import sys

# Configuration
COMPILER = "g++"
STANDARD = "c++17"

LIBRARY_DIR = "lib"
SOURCE_DIR = os.path.join(LIBRARY_DIR, "src")
BUILD_DIR = os.path.join(LIBRARY_DIR, "build")


def compile_library(source_file):
    library_name = os.path.splitext(os.path.basename(source_file))[0]

    output_file = os.path.join(
        BUILD_DIR,
        library_name + ".o"
    )

    command = [
        COMPILER,
        "-std=" + STANDARD,
        "-Wall",
        "-Wextra",
        "-I", LIBRARY_DIR,
        "-c", source_file,
        "-o", output_file
    ]

    print(f"\n[BUILD] Compiling {library_name}...")

    try:
        subprocess.run(command, check=True)

        print(f"[SUCCESS] {library_name} compiled successfully.")
        return True

    except FileNotFoundError:
        print("[ERROR] g++ was not found. Check your PATH.")
        return False

    except subprocess.CalledProcessError:
        print(f"[FAILED] Could not compile {library_name}.")
        return False


def main():
    os.makedirs(BUILD_DIR, exist_ok=True)

    if not os.path.isdir(SOURCE_DIR):
        print(f"[ERROR] Source directory not found: {SOURCE_DIR}")
        sys.exit(1)

    source_files = [
        os.path.join(SOURCE_DIR, file)
        for file in os.listdir(SOURCE_DIR)
        if file.endswith(".cpp")
    ]

    if not source_files:
        print("[INFO] No library source files found.")
        return

    print("CMinus Library Compiler")
    print("----------------------")

    successful = 0
    failed = 0

    for source_file in sorted(source_files):
        if compile_library(source_file):
            successful += 1
        else:
            failed += 1

    print("\n----------------------")
    print(f"Successful: {successful}")
    print(f"Failed: {failed}")

    if failed:
        sys.exit(1)


if __name__ == "__main__":
    main()
