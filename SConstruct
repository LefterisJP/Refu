import os
import platform
import subprocess

SConscript('build_script/compilers.py')
SConscript('build_script/modules.py')


from build_script.modules import setup_modules
from build_script.compilers import setupCompiler
from build_script.configvariables import setupConfigVars
from build_script.cleanup import clean_generated_files
from build_script.code_gen import CodeGen

sourceDir = 'src'
targetSystem = platform.system()
allowedCompilers = ['gcc', 'tcc', 'msvc']
legalBuildTargets = [
    'shared',
    'static',
    'test',
    'check']

# read the configuration file variable and the extra objects
(config_file, extra_objects, refu_dir, args_before) = SConscript(
    'build_script/args_before_config.py')
# create the code_gen object and add all the extra objects
code_gen = CodeGen(refu_dir)
# generate the extra sources for any extra objects
# they should later be copied into the project that wants to use them
for o in extra_objects:
    code_gen.add_object(o, refu_dir)


# Create an intermediate environment to read the build variables
vars = SConscript('build_script/variables.py',
                  exports='allowedCompilers config_file code_gen')
temp = Environment(variables=vars)

# create the building environment and setup the compiler
env = Environment()
setupCompiler(env, targetSystem, temp)

# setting needed flags, paths and defines
env.Append(CPPDEFINES={'REFU_COMPILING': None})
if(temp['__TEST_BUILD']):
    env.Append(CPPDEFINES={'REFU_TEST': None})
env.Append(CPPPATH=os.path.join(refu_dir, 'include'))
env.Append(CCFLAGS=temp['COMPILER_FLAGS'])
env.Append(LINKFLAGS=temp['LINKER_SHARED_FLAGS'])

# setup the required modules
(modules, orig_sources) = setup_modules(temp, env, targetSystem,
                                        refu_dir, code_gen)

# setup the variables of the configuration file
setupConfigVars(temp, env)

# Perform the system check
compiler = temp['COMPILER']
system_attributes = SConscript('scripts/systemcheck/systemcheck.py',
                               exports='compiler')

# only if actually building create the options file
# This condition is here only so that the old style, deprecated tests,
# can work
if 'shared' in COMMAND_LINE_TARGETS or 'static' in COMMAND_LINE_TARGETS:
    SConscript('build_script/options.py',
               exports='modules env targetSystem system_attributes refu_dir')

outName = temp['OUTPUT_NAME']

# only if actually building set the obj dir and prepend obj dir to all sources
env.VariantDir(temp['OBJ_DIR'], sourceDir, duplicate=0)
# a list comprehension prepending the obj dir to all of the sources
# (instead of the sourcedir) ... Scons peculiarity
sources = [temp['OBJ_DIR']+'/'+s for s in orig_sources]


# -- From here and on check build targets
# If clean is specified make sure that we delete all of the generated files
if env.GetOption('clean'):
    clean_generated_files(refu_dir, code_gen)

# -- STATIC LIBRARY
cppdefines_static = env['CPPDEFINES']
cppdefines_static['REFU_STATIC_LIB'] = None
static = env.StaticLibrary(outName, sources, CPPDEFINES=cppdefines_static)
env.Alias('static', static)

# -- SHARED LIBRARY
cppdefines_shared = env['CPPDEFINES']
cppdefines_shared['REFU_DYNAMIC_LIB'] = None
shared = env.SharedLibrary(outName, sources, CPPDEFINES=cppdefines_shared)
env.Alias('shared', shared)


# -- UNIT TESTS
unit_tests_files = [
    'test_main.c',
    'utilities_for_testing.c',
    'test_string_core.c',
    'test_string_conversion.c',
    'test_string_retrieval.c',
    'test_string_files.c',
    'test_string_manipulation.c',
    'test_string_traversal.c',

    'test_utils_unicode.c',
    'test_utils_array.c',

    'test_io_files.c',
    'test_io_textfile.c',

    'test_log.c'
]
unit_tests_files = ['test/' + s for s in unit_tests_files]
unit_tests_files.extend(['src/' + s for s in orig_sources])
libs_check = env['LIBS']
libs_check.append('check')
cppdefines_check = env['CPPDEFINES']
cppdefines_check['RF_OPTION_DEBUG'] = None
cppdefines_check['RF_OPTION_INSANITY_CHECKS'] = None
try:
    del cppdefines_check['NDEBUG']
except:
    pass
program = env.Program('check', unit_tests_files, LIBS=libs_check,
                      CPPDEFINES=cppdefines_check)

test_run = env.Command(
    target="test_run",
    source='check',
    action="./check {} {}".format(
        temp['UNIT_TESTS_OUTPUT'],
        temp['UNIT_TESTS_FORK']))
check_alias = Alias('check', [test_run])
# Simply required.  Without it, 'check' is never considered out of date.
AlwaysBuild(check_alias)

# If we have valgrind also run the unit tests through valgrind
if system_attributes['has_valgrind']:
    valgrind_cmd = ("valgrind --tool=memcheck "
                    "--leak-check=yes "
                    "--track-origins=yes "
                    "--show-reachable=yes "
                    "--num-callers=20 "
                    "--track-fds=yes")
    test_run_val = env.Command(
        target="test_run_val",
        source='check',
        action="{} ./check {} {}".format(
            valgrind_cmd,
            temp['UNIT_TESTS_OUTPUT'],
            False))  # do not fork tests when running in valgrind
    check_alias_val = Alias('check_val', [test_run_val])
    # Simply required.  Without it, 'check' is never considered out of date.
    AlwaysBuild(check_alias_val)

# generate help text for the variables
Help(vars.GenerateHelpText(env))
Help(args_before.GenerateHelpText(env))
