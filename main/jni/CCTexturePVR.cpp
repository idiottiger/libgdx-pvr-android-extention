#include <ctype.h>
#include <cctype>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cstdio>
#include <cstdlib>
#include <string>

#include <string.h>
#include "CCTexturePVR.h"
#include "Common.h"
#include <android/asset_manager_jni.h>

unsigned long ccNextPOT(unsigned long x) {
	x = x - 1;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >> 16);
	return x + 1;
}

//
// XXX DO NO ALTER THE ORDER IN THIS LIST XXX
//
static const ccPVRTexturePixelFormatInfo PVRTableFormats[] = {

// 0: BGRA_8888
// change GL_BGRA - > GL_RGBA
		{ GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, 32, false, true,
				kCCTexture2DPixelFormat_RGBA8888 },
		// 1: RGBA_8888
		{ GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, 32, false, true,
				kCCTexture2DPixelFormat_RGBA8888 },
		// 2: RGBA_4444
		{ GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, 16, false, true,
				kCCTexture2DPixelFormat_RGBA4444 },
		// 3: RGBA_5551
		{ GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, 16, false, true,
				kCCTexture2DPixelFormat_RGB5A1 },
		// 4: RGB_565
		{ GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 16, false, false,
				kCCTexture2DPixelFormat_RGB565 },
		// 5: RGB_888
		{ GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, 24, false, false,
				kCCTexture2DPixelFormat_RGB888 },
		// 6: A_8
		{ GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE, 8, false, false,
				kCCTexture2DPixelFormat_A8 },
		// 7: L_8
		{ GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE, 8, false, false,
				kCCTexture2DPixelFormat_I8 },
		// 8: LA_88
		{ GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 16, false,
				true, kCCTexture2DPixelFormat_AI88 },

		// 9: PVRTC 2BPP RGB
		{ GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG, 0xFFFFFFFF, 0xFFFFFFFF, 2, true,
				false, kCCTexture2DPixelFormat_PVRTC2 },
		// 10: PVRTC 2BPP RGBA
		{ GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG, 0xFFFFFFFF, 0xFFFFFFFF, 2, true,
				true, kCCTexture2DPixelFormat_PVRTC2 },
		// 11: PVRTC 4BPP RGB
		{ GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, 0xFFFFFFFF, 0xFFFFFFFF, 4, true,
				false, kCCTexture2DPixelFormat_PVRTC4 },
		// 12: PVRTC 4BPP RGBA
		{ GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, 0xFFFFFFFF, 0xFFFFFFFF, 4, true,
				true, kCCTexture2DPixelFormat_PVRTC4 }, };

struct _pixelformat_hash {
	uint64_t pixelFormat;
	const ccPVRTexturePixelFormatInfo * pixelFormatInfo;
};

// Values taken from PVRTexture.h from http://www.imgtec.com

enum {
	kPVRTextureFlagMipmap = (1 << 8), // has mip map levels
	kPVRTextureFlagTwiddle = (1 << 9), // is twiddled
	kPVRTextureFlagBumpmap = (1 << 10), // has normals encoded for a bump map
	kPVRTextureFlagTiling = (1 << 11), // is bordered for tiled pvr
	kPVRTextureFlagCubemap = (1 << 12), // is a cubemap/skybox
	kPVRTextureFlagFalseMipCol = (1 << 13), // are there false colored MIP levels
	kPVRTextureFlagVolume = (1 << 14), // is this a volume texture
	kPVRTextureFlagAlpha = (1 << 15), // v2.1 is there transparency info in the texture
	kPVRTextureFlagVerticalFlip = (1 << 16), // v2.1 is the texture vertically flipped
};

static char gPVRTexIdentifier[5] = "PVR!";

// v2

typedef enum {
	kPVR2TexturePixelFormat_RGBA_4444 = 0x10,
	kPVR2TexturePixelFormat_RGBA_5551,
	kPVR2TexturePixelFormat_RGBA_8888,
	kPVR2TexturePixelFormat_RGB_565,
	kPVR2TexturePixelFormat_RGB_555, // unsupported
	kPVR2TexturePixelFormat_RGB_888,
	kPVR2TexturePixelFormat_I_8,
	kPVR2TexturePixelFormat_AI_88,
	kPVR2TexturePixelFormat_PVRTC_2BPP_RGBA,
	kPVR2TexturePixelFormat_PVRTC_4BPP_RGBA,
	kPVR2TexturePixelFormat_BGRA_8888,
	kPVR2TexturePixelFormat_A_8,
} ccPVR2TexturePixelFormat;

