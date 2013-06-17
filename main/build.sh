export NDK_PATH=/cygdrive/d/dev/android-bundle/android-ndk-r8e/ndk-build
echo "NDK_PATH:" $NDK_PATH

rm -rf release
ant clean
$NDK_PATH -C .

ant release

mkdir release

cp -f "bin/classes.jar" "release/libpvrtexture.jar"
cp -rf "libs/armeabi" "release/armeabi"
cp -rf "libs/armeabi-v7a" "release/armeabi-v7a"

chmod 666 -R release