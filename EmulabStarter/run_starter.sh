HOST=`hostname -f`

cd ~/$HOST/Test/EmulabStarter
make
make clean
cp ./emustarter ~/$HOST/
cp ./config ~/$HOST/
cd ~/$HOST
sleep 1
sudo rm -rf ~/$HOST/Test
sudo ./emustarter config

