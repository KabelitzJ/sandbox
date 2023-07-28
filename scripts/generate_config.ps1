rm -Path -Force '.\.bdep\' '.\configs\'

bdep init -q

bdep config create -q @gcc-debug '.\configs\gcc\debug' cc config.cxx='g++' config.cxx.coptions='-g -Wall -Wextra'
bdep config create -q @gcc-release '.\configs\gcc\release' cc config.cxx='g++' config.cxx.coptions='-O3 -Wall -Wextra'

bdep init -q --config-add '.\configs\gcc\debug'
