curl -O http://kylemcdonald.net/sharingfaces/data/filelist.txt.gz
gunzip filelist.txt.gz
mkdir data
cd data
gshuf ../filelist.txt | \
head -1024 | \
cut -d'.' -f2-7 | \
perl -pe 's!(.+)!-O\nhttp://kylemcdonald.net/sharingfaces/data/images$1.jpg\n-O\nhttp://kylemcdonald.net/sharingfaces/data/metadata$1.face!g' | \
xargs curl
mkdir -p images/initial/
mkdir -p metadata/initial/
mv *.jpg images/initial/
mv *.face metadata/initial/