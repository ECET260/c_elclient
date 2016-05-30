set -x
set -e
cd ~
if [ ! -e  gcc-arm-none-eabi-5_3-2016q1 ]; then
    wget https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q1-update/+download/gcc-arm-none-eabi-5_3-2016q1-20160330-linux.tar.bz2
    tar xf gcc-arm-none-eabi-5_3-2016q1-20160330-linux.tar.bz2
fi


