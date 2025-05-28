# RED SDK Documentation

This directory contains the source files for the RED SDK documentation. The documentation is built using Sphinx with Doxygen integration through Breathe.

## Prerequisites

1. Python 3.x with pip
2. Doxygen (for API documentation)
3. Required Python packages:
   ```bash
   pip install sphinx sphinx-rtd-theme breathe sphinx-design
   ```

## Directory Structure

```
docs/
├── Doxyfile           # Doxygen configuration
├── Makefile           # Make targets for building docs
├── README.md          # This file
├── requirements.txt   # Python package requirements
└── source/           # Documentation source files
    ├── api/          # API reference documentation
    ├── guides/       # User guides and tutorials
    └── conf.py       # Sphinx configuration
```

## Building the Documentation

### Setting up Python Virtual Environment

1. **Create and Activate Virtual Environment**
   ```bash
   # Create a new virtual environment in the docs directory
   python3 -m venv .venv

   # On macOS/Linux:
   source .venv/bin/activate

   # On Windows:
   # .venv\Scripts\activate

   # Your prompt should now show (.venv)
   ```

2. **Install Required Packages**
   ```bash
   # Ensure pip is up to date
   python3 -m pip install --upgrade pip

   # Install required packages
   python3 -m pip install -r requirements.txt
   ```

3. **Verify Installation**
   ```bash
   # Verify sphinx is installed
   sphinx-build --version

   # Verify other tools
   python3 -m pip list | grep -E "sphinx|breathe"
   ```

### Building Documentation

1. **Clean and Build**
   ```bash
   # Ensure you're in the virtual environment (.venv)
   # Your prompt should show (.venv)

   # Clean previous builds
   make clean

   # Generate Doxygen XML
   make doxygen

   # Build HTML documentation
   make html
   ```

2. **View Documentation**
   ```bash
   # Start local server (ensure you're in docs directory)
   python3 -m http.server 8000 --directory _build/html
   ```
   Then open http://localhost:8000 in your web browser.

### Troubleshooting Virtual Environment

1. **Port Already in Use**
   ```bash
   # If port 8000 is in use, try a different port
   python3 -m http.server 8080 --directory _build/html
   ```

2. **Virtual Environment Issues**
   ```bash
   # Deactivate and recreate if needed
   deactivate
   rm -rf .venv
   python3 -m venv .venv
   source .venv/bin/activate
   ```

3. **Missing _build Directory**
   ```bash
   # Ensure you're in the docs directory
   pwd  # Should show .../red/sdk/docs
   
   # Rebuild if _build is missing
   make clean
   make html
   ```

## Make Targets

- `make clean`: Remove all built documentation
- `make doxygen`: Generate Doxygen XML files
- `make html`: Build HTML documentation
- `make latexpdf`: Build PDF documentation (requires LaTeX)
- `make help`: Show all available make targets

## Adding Documentation

1. **API Documentation**
   - Add new API documentation files in `source/api/`
   - Update `source/api/index.rst` to include new files

2. **Guides and Tutorials**
   - Add new guide files in `source/guides/`
   - Update `source/index.rst` to include new guides

3. **Configuration**
   - Sphinx configuration is in `source/conf.py`
   - Doxygen configuration is in `Doxyfile`

## Common Issues

1. **Missing XML Files**
   - Ensure Doxygen has been run: `make doxygen`
   - Check Doxygen output for errors
   - Verify XML files are in `_build/xml/`

2. **Build Errors**
   - Check Python package versions in `requirements.txt`
   - Ensure all required packages are installed
   - Look for syntax errors in RST files

3. **Viewing Issues**
   - Clear browser cache if changes aren't visible
   - Ensure `make clean` is run before rebuilding
   - Check file permissions in `_build` directory

## Contributing

1. Follow the existing documentation style and structure
2. Use RST format for documentation files
3. Test builds locally before committing
4. Update table of contents when adding new pages

## Maintenance

- Keep requirements.txt up to date
- Regularly test PDF generation
- Check for broken links
- Update version numbers in conf.py 