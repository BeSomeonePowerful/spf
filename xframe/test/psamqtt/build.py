#scripts for running mqttdemo
from subprocess import call
from os import system

call("make")
# call(["make"])
# call("export LD_LIBRARY_PATH='../../_bin/lib/'")
system("export LD_LIBRARY_PATH='../../_bin/lib/'")
call(["./mqttdemo"])
