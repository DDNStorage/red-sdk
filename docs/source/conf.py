"""
                               --- WARNING ---

    This work contains trade secrets of DataDirect Networks, Inc.  Any
    unauthorized use or disclosure of the work, or any part thereof, is
    strictly prohibited. Any use of this work without an express license
    or permission is in violation of applicable laws.

Copyright DataDirect Networks, Inc. CONFIDENTIAL AND PROPRIETARY
Copyright DataDirect Networks Copyright, Inc. (c) 2021-2025. All rights reserved.

Name:       conf.py
Project:    RED

Description: Configuration file for the Sphinx documentation builder.
            Contains all settings for building the RED SDK documentation.
"""

import os
import sys

# pylint: disable=invalid-name,redefined-builtin
# Sphinx requires these variable names to be lowercase
# 'copyright' is a required Sphinx configuration variable

sys.path.insert(0, os.path.abspath('../..'))

project = 'RED SDK'
_copyright_text = '2024, DataDirect Networks, Inc.'
author = 'RED Team'
version = '1.0'
release = '1.0.0'

# Extensions
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.napoleon',
    'sphinx.ext.viewcode',
    'sphinx.ext.intersphinx',
    'breathe',
    'sphinx_rtd_theme',
    'sphinx_copybutton',
    'sphinx_design',
    'myst_parser',
    'sphinx.ext.autodoc.typehints',
]

# Breathe configuration
breathe_projects = {
    "RED SDK": "_build/xml"
}
breathe_default_project = "RED SDK"
breathe_default_members = ('members', 'undoc-members')

# HTML theme settings
html_theme = 'sphinx_rtd_theme'
html_theme_options = {
    'logo_only': False,
    'prev_next_buttons_location': 'bottom',
    'style_external_links': False,
    'style_nav_header_background': '#2980B9',
    'collapse_navigation': True,
    'sticky_navigation': True,
    'navigation_depth': 4,
    'includehidden': True,
    'titles_only': False
}

# Other settings
templates_path = ['_templates']
exclude_patterns = []
html_static_path = ['_static']
source_suffix = {
    '.rst': 'restructuredtext',
    '.md': 'markdown',
}

# Napoleon settings
napoleon_google_docstring = True
napoleon_numpy_docstring = True
napoleon_include_init_with_doc = True
napoleon_include_private_with_doc = True
napoleon_include_special_with_doc = True
napoleon_use_admonition_for_examples = True
napoleon_use_admonition_for_notes = True
napoleon_use_admonition_for_references = True
napoleon_use_ivar = True
napoleon_use_param = True
napoleon_use_rtype = True
napoleon_type_aliases = None

# HTML output settings
html_show_copyright = True
html_show_sphinx = False
copyright = _copyright_text  # Required by Sphinx, using our custom text