// v3
/* supported predefined formats */
#define kPVR3TexturePixelFormat_PVRTC_2BPP_RGB   0
#define kPVR3TexturePixelFormat_PVRTC_2BPP_RGBA  1
#define kPVR3TexturePixelFormat_PVRTC_4BPP_RGB   2
#define kPVR3TexturePixelFormat_PVRTC_4BPP_RGBA  3

/* supported channel type formats */
#define kPVR3TexturePixelFormat_BGRA_8888  0x0808080861726762ULL
#define kPVR3TexturePixelFormat_RGBA_8888  0x0808080861626772ULL
#define kPVR3TexturePixelFormat_RGBA_4444  0x0404040461626772ULL
#define kPVR3TexturePixelFormat_RGBA_5551  0x0105050561626772ULL
#define kPVR3TexturePixelFormat_RGB_565    0x0005060500626772ULL
#define kPVR3TexturePixelFormat_RGB_888    0x0008080800626772ULL
#define kPVR3TexturePixelFormat_A_8        0x0000000800000061ULL
#define kPVR3TexturePixelFormat_L_8        0x000000080000006cULL
#define kPVR3TexturePixelFormat_LA_88      0x000008080000616cULL

// v2
static struct _pixelformat_hash v2_pixelformat_hash[] = {

{ kPVR2TexturePixelFormat_BGRA_8888, &PVRTableFormats[0] }, {
		kPVR2TexturePixelFormat_RGBA_8888, &PVRTableFormats[1] }, {
		kPVR2TexturePixelFormat_RGBA_4444, &PVRTableFormats[2] }, {
		kPVR2TexturePixelFormat_RGBA_5551, &PVRTableFormats[3] }, {
		kPVR2TexturePixelFormat_RGB_565, &PVRTableFormats[4] }, {
		kPVR2TexturePixelFormat_RGB_888, &PVRTableFormats[5] }, {
		kPVR2TexturePixelFormat_A_8, &PVRTableFormats[6] }, {
		kPVR2TexturePixelFormat_I_8, &PVRTableFormats[7] }, {
		kPVR2TexturePixelFormat_AI_88, &PVRTableFormats[8] }, {
		kPVR2TexturePixelFormat_PVRTC_2BPP_RGBA, &PVRTableFormats[10] }, {
		kPVR2TexturePixelFormat_PVRTC_4BPP_RGBA, &PVRTableFormats[12] }, };

#define PVR2_MAX_TABLE_ELEMENTS (sizeof(v2_pixelformat_hash) / sizeof(v2_pixelformat_hash[0]))

// v3
struct _pixelformat_hash v3_pixelformat_hash[] = {

{ kPVR3TexturePixelFormat_BGRA_8888, &PVRTableFormats[0] }, {
		kPVR3TexturePixelFormat_RGBA_8888, &PVRTableFormats[1] }, {
		kPVR3TexturePixelFormat_RGBA_4444, &PVRTableFormats[2] }, {
		kPVR3TexturePixelFormat_RGBA_5551, &PVRTableFormats[3] }, {
		kPVR3TexturePixelFormat_RGB_565, &PVRTableFormats[4] }, {
		kPVR3TexturePixelFormat_RGB_888, &PVRTableFormats[5] }, {
		kPVR3TexturePixelFormat_A_8, &PVRTableFormats[6] }, {
		kPVR3TexturePixelFormat_L_8, &PVRTableFormats[7] }, {
		kPVR3TexturePixelFormat_LA_88, &PVRTableFormats[8] }, {
		kPVR3TexturePixelFormat_PVRTC_2BPP_RGB, &PVRTableFormats[9] }, {
		kPVR3TexturePixelFormat_PVRTC_2BPP_RGBA, &PVRTableFormats[10] }, {
		kPVR3TexturePixelFormat_PVRTC_4BPP_RGB, &PVRTableFormats[11] }, {
		kPVR3TexturePixelFormat_PVRTC_4BPP_RGBA, &PVRTableFormats[12] }, };

//Tells How large is tableFormats
#define PVR3_MAX_TABLE_ELEMENTS (sizeof(v3_pixelformat_hash) / sizeof(v3_pixelformat_hash[0]))

typedef struct _PVRTexHeader {
	unsigned int headerLength;
	unsigned int height;
	unsigned int width;
	unsigned int numMipmaps;
	unsigned int flags;
	unsigned int dataLength;
	unsigned int bpp;
	unsigned int bitmaskRed;
	unsigned int bitmaskGreen;
	unsigned int bitmaskBlue;
	unsigned int bitmaskAlpha;
	unsigned int pvrTag;
	unsigned int numSurfs;
} ccPVRv2TexHeader;

