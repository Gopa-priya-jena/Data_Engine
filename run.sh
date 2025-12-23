#!/bin/bash
reset

# Regular colors
BLACK='\033[0;30m'
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[0;37m'

# Bold colors
BOLD_RED='\033[1;31m'
BOLD_GREEN='\033[1;32m'
BOLD_YELLOW='\033[1;33m'

# Background colors
BG_RED='\033[41m'
BG_GREEN='\033[42m'
NC='\033[0m'

cores=$(awk '/cores/{print $ 4;exit}' /proc/cpuinfo)
compiler_arg="$compiler_arg $san "

if cd build; then
  echo " entered build"
else
  mkdir build
  cd build
  echo " entered build"
fi

cmd="cmake .. "

echo -e "${BLUE} $cmd ${NC} "
if eval "$cmd"; then
  clear
  echo -e "CMAKE SUCESSFULL \n" | lolcat
  cp compile_commands.json ..
  if cmake --build . -j "${cores}"; then
    clear
    echo -e "BUILD SUCESSFULL \n" | lolcat
  else
    echo -e "${BOLD_RED}BUILD FAILED\n${NC}"
  fi
else
  echo -e "${BOLD_RED}CMAKE FAILED\n${NC}"
fi

./Test/main
