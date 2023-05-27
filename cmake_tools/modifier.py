import json
import sys


def processCompileCommands(
        compileCommandsPath,
        llvmOutputPath,
        newCommandsPath,
        llvmPassPath,
        logopLLVMPath,
):
    with open(compileCommandsPath, "r") as compileCommandsJson:
        with open(llvmOutputPath, "w") as llvmOutputs:
            with open(newCommandsPath, "w") as modifiedCommandsJson:
                newCommands = []
                compileCommands = json.load(compileCommandsJson)
                for command in compileCommands:
                    commandArgs = command["command"].split()
                    outputFile = commandArgs[commandArgs.index("-o") + 1]
                    llvmIROutputFile = outputFile.replace(".o", ".ll")
                    commandArgs[commandArgs.index("-o") + 1] = llvmIROutputFile
                    commandArgs.append("-emit-llvm")
                    commandArgs.append("-fno-use-cxa-atexit")

                    outputCommand = command["output"].replace(".o", ".ll")

                    print(outputCommand, file=llvmOutputs)

                    modifiedOutput = f'{outputCommand}.modified.ll'

                    linkedOutput = f'{outputCommand}.linked.ll'

                    newCommands.append(
                        {
                            "directory": command["directory"],
                            "command": " ".join(commandArgs),
                            "file": command["file"],
                            "output": outputCommand
                        }
                    )
                    newCommands.append(
                        {
                            "directory": command["directory"],
                            "command": f'/usr/bin/opt -load {llvmPassPath} -ast -enable-new-pm=0 {outputCommand} > {modifiedOutput}',
                            "file": outputCommand,
                            "output": modifiedOutput
                        }
                    )
                    newCommands.append(
                        {
                            "directory": command["directory"],
                            "command": f'/usr/bin/llvm-link  {logopLLVMPath} {modifiedOutput} -o {linkedOutput}',
                            "file": modifiedOutput,
                            "output": linkedOutput
                        }
                    )
                json.dump(newCommands, modifiedCommandsJson, indent=4)


processCompileCommands(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])