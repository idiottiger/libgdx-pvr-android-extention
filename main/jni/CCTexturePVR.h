
#ifndef __CCPVRTEXTURE_H__
#define __CCPVRTEXTURE_H__

#include <GLES/gl.h>

/**
 @brief Structure which can tell where mipmap begins and how long is it
 */
struct CCPVRMipmap {
    unsigned char *address;
    unsigned int len;
};

typedef enum {
    //! 32-bit texture: RGBA8888
    kCCTexture2DPixelFormat_RGBA8888,
    //! 24-bit texture: RGBA888
    kCCTexture2DPixelFormat_RGB888,
    //! 16-bit texture without Alpha channel
    kCCTexture2DPixelFormat_RGB565,
    //! 8-bit textures used as masks
    kCCTexture2DPixelFormat_A8,
    //! 8-bit intensity texture
    kCCTexture2DPixelFormat_I8,
    //! 16-bit textures used as masks
    kCCTexture2DPixelFormat_AI88,
    //! 16-bit textures: RGBA4444
    kCCTexture2DPixelFormat_RGBA4444,
    //! 16-bit textures: RGB5A1
    kCCTexture2DPixelFormat_RGB5A1,
    //! 4-bit PVRTC-compressed texture: PVRTC4
    kCCTexture2DPixelFormat_PVRTC4,
    //! 2-bit PVRTC-compressed texture: PVRTC2
    kCCTexture2DPixelFormat_PVRTC2,

    //! Default texture format: RGBA8888
    kCCTexture2DPixelFormat_Default = kCCTexture2DPixelFormat_RGBA8888,

    // backward compatibility stuff
    kTexture2DPixelFormat_RGBA8888 = kCCTexture2DPixelFormat_RGBA8888,
    kTexture2DPixelFormat_RGB888 = kCCTexture2DPixelFormat_RGB888,
    kTexture2DPixelFormat_RGB565 = kCCTexture2DPixelFormat_RGB565,
    kTexture2DPixelFormat_A8 = kCCTexture2DPixelFormat_A8,
    kTexture2DPixelFormat_RGBA4444 = kCCTexture2DPixelFormat_RGBA4444,
    kTexture2DPixelFormat_RGB5A1 = kCCTexture2DPixelFormat_RGB5A1,
    kTexture2DPixelFormat_Default = kCCTexture2DPixelFormat_Default

} CCTexture2DPixelFormat;

typedef struct _ccPVRTexturePixelFormatInfo {
    GLenum internalFormat;
    GLenum format;
    GLenum type;
    uint32_t bpp;
    bool compressed;
    bool alpha;
    CCTexture2DPixelFormat ccPixelFormat;
} ccPVRTexturePixelFormatInfo;

/**
 @brief Determine how many mipmaps can we have. 
 Its same as define but it respects namespaces
 */
enum {
    CC_PVRMIPMAP_MAX = 16,
};

/** CCTexturePVR
     
 Object that loads PVR images.

 Supported PVR formats:
    - RGBA8888
    - BGRA8888
    - RGBA4444
    - RGBA5551
    - RGB565
    - A8
    - I8
    - AI88
    - PVRTC 4BPP
    - PVRTC 2BPP
     
 Limitations:
    Pre-generated mipmaps, such as PVR textures with mipmap levels embedded in file,
    are only supported if all individual sprites are of _square_ size. 
    To use mipmaps with non-square textures, instead call CCTexture2D#generateMipmap on the sheet texture itself
    (and to save space, save the PVR sprite sheet without mip maps included).
 */
class CCTexturePVR {
public:
    CCTexturePVR();
    virtual ~CCTexturePVR();

    bool iniTextTureData(unsigned char* data, int pvrlen);

    // properties 

    inline unsigned int getName() {
        return m_uName;
    }

    inline unsigned int getWidth() {
        return m_uWidth;
    }

    inline unsigned int getHeight() {
        return m_uHeight;
    }

    inline bool hasAlpha() {
        return m_bHasAlpha;
    }

    inline unsigned int getNumberOfMipmaps() {
        return m_uNumberOfMipmaps;
    }

    inline CCTexture2DPixelFormat getFormat() {
        return m_eFormat;
    }

    inline bool isRetainName() {
        return m_bRetainName;
    }

    inline void setRetainName(bool retainName) {
        m_bRetainName = retainName;
    }

    bool unpackPVRv2Data(unsigned char* data, unsigned int len);
    bool unpackPVRv3Data(unsigned char* dataPointer, unsigned int dataLength);
    bool createGLTexture(bool cache);


protected:
    struct CCPVRMipmap m_asMipmaps[CC_PVRMIPMAP_MAX]; // pointer to mipmap images    
    unsigned int m_uNumberOfMipmaps; // number of mipmap used

    unsigned int m_uWidth, m_uHeight;
    GLuint m_uName;
    bool m_bHasAlpha;

    // cocos2d integration
    bool m_bRetainName;
    CCTexture2DPixelFormat m_eFormat;

    const ccPVRTexturePixelFormatInfo *m_pPixelFormatInfo;

    // the unpack and create gl invoke in different method, so here add instance to save data
    // and release
    unsigned char* mPVRTextureData;

private:
    bool _createGLTexture();

};

// end of textures group
/// @}


#endif //__CCPVRTEXTURE_H__
