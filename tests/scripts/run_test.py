#!/usr/bin/env python2.7
import argparse
import subprocess
import sys
import os
import logging
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

    returncode = 0
    if stdout != expected_stdout:
        print("Stdout does not match")
        print("== expected ".ljust(80, "="))
        print(expected_stdout)
        print("== got ".ljust(80, "="))
        print(stdout)
        print("=" * 80)
        returncode = 1

    if stderr != expected_stderr:
        print("Stderr does not match")
        print("== expected ".ljust(80, "="))
        print(expected_stderr)
        print("== got ".ljust(80, "="))
        print(stderr)
        print("=" * 80)
        returncode = 1

    if proc.returncode != expected_retcode:
        print("Return code does not match")
        print("== expected ".ljust(80, "="))
        print(expected_retcode)
        print("== got ".ljust(80, "="))
        print(proc.returncode)
        print("=" * 80)
        returncode = 1

    if returncode == 0:
        print("OK!")
    return returncode 

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
