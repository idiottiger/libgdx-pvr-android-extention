#include "Common.h"
#include <android/asset_manager_jni.h>
#include <cctype>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cstdio>
#include <cstdlib>

unsigned char* getAssertFileData(JNIEnv* env, jobject assetManagerObj,
		const char* filename, unsigned long* pSize) {

	LOGI("Assert Read:[%s]", filename);

	unsigned char *buffer = 0;
	*pSize = 0;

	//get asset manager
	AAssetManager* assetManager = AAssetManager_fromJava(env, assetManagerObj);
	if (assetManager == NULL) {
		LOGE("ERROR: AAssetManager_fromJava RETURN NULL");
		return NULL;
	}

	//get file asset
	AAsset* asset = AAssetManager_open(assetManager, filename,
			AASSET_MODE_UNKNOWN);
	if (asset == NULL) {
		LOGE("ERROR: AAssetManager_open filename:%s", filename);
		return NULL;
	}

	off_t length = AAsset_getLength(asset);

	if (length < 0) {
		LOGE("ERROR: AAsset_getLength filename:%s", filename);
		AAsset_close(asset);
		return NULL;
	} else {
		//read file
		buffer = new unsigned char[length + 1];
		*pSize = AAsset_read(asset, buffer, length);
		buffer[length] = 0;

		//check return size
		if (*pSize < 0) {
			LOGE("ERROR: AAsset_read filename:%s,size:%d", filename, *pSize);
			//also delete buffer
			delete[] buffer;
			buffer = 0;

			//close
			AAsset_close(asset);
			return NULL;
		}

		//LOGI("FINISH AAsset_read file:[%s],size:[%d]", filename, *pSize);

		AAsset_close(asset);
	}

	return buffer;
}

unsigned char* getFileData(const char* pszFileName, const char* pszMode,
		unsigned long* pSize) {

	LOGI("Sdcard Read :[%s]", pszFileName);

	unsigned char* pBuffer = NULL;
	*pSize = 0;
	do {
		// read the file from hardware
		FILE *fp = fopen(pszFileName, pszMode);
		if (fp == NULL) {
			LOGE("Error: file name:[%s], cannot access.", pszFileName);
			return NULL;
		}
		fseek(fp, 0, SEEK_END);
		*pSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		pBuffer = new unsigned char[*pSize];
		*pSize = fread(pBuffer, sizeof(unsigned char), *pSize, fp);
		fclose(fp);
	} while (0);

	return pBuffer;
}
