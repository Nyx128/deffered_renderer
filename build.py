import os
import subprocess
import sys
import argparse
from pathlib import Path

def needs_recompile(source_file, obj_file, header_dirs):
    """Check if a source file needs to be recompiled."""
    if not obj_file.exists():
        return True
    
    obj_mtime = obj_file.stat().st_mtime
    
    # Check if source file is newer
    if source_file.stat().st_mtime > obj_mtime:
        return True
    
    # Check if any header files are newer
    # Simple approach: check all .h/.hpp files in include and src directories
    for header_dir in header_dirs:
        if header_dir.exists():
            for header in header_dir.rglob('*.h*'):  # Matches .h, .hpp, etc.
                if header.stat().st_mtime > obj_mtime:
                    return True
    
    return False

def main():
    parser = argparse.ArgumentParser(description='Build the project.')
    parser.add_argument('--mode', choices=['debug', 'release'], default='debug', help='Build mode (default: debug)')
    parser.add_argument('--clean', action='store_true', help='Clean build (recompile everything)')
    args = parser.parse_args()

    # Configuration
    src_dir = Path('src')
    include_dir = Path('include')
    lib_dir = Path('lib')
    int_dir = Path('int')
    build_dir = Path('build')
    output_file = build_dir / 'app.exe'

    # Compiler settings
    compiler = 'g++'
    base_flags = ['-std=c++17', '-Wall']
    
    if args.mode == 'debug':
        c_flags = base_flags + ['-g', '-O0']
        print("Building in DEBUG mode...")
    else:
        c_flags = base_flags + ['-O3', '-DNDEBUG']
        print("Building in RELEASE mode...")

    includes = [f'-I{include_dir}', f'-I{src_dir}']
    
    # Linker settings
    lib_paths = [f'-L{lib_dir}']
    libs = ['-lglfw3', '-lopengl32', '-lgdi32', '-luser32', '-lkernel32', '-lshell32']
    l_flags = ['-static']

    # Create directories
    if not int_dir.exists():
        int_dir.mkdir()
    if not build_dir.exists():
        build_dir.mkdir()

    # Clean build if requested
    if args.clean:
        print("Cleaning build artifacts...")
        for obj_file in int_dir.glob('*.o'):
            obj_file.unlink()
        if output_file.exists():
            output_file.unlink()

    # Find source files in src/
    sources = []
    for root, dirs, files in os.walk(src_dir):
        for file in files:
            if file.endswith('.cpp') or file.endswith('.c'):
                sources.append(Path(root) / file)

    # Add all .cpp files from include/ directory
    for root, dirs, files in os.walk(include_dir):
        for file in files:
            if file.endswith('.cpp') or file.endswith('.c'):
                sources.append(Path(root) / file)

    if not sources:
        print("No source files found!")
        return

    # Compile object files
    object_files = []
    compile_failed = False
    compiled_count = 0
    skipped_count = 0

    print("Starting compilation...")

    # Header directories to check for changes
    header_dirs = [include_dir, src_dir]

    for source in sources:
        # Flat structure: just use the filename
        obj_name = source.stem + '.o'
        obj_path = int_dir / obj_name
        
        object_files.append(str(obj_path))
        
        # Check if recompilation is needed
        if needs_recompile(source, obj_path, header_dirs):
            print(f"Compiling {source.name}...")
            
            cmd = [compiler] + c_flags + includes + ['-c', str(source), '-o', str(obj_path)]
            
            result = subprocess.run(cmd)
            if result.returncode != 0:
                compile_failed = True
                break
            compiled_count += 1
        else:
            print(f"Skipping {source.name} (up to date)")
            skipped_count += 1

    if compile_failed:
        print("\nBuild failed during compilation.")
        sys.exit(1)

    print(f"\nCompilation summary: {compiled_count} compiled, {skipped_count} skipped")

    # Link
    print("Linking...")
    link_cmd = [compiler] + object_files + lib_paths + l_flags + libs + ['-o', str(output_file)]
    
    result = subprocess.run(link_cmd)
    
    if result.returncode == 0:
        print(f"\nBuild successful! Output: {output_file}")
    else:
        print("\nLinking failed.")
        sys.exit(1)

if __name__ == '__main__':
    main()
