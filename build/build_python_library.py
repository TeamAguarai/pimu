import os
import subprocess
import shutil
import sys

OUT_DIR = "python-lib"
SO_FILE = "pimu.cpython-311-aarch64-linux-gnu.so"
INSTALL_DIR = "/usr/lib/python3/dist-packages/"
REPO_URL = "https://github.com/pybind/pybind11"
MERGE_SCRIPT = "../../scripts/merge.py"
INCLUDE_DIR = "../../include"

def ensure_cmake_installed():
    print("🔍 Verifying if cmake is installed...")
    if shutil.which("cmake") is None:
        print("⚠️  cmake not found. Installing cmake...")
        subprocess.run("sudo apt update && sudo apt install -y cmake", shell=True, check=True)
    else:
        print("✅ cmake is already installed.")

def install_wiringpi():
    print("🔧 Installing wiringPi (if not already installed)...")
    subprocess.run("wget -O - https://raw.githubusercontent.com/TeamAguarai/pampas/refs/heads/main/scripts/wiringPi.sh | bash", shell=True, check=True)

def clone_pybind11():
    if not os.path.exists("pybind11"):
        print("📥 Cloning pybind11...")
        subprocess.run(["git", "clone", REPO_URL], check=True)
    else:
        print("📦 pybind11 repository already present.")

def merge_headers():
    print("🧩 Generating merged pimu.hpp...")
    subprocess.run(f"python3 {MERGE_SCRIPT} {INCLUDE_DIR} .hpp pimu.hpp", shell=True, check=True)

def run_cmake_and_make():
    print("🛠️  Running cmake...")
    subprocess.run(["cmake", ".."], check=True)
    print("🏗️  Building with make...")
    subprocess.run(["make"], check=True)

def move_shared_object():
    dest_so_path = os.path.join(INSTALL_DIR, SO_FILE)

    if os.path.exists(dest_so_path):
        print(f"🧹 Removing previous version at {dest_so_path}...")
        os.remove(dest_so_path)

    if os.path.exists(SO_FILE):
        shutil.move(SO_FILE, dest_so_path)
        print(f"\n✅ pimu library successfully installed at {dest_so_path}")
        print("📦 You can now import 'pimu' in your Python projects.")
    else:
        print(f"\n❌ Could not find the file {SO_FILE}. Make sure the compilation completed successfully.")
        sys.exit(1)

def main():
    ensure_cmake_installed()
    os.makedirs(OUT_DIR, exist_ok=True)
    os.chdir(OUT_DIR)
    install_wiringpi()
    clone_pybind11()
    merge_headers()
    run_cmake_and_make()
    move_shared_object()

if __name__ == "__main__":
    main()
