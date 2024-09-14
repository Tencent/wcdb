import os
import sys

def has_header_files(path):
    for entry in os.scandir(path):
        if entry.is_file() and (entry.name.endswith('.h') or entry.name.endswith('.hpp')):
            return True
    return False

if len(sys.argv) != 2:
    print("用法: python script.py <目录路径>")
    sys.exit(1)

directory_path = sys.argv[1]

for root, dirs, files in os.walk(directory_path):
    dirs[:] = [dir for dir in dirs if 'test' not in dir.lower() and 'support' not in dir.lower() and 'java' not in dir.lower() and 'swift' not in dir.lower() and 'xcodeproj' not in dir.lower()]
    for dir in dirs:
        dir_path = os.path.join(root, dir)
        if has_header_files(dir_path):
            print(os.path.relpath(dir_path, directory_path))