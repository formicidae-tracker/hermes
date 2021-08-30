import setuptools
import setuptools.command.build_py
import os
import subprocess
import sys

from distutils.spawn import find_executable
from version import get_git_version

with open("../../README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()



class BuildProtoCommand(setuptools.command.build_py.build_py):
    """A Command that generate required protobuf messages"""

    if 'PROTOC' in os.environ and os.path.exists(os.environ['PROTOC']):
        protoc = os.environ['PROTOC']
    else:
        protoc = find_executable("protoc")

    def generate_proto(source):
        """Invokes the Protocol Compiler to generate a _pb2.py from the given
        .proto file.  Does nothing if the output already exists and is newer than
        the input."""

        output = source.replace(".proto", "_pb2.py").replace("../src/", "")

        if (not os.path.exists(output) or
            (os.path.exists(source) and
             os.path.getmtime(source) > os.path.getmtime(output))):
            print("Generating %s..." % output)

        if not os.path.exists(source):
            sys.stderr.write("Can't find required file: %s\n" % source)
            sys.exit(-1)

        if BuildProtoCommand.protoc is None:
            sys.stderr.write(
                "protoc is not found. Please install the binary package.\n")
            sys.exit(-1)

        protoc_command = [ BuildProtoCommand.protoc, "-I../../protobuf", "--python_out=src/py_fort_hermes/", source ]
        if subprocess.call(protoc_command) != 0:
            sys.exit(-1)

    def run(self):
        BuildProtoCommand.generate_proto('../../protobuf/Tag.proto')
        BuildProtoCommand.generate_proto('../../protobuf/FrameReadout.proto')
        BuildProtoCommand.generate_proto('../../protobuf/Header.proto')
        setuptools.command.build_py.build_py.run(self)



setuptools.setup(
    name="py_fort_myrmidon",
    version=get_git_version(),
    author="Alexandre Tuleu",
    author_email="alexandre.tuleu.2005@polytechnique.org",
    description="FORmicidae Tracker hermes python implementation",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/formicidae-tracker/hermes",
    project_urls={
        "Bug Tracker": "https://github.com/formicidae-tracker/hermes/issues",
    },
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GNU Lesser General Public License v3 (LGPLv3)",
        "Operating System :: OS Independent",
    ],
    package_dir={"": "src"},
    packages=setuptools.find_packages(where="src"),
    python_requires=">=3.6",
    cmdclass={
        'build_py': BuildProtoCommand,
    },
)
