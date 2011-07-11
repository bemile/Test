HOST=`hostname -f`

cd ~/$HOST/Test/EmulabStarter
make
make clean
sudo ./emustarter config

