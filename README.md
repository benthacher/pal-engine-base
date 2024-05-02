# Palygon SDL2 Sim

This project is an example usage of the [PAL engine](https://github.com/benthacher/pal-engine) built on linux using SDL2 as a backend. Currently it's being used to flesh out the PAL engine while hardware design is finalized.

## Running

### Prerequisites

- cmake
    - Installing cmake can be annoying, I'd just follow [their instructions from their download page](https://cmake.org/download/)
- SDL2
    - Depending on your platform, SDL2 dev libraries are probably included in your package manager, but if not, follow [these instructions from their website](https://wiki.libsdl.org/SDL2/Installation)
- Python and venv
    - Python is used for converting sprites and midi files to code usable by the engine. `venv` is used for python environment management, and is required to run the utils in the `util` directory. To install the requirements and make the virtual environment, just run the `setup.sh` script.