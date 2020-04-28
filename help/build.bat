

echo Generating help files..
qcollectiongenerator programmer.qhcp -o programmer.qhc
echo Copying to installer
copy programmer.qhc ..\installer
