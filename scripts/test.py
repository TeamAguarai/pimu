import os
import sys

def merge_files(cpp_file, hpp_file, output_folder):
    """Merges a .cpp and a .hpp file into a specified output folder and generates a Makefile."""
    
    if not os.path.isfile(cpp_file) or not cpp_file.endswith(".cpp"):
        print("Error: First file must be a .cpp file.")
        return
    
    if not os.path.isfile(hpp_file) or not hpp_file.endswith(".hpp"):
        print("Error: Second file must be a .hpp file.")
        return
    
    if not os.path.isdir(output_folder):
        os.makedirs(output_folder)
    
    # Copy the .cpp and .hpp files to the output folder
    cpp_filename = os.path.basename(cpp_file)
    hpp_filename = os.path.basename(hpp_file)
    cpp_output_path = os.path.join(output_folder, cpp_filename)
    hpp_output_path = os.path.join(output_folder, hpp_filename)
    
    with open(cpp_file, "r", encoding="utf-8") as src, open(cpp_output_path, "w", encoding="utf-8") as dst:
        dst.write(src.read())
    
    with open(hpp_file, "r", encoding="utf-8") as src, open(hpp_output_path, "w", encoding="utf-8") as dst:
        dst.write(src.read())
    
    # Extract base name without extension for the Makefile
    cpp_base_name = os.path.splitext(cpp_filename)[0]
    
    # Create Makefile
    makefile_content = f"""all:
	g++ {cpp_filename} -o {cpp_base_name}  
	./{cpp_base_name}
"""
    
    makefile_path = os.path.join(output_folder, "makefile")
    with open(makefile_path, "w", encoding="utf-8") as makefile:
        makefile.write(makefile_content)
    
    print(f"Successfully merged files into '{output_folder}' and created makefile.")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python test.py <cpp_file> <hpp_file> <output_folder>")
    else:
        merge_files(sys.argv[1], sys.argv[2], sys.argv[3])