#!/usr/bin/env python
import os.path
import sys


def gen_xmacro_add():
    # the maximum number from which addition can happen
    i = 64
    print("Generating rf_xmacro_add.h ...")
    f = open(os.path.dirname(
        sys.argv[0]) + "/../include/Preprocessor/rf_xmacro_add.h", "w")

    # put some required definitions at the top
    f.write("/**\n** @author Lefteris\n** @date 01/10/2015\n**\n\
**This file contains token definition for emulating addition with\n\
**the C preprocessor. It is automatically generated by a python script\n\
**gen_rf_xmacro_add.py\n*/")

    f.write("\n//! This is the macro from which addition will be achieved")
    f.write("\n#define RP_ADD(v1__,v2__)  i_ADD_##v1__##_##v2__\n\n")

    # the outermost while
    while (i > 0):
        f.write("//! These are the tokens for subtraction from "+str(i)+"\n")
        j = i
        # this is the individual while for which we get each subtraction from i
        while (j >= 0):
            f.write("#define i_ADD_{}_{}   {}\n".format(str(i), str(j), str(i + j)))
            j -= 1
        i -= 1
        f.write("\n")
    # at the end close the file
    print("rf_xmacro_add.h has been generated!")
    f.close()

if __name__ == '__main__':
    gen_xmacro_add()
