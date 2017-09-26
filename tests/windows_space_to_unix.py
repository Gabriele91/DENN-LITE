from os import getcwd
from os import listdir
from os import path
from os.path import isfile, join

local_path = getcwd()
files_of_path = [join(local_path, file) for file in listdir(local_path) if isfile(join(local_path, file)) and (path.splitext(file)[1] == ".cpp" or path.splitext(file)[1] == ".h")]

print("files:", files_of_path)


for filename in files_of_path:
    file_data_n = ""
    with open(filename, "rb") as file:
        file_data_n = file.read().decode("utf-8").replace('\r\n', '\n')
    with open(filename, "wb") as file:
        file.write(file_data_n.encode("utf-8"))
