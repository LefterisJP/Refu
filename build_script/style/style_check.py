#!/usr/bin/env python2.7
import os
import subprocess
import argparse
import sys


from comments_style import reformat_file_comments

_dirname = os.path.dirname(os.path.realpath(__file__))
_diff_exe = "colordiff"

def gen_diff_call(file_name):
    if _diff_exe == "colordiff":
        return [_diff_exe,
                "--show-c-function",
                "-u",
                "{}.orig".format(file_name),
                file_name
            ]
    else:
        return [_diff_exe,
                "-u",
                "{}.orig".format(file_name),
                file_name
            ]        
    

def parse_arguments():
    parser = argparse.ArgumentParser(description='Astyle wrapper script')
    parser.add_argument('-s', '--check-style', action="store_true",
                        help='checks for style violations')
    parser.add_argument('-r', '--reformat-style', action="store_true",
                        help='Reformats style')
    parser.add_argument('-c', '--check-comments', action="store_true",
                        help='checks the style of comments above functions')
    parser.add_argument(dest="file_name",
                        help="The name of the file to work on")
    args = parser.parse_args()
    return args

def is_on_path(program):
    """Checks if an executable is on the path and can be executed"""
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    for path in os.environ["PATH"].split(os.pathsep):
        path = path.strip('"')
        exe_file = os.path.join(path, program)
        if is_exe(exe_file):
            return True

    return False

def sanity_check():
    global _diff_exe
    """Makes sure we have the programs needed"""
    if not is_on_path("astyle"):
        print ("astyle can not be found in the path. Quitting ...")
        return False
    if not is_on_path("colordiff"):
        print ("colordiff can't be found in the path. Will use diff")
        _diff_exe = "diff"
    return True


def call_astyle(file_name):
    """
    Calls astyle with the arguments specified in the options file
    which should be in the same directory as the executable
    """
    if not os.path.isfile(file_name):
        print("{} could not be found".format(file_name))
        return False

    try:
        rc = subprocess.call([
            "astyle",
            "--options={}/astyle_options".format(_dirname),
            file_name]
        )
    except OSerror as e:
        if e.errno == os.errno.ENOENT:
            print("astyle could not be found in the path")
        else:
            print("There was a problem invoking the astyle command")
        return False

    if rc != 0:
        print("astyle command returned error")
        return False

    return True

def check_style(file_name):
    """Checks style violations and provides a diff"""
    if not call_astyle(file_name):
        return False

    if not os.path.isfile("{}.orig".format(file_name)):
        print("Woohoo! Go you! No style violations!")
        return True

    l = gen_diff_call(file_name)
    try:
        subprocess.call(l)
    except:
        print("There was a problem invoking the diff command")
        return False

    os.rename("{}.orig".format(file_name), file_name)
    return True




def reformat_with_astyle(file_name, keep_original=False):
    """Reformats the file using astyle"""
    if not call_astyle(file_name):
        return False

    if not keep_original:
        os.remove("{}.orig".format(file_name))




        

    


if __name__ == '__main__':
    if not sanity_check():
        sys.exit(1)

    args = parse_arguments()
    if args.check_style:
        check_style(os.path.abspath(args.file_name))

    if args.reformat_style:
        reformat_style(os.path.abspath(args.file_name))

    if args.check_comments:
        reformat_file_comments(os.path.abspath(args.file_name))
        
