import subprocess
import os
import glob
import sys


def configureDoxyfile(input_files, output_dir):
    with open('Doxyfile.in', 'r') as file:
        filedata = file.read()

    filedata = filedata.replace('@DOXYGEN_INPUT_FILES@', input_files)
    filedata = filedata.replace('@DOXYGEN_OUTPUT_DIR@', output_dir)

    with open('Doxyfile', 'w') as file:
        file.write(filedata)


def listFiles():
    input_dir = '../src/fort/hermes'
    files = glob.glob(input_dir + '/*.hpp') + glob.glob(input_dir + '*.h')
    return ' '.join(str(x) for x in files)


# Check if we're running on Read the Docs' servers
read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'
ci_build = os.environ.get('CI', None) is not None

breathe_projects = {}

if read_the_docs_build:
    output_dir = 'build'
    configureDoxyfile(listFiles(), output_dir)
    subprocess.call('doxygen', shell=True)
    breathe_projects['fort-hermes'] = output_dir + '/xml'

# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'fort-hermes'
copyright = '2021, Alexandre Tuleu'
author = 'Alexandre Tuleu'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "breathe",
    "sphinx.ext.autodoc",
    "sphinx.ext.napoleon",
]

rootpath = os.path.abspath(os.path.dirname(os.path.dirname(__file__)))
sys.path.append(os.path.join(rootpath, 'src/python'))

breathe_default_project = "fort-hermes"

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

html_context = {
    'display_github': True,
    'github_repo': 'formicidae-tracker/hermes',
    'github_version': 'master',
    'conf_py_path': '/docs/',
}

ci_build = True

if ci_build:
    html_extra_path = ['version_dropdown.js']
    html_context['version'] = 'placeholder'
    html_context['theme_display_version'] = True
    html_js_files = ['../version_dropdown.js']
