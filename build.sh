#/bin/bash
# doing this so that we can campile OS using C lang.
# give this file +x permission using - 'sudo chmod +x ./build.sh' 

export PREFIX="$HOME/opt/cross"
export TARGET=1686-elf
export PATH="$PREFIX/bin:$PATH"
make all