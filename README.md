libgdx-pvr-android-extention
============================

### This is a library base on the libgdx, and make the libgdx android support the pvr texture, it's not the soft decode, it also need the device can support pvr. ###




folder structure
----------------------------
main  folder is the library folder.

example folder is the library example useage.


how to build & use
------------------------------
set the NDK_PATH in main/build.sh build script, and run it, it will create the so file

because the libgdx-pvr-android-extention need use the gdx.jar file, the project also have one copy of gdx.jar in libs folder, so cann't directed reference this project, after so file created, need export the java code to jar file.

and finally copy the libs folder and export jar file to your project's libs folder


about the code
-----------------------------
there has only class: __PVRTextureData.java__ which extends the __com.badlogic.gdx.graphics.TextureData__, you can pass the PVRTextureData to Texture, and use Texture to draw it, how to use the Texture, please check: http://libgdx.l33tlabs.org/docs/api/com/badlogic/gdx/graphics/Texture.html.

if you check the native code, you will find has __CCTexturePVR.cpp__ and __CCTexturePVR.h__ these two files, yes, this code came from the __cocos2dx__ project, base on that, make the code suitable for libgdx.
