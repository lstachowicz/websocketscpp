#!/usr/bin/env python
import sys
import os
import shutil
import argparse

root_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir))
build_dir = os.path.join(root_dir, "BUILD")

def configure():
	if os.path.exists(build_dir):
		shutil.rmtree(build_dir)

	os.mkdir(build_dir)
	os.chdir(build_dir)
	os.system('cmake ../make -GXcode')

def build():
	os.system('cmake --build .')
	os.system('cmake --build . --target BUILD_ALL_TESTS')

def tests():
	pass

def coverage():
	# TODO: Create coverage report out of artifacts
	pass

def sdk():
	# TODO: Build sdk out of artifacts
	pass

def main(argv):
	parser = argparse.ArgumentParser('Build websocketscpp project')
	parser.add_argument('--coverage', dest='coverage', action='store_true', help='Generate coverage')
	parser.add_argument('--tests', dest='tests', action='store_true', help='Run tests')
	parser.add_argument('--sdk', dest='sdk', action='store_true', help='Generate sdk')
	args = parser.parse_args(argv[:1])

	configure()
	build()

	if args.tests:
		tests()

	if args.coverage:
		coverage()

	if args.sdk:
		sdk()

if __name__ == "__main__":
	main(sys.argv[1:])