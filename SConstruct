buildType = 'debug'
include = '#export/$BUILDTYPE/include'
lib = '#export/$BUILDTYPE/lib'
bin = '#export/$BUILDTYPE/bin'

env = Environment(BUILDTYPE = buildType,
                  INCDIR = include,
                  LIBDIR = lib,
                  BINDIR = bin)

env.Append(CCFLAGS = '-g -std=c++11')
#env.Append(CCFLAGS = '-stdlib=libc++')
#env.Append(LINKFLAGS = '-lc++')
env.Replace(CXX = 'clang++')

import os
env['EXE_NAME'] = os.path.basename(os.getcwd())

Export('env')
env.SConscript('src/SConscript', variant_dir='build/$BUILDTYPE')
