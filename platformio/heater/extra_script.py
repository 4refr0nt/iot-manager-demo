from subprocess import call
Import("env")

print "Current build targets: ", map(str, BUILD_TARGETS)
print "Add build hook"
call(".\prebuild\gen_h.bat")