typedef struct {
	uint32_t version;
	uint32_t flags;
	uint64_t pixelFormat;
	uint32_t colorSpace;
	uint32_t channelType;
	uint32_t height;
	uint32_t width;
	uint32_t depth;
	uint32_t numberOfSurfaces;
	uint32_t numberOfFaces;
	uint32_t numberOfMipmaps;
	uint32_t metadataLength;
} ccPVRv3TexHeader;

CCTexturePVR::CCTexturePVR() {
	mPVRTextureData = 0;
}

CCTexturePVR::~CCTexturePVR() {
	CC_SAFE_DELETE_ARRAY(mPVRTextureData);
	LOGI("TexturePVR release ...");
}

bool CCTexturePVR::unpackPVRv2Data(unsigned char* data, unsigned int len) {
	LOGD("START unpackPVRv2Data ...");
	bool success = false;
	ccPVRv2TexHeader *header = NULL;
	unsigned int flags, pvrTag;
	unsigned int dataLength = 0, dataOffset = 0, dataSize = 0;
	unsigned int blockSize = 0, widthBlocks = 0, heightBlocks = 0;
	unsigned int width = 0, height = 0, bpp = 4;
	unsigned char *bytes = NULL;
	unsigned int formatFlags;

	//Cast first sizeof(PVRTexHeader) bytes of data stream as PVRTexHeader
	header = (ccPVRv2TexHeader *) data;

	//Make sure that tag is in correct formatting
	pvrTag = CC_SWAP_INT32_LITTLE_TO_HOST(header->pvrTag);

	if (gPVRTexIdentifier[0] != ((pvrTag >> 0) & 0xff)
			|| gPVRTexIdentifier[1] != ((pvrTag >> 8) & 0xff)
			|| gPVRTexIdentifier[2] != ((pvrTag >> 16) & 0xff)
			|| gPVRTexIdentifier[3] != ((pvrTag >> 24) & 0xff)) {
		return false;
	}

	flags = CC_SWAP_INT32_LITTLE_TO_HOST(header->flags);
	formatFlags = flags & PVR_TEXTURE_FLAG_TYPE_MASK;
	bool flipped = (flags & kPVRTextureFlagVerticalFlip) ? true : false;

	if (flipped) {
		LOGW("WARNING: Image is flipped. Regenerate it using PVRTexTool");
	}

	if (header->width != ccNextPOT(header->width)
			|| header->height != ccNextPOT(header->height)) {
		LOGE(
				"ERROR: Loading an NPOT texture (%dx%d) but is not supported on this device", header->width, header->height);
		return false;
	}

	for (unsigned int i = 0; i < (unsigned int) PVR2_MAX_TABLE_ELEMENTS; i++) {
		//Does image format in table fits to the one parsed from header?
		if (v2_pixelformat_hash[i].pixelFormat == formatFlags) {
			m_pPixelFormatInfo = v2_pixelformat_hash[i].pixelFormatInfo;

			//Reset num of mipmaps
			m_uNumberOfMipmaps = 0;

			//Get size of mipmap
			m_uWidth = width = CC_SWAP_INT32_LITTLE_TO_HOST(header->width);
			m_uHeight = height = CC_SWAP_INT32_LITTLE_TO_HOST(header->height);

			//Do we use alpha ?
			if (CC_SWAP_INT32_LITTLE_TO_HOST(header->bitmaskAlpha)) {
				m_bHasAlpha = true;
			} else {
				m_bHasAlpha = false;
			}

			//Get ptr to where data starts..
			dataLength = CC_SWAP_INT32_LITTLE_TO_HOST(header->dataLength);

			//Move by size of header
			bytes = ((unsigned char *) data) + sizeof(ccPVRv2TexHeader);
			m_eFormat = m_pPixelFormatInfo->ccPixelFormat;
			bpp = m_pPixelFormatInfo->bpp;

			// Calculate the data size for each texture level and respect the minimum number of blocks
			while (dataOffset < dataLength) {
				switch (formatFlags) {
				case kPVR2TexturePixelFormat_PVRTC_2BPP_RGBA:
					blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
					widthBlocks = width / 8;
					heightBlocks = height / 4;
					break;
				case kPVR2TexturePixelFormat_PVRTC_4BPP_RGBA:
					blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
					widthBlocks = width / 4;
					heightBlocks = height / 4;
					break;
				case kPVR2TexturePixelFormat_BGRA_8888:
					LOGE("ERROR: TexturePVR. BGRA8888 not supported");
					return false;
				default:
					blockSize = 1;
					widthBlocks = width;
					heightBlocks = height;
					break;
				}

				// Clamp to minimum number of blocks
				if (widthBlocks < 2) {
					widthBlocks = 2;
				}
				if (heightBlocks < 2) {
					heightBlocks = 2;
				}

				dataSize = widthBlocks * heightBlocks * ((blockSize * bpp) / 8);
				unsigned int packetLength = (dataLength - dataOffset);
				packetLength =
						packetLength > dataSize ? dataSize : packetLength;

				//Make record to the mipmaps array and increment counter
				m_asMipmaps[m_uNumberOfMipmaps].address = bytes + dataOffset;
				m_asMipmaps[m_uNumberOfMipmaps].len = packetLength;
				m_uNumberOfMipmaps++;

				//Check that we didn't overflow

				dataOffset += packetLength;

				//Update width and height to the next lower power of two
				width = MAX(width >> 1, 1);
				height = MAX(height >> 1, 1);
			}

			//Mark pass as success
			success = true;
			break;
		}
	}

	if (!success) {
		LOGW(
				"WARNING: Unsupported PVR Pixel Format: 0x%2x. Re-encode it with a OpenGL pixel format variant", formatFlags);
	}

	return success;
}

