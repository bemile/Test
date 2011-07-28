HOST=`hostname -f`

cd ~/$HOST/src/EmulabStarter
make
make clean
cp ./emustarter ~/$HOST/
cp ./config ~/$HOST/
cd ~/$HOST
sudo ./emustarter config

