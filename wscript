import Options,sys,os,shutil
from os.path import exists, expanduser 
from os import symlink 

srcdir = "."
blddir = "build"
VERSION = "0.5.0"

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  conf.env.append_value('CXXFLAGS', ['-arch', 'x86_64'])
  conf.env.append_value('LDFLAGS', ['-arch', 'x86_64'])
  conf.check(
    staticlib='freeimage', 
    libpath=[expanduser('~')+'/code/FreeImage/Dist'],
    includes=[expanduser('~')+'/code/FreeImage/Source']
  )

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = "image"
  obj.source = bld.path.ant_glob('src/*.cpp')
  #obj.cxxflags = ["-g", "-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE","-fPIC","-Wall"]
  obj.linkflags = ['-arch','x86_64',expanduser('~')+'/code/FreeImage/Dist/libfreeimage.a']
 
def shutdown(bld):
  if Options.commands['clean']:
    if exists('node-image.node'): os.unlink('node-image.node')
  else:
    if exists('build/Release/node-image.node') and not exists('node-image.node'):
      symlink('build/Release/node-image.node', 'node-image.node')
