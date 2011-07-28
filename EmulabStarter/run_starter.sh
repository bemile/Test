HOST=`hostname -f`

make
make clean
cp ./emustarter ~/$HOST/
cp ./config ~/$HOST/
cd ~/$HOST
sudo ./emustarter config

