#!/usr/bin/env python
import os
from sys import argv

def main():
	if(len(argv) == 2):
		test_dir = argv[1]
		for d in ["success", "fail"]:
			out = open(os.path.join(test_dir, d + ".list"), "w")
			for root, dirs, files in os.walk(os.path.join(test_dir, d)):
				for i in files:
					if i.endswith(".c"):
						out.write(os.path.join(root, i) + "\n")

if __name__ == "__main__":
	main()
