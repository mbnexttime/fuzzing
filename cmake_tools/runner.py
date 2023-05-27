import json
import subprocess
import os
import sys

with open(sys.argv[1]) as compileCommandsJson:
    commands = json.load(compileCommandsJson)
    
    for command in commands:
        os.chdir(command['directory'])
        os.system(command['command'])
