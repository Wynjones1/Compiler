#!/usr/bin/env python2.7
import argparse
import subprocess
import sys
import os
from os.path import join as pjoin

def parse_args(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("exe")
    parser.add_argument("test")
    return parser.parse_args(argv)

def main(argv):
    args = parse_args(argv)
    test_file        = pjoin(args.test, "main.x")
    expected_retcode = int(open(pjoin(args.test, "returncode.txt")).read())
    expected_stdout  = open(pjoin(args.test, "stdout.txt")).read()
    expected_stderr  = open(pjoin(args.test, "stderr.txt")).read()

    proc = subprocess.Popen([args.exe, test_file],
                            stderr=subprocess.PIPE,
                            stdout=subprocess.PIPE)
    stdout, stderr = proc.communicate()

    if stdout != expected_stdout:
        return 1

    if stderr != expected_stderr:
        return 1

    if proc.returncode != expected_retcode:
        return 1

    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