bool CCTexturePVR::unpackPVRv3Data(unsigned char* dataPointer,
		unsigned int dataLength) {
	LOGD("START unpackPVRv3Data ...");

	if (dataLength < sizeof(ccPVRv3TexHeader)) {
		return false;
	}

	ccPVRv3TexHeader *header = (ccPVRv3TexHeader *) dataPointer;

	// validate version
	if (CC_SWAP_INT32_BIG_TO_HOST(header->version) != 0x50565203) {
		LOGE("ERROR: pvr file version mismatch");
		return false;
	}

	// parse pixel format
	uint64_t pixelFormat = header->pixelFormat;

	bool infoValid = false;

	for (int i = 0; i < PVR3_MAX_TABLE_ELEMENTS; i++) {
		if (v3_pixelformat_hash[i].pixelFormat == pixelFormat) {
			m_pPixelFormatInfo = v3_pixelformat_hash[i].pixelFormatInfo;
			m_bHasAlpha = m_pPixelFormatInfo->alpha;
			infoValid = true;
			break;
		}
	}

	// unsupported / bad pixel format
	if (!infoValid) {
		LOGE("ERROR: unsupported pvr pixelformat: %llx", pixelFormat);
		return false;
	}

	// sizing
	uint32_t width = CC_SWAP_INT32_LITTLE_TO_HOST(header->width);
	uint32_t height = CC_SWAP_INT32_LITTLE_TO_HOST(header->height);
	m_uWidth = width;
	m_uHeight = height;
	uint32_t dataOffset = 0, dataSize = 0;
	uint32_t blockSize = 0, widthBlocks = 0, heightBlocks = 0;
	uint8_t *bytes = NULL;

	dataOffset = (sizeof(ccPVRv3TexHeader) + header->metadataLength);
	bytes = dataPointer;

	m_uNumberOfMipmaps = header->numberOfMipmaps;
	//CCAssert(m_uNumberOfMipmaps < CC_PVRMIPMAP_MAX, @"TexturePVR: Maximum number of mimpaps reached. Increate the CC_PVRMIPMAP_MAX value");

	for (unsigned int i = 0; i < m_uNumberOfMipmaps; i++) {
		switch (pixelFormat) {
		case kPVR3TexturePixelFormat_PVRTC_2BPP_RGB:
		case kPVR3TexturePixelFormat_PVRTC_2BPP_RGBA:
			blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
			widthBlocks = width / 8;
			heightBlocks = height / 4;
			break;
		case kPVR3TexturePixelFormat_PVRTC_4BPP_RGB:
		case kPVR3TexturePixelFormat_PVRTC_4BPP_RGBA:
			blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
			widthBlocks = width / 4;
			heightBlocks = height / 4;
			break;
		case kPVR3TexturePixelFormat_BGRA_8888:
			LOGE("ERROR: TexturePVR. BGRA8888 not supported on this device");
			return false;
		default:
			blockSize = 1;
			widthBlocks = width;
			heightBlocks = height;
			break;
		}

		// Clamp to minimum number of blocks
		if (widthBlocks < 2) {
			widthBlocks = 2;
		}
		if (heightBlocks < 2) {
			heightBlocks = 2;
		}

		dataSize = widthBlocks * heightBlocks
				* ((blockSize * m_pPixelFormatInfo->bpp) / 8);
		unsigned int packetLength = ((unsigned int) dataLength - dataOffset);
		packetLength = packetLength > dataSize ? dataSize : packetLength;

		m_asMipmaps[i].address = bytes + dataOffset;
		m_asMipmaps[i].len = packetLength;

		dataOffset += packetLength;
		//CCAssert(dataOffset <= dataLength, "CCTexurePVR: Invalid lenght");

		width = MAX(width >> 1, 1);
		height = MAX(height >> 1, 1);
	}

	return true;
}

