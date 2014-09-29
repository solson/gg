import os

def FlagsForFile(filename, **kwargs):
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    flags = os.popen('scons -Q ycm=1').read().split()

    # Force YCM to recognize all files as C++, including header files
    flags.extend(['-x', 'c++'])

    # Disgusting hack to get around libclang's failure to use the correct
    # lookup paths. These include directories were obtained from the command
    # `echo | clang -v -E -x c++ -`
    sys_includes = """
    /usr/bin/../lib64/gcc/x86_64-unknown-linux-gnu/4.9.1/../../../../include/c++/4.9.1
    /usr/bin/../lib64/gcc/x86_64-unknown-linux-gnu/4.9.1/../../../../include/c++/4.9.1/x86_64-unknown-linux-gnu
    /usr/bin/../lib64/gcc/x86_64-unknown-linux-gnu/4.9.1/../../../../include/c++/4.9.1/backward
    /usr/local/include
    /usr/bin/../lib/clang/3.4.2/include
    /usr/include
    """.split()

    for include in sys_includes:
        flags.extend(['-isystem', include])

    return {
        'flags': flags,
        'do_cache': True
    }
