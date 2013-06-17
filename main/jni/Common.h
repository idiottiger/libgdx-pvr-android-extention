/* 
 * File:   Common.h
 * Author: xiaocong.chen
 *
 * Created on February 28, 2013, 10:39 AM
 */

#ifndef COMMON_H
#define	COMMON_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <android/log.h>   
#include <jni.h>


#define ENABLE_DETAIL_LOG
#define TAG "LIBGDX_PVRTEXTURE"    
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

#ifdef ENABLE_DETAIL_LOG
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#else
#define LOGD(...) (void)0
#endif
#define PVR_TEXTURE_FLAG_TYPE_MASK    0xff

#define CC_HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100) 
#define CC_SWAP32(i)  ((i & 0x000000ff) << 24 | (i & 0x0000ff00) << 8 | (i & 0x00ff0000) >> 8 | (i & 0xff000000) >> 24)
#define CC_SWAP16(i)  ((i & 0x00ff) << 8 | (i &0xff00) >> 8)   
#define CC_SWAP_INT32_LITTLE_TO_HOST(i) ((CC_HOST_IS_BIG_ENDIAN == true)? CC_SWAP32(i) : (i) )
#define CC_SWAP_INT16_LITTLE_TO_HOST(i) ((CC_HOST_IS_BIG_ENDIAN == true)? CC_SWAP16(i) : (i) )
#define CC_SWAP_INT32_BIG_TO_HOST(i)    ((CC_HOST_IS_BIG_ENDIAN == true)? (i) : CC_SWAP32(i) )
#define CC_SWAP_INT16_BIG_TO_HOST(i)    ((CC_HOST_IS_BIG_ENDIAN == true)? (i):  CC_SWAP16(i) )

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define MAX max

#define CC_SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = 0; } } while(0)    

unsigned char* getAssertFileData(JNIEnv* env, jobject assetManager,
		const char* filename, unsigned long* pSize);
unsigned char* getFileData(const char* pszFileName, const char* pszMode,
		unsigned long* pSize);
#ifdef	__cplusplus
}
#endif

#endif	/* COMMON_H */

