libgdx-pvr-android-extention
============================

### This is a library base on the libgdx, and make the libgdx android support the pvr texture, it's not the soft decode, it also need the device can support pvr. ###




folder structure
----------------------------
__main__  folder is the library folder.

__example__ folder is the library example useage.


how to build & use
------------------------------
set the __NDK_PATH__ in main/build.sh build script, and run it, it will export the jar file and SO file to __release__ folder, you need copy all files under __release__ folder to your project's __libs__ folder.


about the code
-----------------------------
there has only class: __PVRTextureData.java__ which extends the __com.badlogic.gdx.graphics.TextureData__, you can pass the PVRTextureData to Texture, and use Texture to draw it, how to use the Texture, please check: http://libgdx.l33tlabs.org/docs/api/com/badlogic/gdx/graphics/Texture.html.

if you check the native code, you will find has __CCTexturePVR.cpp__ and __CCTexturePVR.h__ these two files, yes, this code came from the __cocos2dx__ project, base on that, make the code suitable for libgdx.
