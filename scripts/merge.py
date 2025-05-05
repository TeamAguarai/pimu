import os
import sys
import platform
import getpass # linux
import datetime
import re
from collections import defaultdict

def get_system_info(output_file, input_folder, files):
    """Retrieve system information for logging purposes."""
    date_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    os_info = platform.system() + " " + platform.release()
    user_name = getpass.getuser()
    files_list = "\n".join(files)
    return (f"File: {output_file}\nCreated by: {user_name}\nDate: {date_time}\n"
            f"OS: {os_info}\nFolder: {input_folder}\n\n"
            f"Merged Files:\n{files_list}\n")

def extract_includes(file_path):
    """Extracts #include dependencies from a file."""
    includes = set()
    include_pattern = re.compile(r'#include\s+"(.+?)"')

    with open(file_path, "r", encoding="utf-8") as file:
        for line in file:
            match = include_pattern.match(line)
            if match:
                includes.add(match.group(1))  # Extract the included file name
    return includes

def resolve_dependencies(file, dependencies, resolved, unresolved, input_folder):
    """Recursively resolves dependencies using Topological Sorting."""
    if file in resolved:
        return
    if file in unresolved:
        print(f"Error: Circular dependency detected in {file}.")
        sys.exit(1)

    unresolved.add(file)

    for dep in dependencies[file]:
        dep_path = os.path.join(input_folder, dep)
        if os.path.isfile(dep_path):  # Only resolve if the file exists
            resolve_dependencies(dep, dependencies, resolved, unresolved, input_folder)

    unresolved.remove(file)
    resolved.append(file)

def find_file_order(input_folder, files):
    """Determines the correct order of files based on #include dependencies using recursive resolution."""
    dependencies = defaultdict(set)  # {file: {dependencies}}
    file_paths = {file: os.path.join(input_folder, file) for file in files}

    # Build dependency graph
    for file in files:
        includes = extract_includes(file_paths[file])
        dependencies[file].update(includes)

    # Resolve dependencies
    resolved = []
    unresolved = set()

    for file in files:
        resolve_dependencies(file, dependencies, resolved, unresolved, input_folder)

    return resolved

def merge_files(input_folder, output_file, file_extension):
    """Merges files with correct dependency order."""
    if not os.path.isdir(input_folder):
        print(f"Error: The folder '{input_folder}' does not exist.")
        return

    files = [f for f in os.listdir(input_folder) if f.endswith(file_extension)]
    
    if not files:
        print("No matching files found in the specified folder.")
        return

    sorted_files = find_file_order(input_folder, files)  # Get ordered list
    system_info = get_system_info(output_file, input_folder, sorted_files)
    
    print("System Information:")
    print(system_info)
    
    with open(output_file, "w", encoding="utf-8") as out_file:
        out_file.write(f"/* {system_info}*/\n")

        included_files = set()  # Prevent duplicate includes

        for file in sorted_files:
            if file in included_files:
                continue  # Skip if already included

            included_files.add(file)
            file_path = os.path.join(input_folder, file)
            out_file.write(f"\n// ===== {file} =====\n")
            
            with open(file_path, "r", encoding="utf-8") as in_file:
                out_file.write(in_file.read() + "\n")
    
    print(f"Successfully merged {len(sorted_files)} files into '{output_file}'.")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python merge.py <input_folder> <file_extension> <output_file>")
        print("Use '*' as <file_extension> to merge all files in the folder.")
    else:
        input_folder = sys.argv[1]
        file_extension = sys.argv[2]
        output_file = sys.argv[3]
        merge_files(input_folder, output_file, file_extension)
