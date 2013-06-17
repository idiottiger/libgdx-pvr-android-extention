rm -rf release
ant clean
export NDK_PATH=/cygdrive/d/dev/android-bundle/android-ndk-r8e/ndk-build
$NDK_PATH -C .

ant release

mkdir release

cp -f "bin/classes.jar" "release/libpvrtexture.jar"
cp -rf "libs/armeabi" "release/armeabi"
cp -rf "libs/armeabi-v7a" "release/armeabi-v7a"

chmod 666 -R release