bool CCTexturePVR::createGLTexture(bool cache) {
	bool result = _createGLTexture();
	if (!cache) {
		CC_SAFE_DELETE_ARRAY(mPVRTextureData);
	}
	return result;
}

bool CCTexturePVR::_createGLTexture() {
	unsigned int width = m_uWidth;
	unsigned int height = m_uHeight;
	GLenum err;

	LOGI("Create GLTexture ...");

	if (m_uNumberOfMipmaps > 0) {

		// From PVR sources: "PVR files are never row aligned."
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		//@ COMMENT IN ANDROID FOR LIBGDX
		//glGenTextures(1, &m_uName);
		//glBindTexture(GL_TEXTURE_2D, m_uName);

		// Default: Anti alias.
		if (m_uNumberOfMipmaps == 1) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					GL_LINEAR_MIPMAP_NEAREST);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	// CHECK_GL_ERROR_DEBUG(); // clean possible GL error

	GLenum internalFormat = m_pPixelFormatInfo->internalFormat;
	GLenum format = m_pPixelFormatInfo->format;
	GLenum type = m_pPixelFormatInfo->type;
	bool compressed = m_pPixelFormatInfo->compressed;

	LOGD(
			"createGLTexture [width:%d,height:%d,m_uNumberOfMipmaps:%d,internalFormat:%d,format:%d,type:%d,compressed:%d]", width, height, m_uNumberOfMipmaps, internalFormat, format, type, compressed);

	// Generate textures with mipmaps
	for (unsigned int i = 0; i < m_uNumberOfMipmaps; ++i) {
		//PVRTC support need check in java

		//        if (compressed && !CCConfiguration::sharedConfiguration()->supportsPVRTC()) {
		//            CCLOG("cocos2d: WARNING: PVRTC images are not supported");
		//            return false;
		//        }

		unsigned char *data = m_asMipmaps[i].address;
		GLsizei datalen = m_asMipmaps[i].len;

		if (compressed) {
			glCompressedTexImage2D(GL_TEXTURE_2D, i, internalFormat, width,
					height, 0, datalen, data);
		} else {
			glTexImage2D(GL_TEXTURE_2D, i, internalFormat, width, height, 0,
					format, type, data);
		}

		if (i > 0 && (width != height || ccNextPOT(width) != width)) {
			LOGW(
					"WARNING. Mipmap level %u is not squared. Texture won't render correctly. width=%u != height=%u", i, width, height);
		}

		err = glGetError();
		if (err != GL_NO_ERROR) {
			// CCLOG("cocos2d: TexturePVR: Error uploading compressed texture level: %u . glError: 0x%04X", i, err);

			LOGE(
					"ERROR: compressed texture level: %u, glError code:%d", i, err);
			return false;
		}

		width = MAX(width >> 1, 1);
		height = MAX(height >> 1, 1);
	}

	return true;
}

bool CCTexturePVR::iniTextTureData(unsigned char* data, int pvrlen) {
	//unsigned char* pvrdata = NULL;

	LOGI("iniTextTureData ...");

	CC_SAFE_DELETE_ARRAY(mPVRTextureData);

	mPVRTextureData = data;

	if (mPVRTextureData == NULL) {
		return false;
	}

	m_uNumberOfMipmaps = 0;

	m_uName = 0;
	m_uWidth = m_uHeight = 0;
	m_pPixelFormatInfo = NULL;
	m_bHasAlpha = false;

	m_bRetainName = false; // cocos2d integration

	bool result = unpackPVRv2Data(mPVRTextureData, pvrlen);

	LOGD("Result  unpackPVRv2Data:%d", result);

	bool result2 = result || unpackPVRv3Data(mPVRTextureData, pvrlen);

	if (!result) {
		LOGD("Result  unpackPVRv3Data:%d", result2);
	}

	//if return false mean can't pack
	//but data will delete
	if (!result2) {
		CC_SAFE_DELETE_ARRAY(mPVRTextureData);
	}

	return result2;
}

