#include <math.h>
#include <map>

#include "PVRTextureData.h"
#include "CCTexturePVR.h"
#include "Common.h"

static std::map<int, TexturePVRItem*> sTexturePVRMaps;
static bool sCacheEnable = false;

/**
 * get item from map
 */
TexturePVRItem* getItemFromMap(int objectId) {
	TexturePVRItem *item = 0;

	std::map<int, TexturePVRItem*>::iterator it;
	it = sTexturePVRMaps.find(objectId);
	if (it != sTexturePVRMaps.end()) {
		//find
		item = it->second;
	}

	return item;
}

JNIEXPORT void JNICALL Java_pkg_prvextention_PVRTextureData_native_1setTextureCacheEnable(
		JNIEnv *env, jclass cls, jboolean cacheEnable) {
	sCacheEnable = cacheEnable;
	LOGI("all pvr texture cache enable:%d", sCacheEnable);
}

JNIEXPORT void JNICALL Java_pkg_prvextention_PVRTextureData_native_1disposeAllTextureCache(
		JNIEnv *env, jclass cls) {

	LOGI("dispose all texture cache");

	std::map<int, TexturePVRItem*>::iterator it = sTexturePVRMaps.begin();
	std::map<int, TexturePVRItem*>::iterator end = sTexturePVRMaps.end();

	while (it != end) {
		delete (it->second);
		it++;
	}

	sTexturePVRMaps.clear();
}

/*
 * Class:     pkg_prvextention_PVRTextureData
 * Method:    native_preparePVRTexture
 * Signature: (Landroid/content/res/AssetManager;Ljava/lang/String;I)V
 */JNIEXPORT void JNICALL Java_pkg_prvextention_PVRTextureData_native_1preparePVRTexture(
		JNIEnv *env, jobject obj, jint objectId, jobject assetManager,
		jstring path, jint type) {

	TexturePVRItem *item = getItemFromMap(objectId);
	if (!item) {

		item = new TexturePVRItem();

		CCTexturePVR* texture = new CCTexturePVR();
		const char* cpath = env->GetStringUTFChars(path, NULL);
		int pvrLen = 0;
		unsigned char* data = 0;

		//type check
		if (type == pkg_prvextention_PVRTextureData_TYPE_RESOURCE_ASSETS) {
			data = getAssertFileData(env, assetManager, cpath,
					(unsigned long *) (&pvrLen));
		} else if (type
				== pkg_prvextention_PVRTextureData_TYPE_RESOURCE_EXTERNAL) {
			data = getFileData(cpath, "rb", (unsigned long *) (&pvrLen));
		}

		if (data) {
			item->result = (texture->iniTextTureData(data, pvrLen));
		} else {
			item->result = false;
		}

		if (item->result) {
			item->pvrTexture = texture;
			sTexturePVRMaps.insert(
					std::pair<int, TexturePVRItem*>(objectId, item));
		} else {
			delete texture;
			delete item;
		}

		//release string
		env->ReleaseStringUTFChars(path, cpath);
	}
}

/*
 * Class:     pkg_prvextention_PVRTextureData
 * Method:    native_consumeCompressedData
 * Signature: ()V
 */JNIEXPORT void JNICALL Java_pkg_prvextention_PVRTextureData_native_1consumeCompressedData(
		JNIEnv *env, jobject obj, jint objectId) {
	TexturePVRItem *item = getItemFromMap(objectId);
	if (item && item->result) {
		item->pvrTexture->createGLTexture(sCacheEnable);

		//delete item and remove from map
		if (!sCacheEnable) {
			delete item;
			sTexturePVRMaps.erase(objectId);
		}
	}
}

/*
 * Class:     pkg_prvextention_PVRTextureData
 * Method:    native_getWidth
 * Signature: ()I
 */JNIEXPORT jint JNICALL Java_pkg_prvextention_PVRTextureData_native_1getWidth(
		JNIEnv *env, jobject obj, jint objectId) {
	TexturePVRItem *item = getItemFromMap(objectId);
	if (item && item->result) {
		return item->pvrTexture->getWidth();
	}
	return -1;
}

/*
 * Class:     pkg_prvextention_PVRTextureData
 * Method:    native_getHeight
 * Signature: ()I
 */JNIEXPORT jint JNICALL Java_pkg_prvextention_PVRTextureData_native_1getHeight(
		JNIEnv *env, jobject obj, jint objectId) {
	TexturePVRItem *item = getItemFromMap(objectId);
	if (item && item->result) {
		return item->pvrTexture->getHeight();
	}
	return -1;
}

/*
 * Class:     pkg_prvextention_PVRTextureData
 * Method:    native_useMipMaps
 * Signature: ()Z
 */JNIEXPORT jboolean JNICALL Java_pkg_prvextention_PVRTextureData_native_1useMipMaps(
		JNIEnv *env, jobject obj, jint objectId) {
	TexturePVRItem *item = getItemFromMap(objectId);
	if (item && item->result) {
		return (item->pvrTexture->getNumberOfMipmaps()) > 1;
	}
	return false;
}

/*
 * Class:     pkg_prvextention_PVRTextureData
 * Method:    native_dispose
 * Signature: (I)V
 */JNIEXPORT void JNICALL Java_pkg_prvextention_PVRTextureData_native_1dispose(
		JNIEnv *env, jobject obj, jint objectId) {
	if (sCacheEnable) {
		TexturePVRItem *item = getItemFromMap(objectId);
		if (item) {
			delete item;
			//delete from map
			sTexturePVRMaps.erase(objectId);
		}
	}
}
