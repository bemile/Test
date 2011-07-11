HOST=`hostname -f`

cd ~/$HOST/src/Test/EmulabStarter
make
make clean
sudo ./emustarter config

