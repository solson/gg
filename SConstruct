import os

env = Environment(
    CXX = 'clang++',
    CPPFLAGS = [
        '-Wall',
        '-Wextra',
        '--std=c++11',
    ],

    # Allow clang++ to use color.
    ENV = {'TERM': os.environ['TERM']},
)

env.ParseConfig('pkg-config --libs --cflags glew sdl2')

env.Program(
    target = 'gg',
    source = [
        'main.cpp',
    ],
)

debug = ARGUMENTS.get('debug', 0)
if int(debug):
   env.Append(CPPFLAGS = ['-g'])

# Print C++ flags for the YouCompleteMe vim plugin.
if ARGUMENTS.get('ycm', 0):
    print(env.subst('$CXXFLAGS $CCFLAGS $_CCCOMCOM'))
    exit()
