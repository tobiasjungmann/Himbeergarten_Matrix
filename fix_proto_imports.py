import sys
import re

def replace_import_line(filename, replacement_string):
    with open(filename, 'r') as file:
        lines = file.readlines()

    pattern = r'^import .*_pb2 as .*$'
    with open(filename, 'w') as file:
        for line in lines:
            if bool(re.match(pattern, line)):
                print(line)
                file.write("import "+replacement_string +line[7:])
            else:
                file.write(line)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python replace_import.py <filename> <replacement_string>")
    else:
        filename = sys.argv[1]
        replacement_string = sys.argv[2]
        replace_import_line(filename, replacement_string)