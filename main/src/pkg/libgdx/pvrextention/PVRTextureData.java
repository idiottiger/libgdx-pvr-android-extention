package pkg.libgdx.pvrextention;

import android.content.res.AssetManager;

import com.badlogic.gdx.graphics.Pixmap;
import com.badlogic.gdx.graphics.Pixmap.Format;
import com.badlogic.gdx.graphics.TextureData;
import com.badlogic.gdx.utils.GdxRuntimeException;

/**
 * PVR texture data <br>
 * This class extend the {@link com.badlogic.gdx.graphics.TextureData}, you can
 * pass the texture data to the {@link com.badlogic.gdx.graphics.Texture} to
 * draw the pvr, please check the online javadoc for Texture use.
 * 
 * @author idiottiger
 * 
 */
public class PVRTextureData implements TextureData {

    public static boolean ENABLE_LOG = false;

    private static final int TYPE_RESOURCE_ASSETS = 1;
    private static final int TYPE_RESOURCE_EXTERNAL = 2;

    private static boolean sCacheEnable;

    /**
     * set the texture data cache enable.
     * <p>
     * <b> if enabled, will cache the pvr data in the native and load quick in
     * next time, when dont need the cache, need invoke
     * {@link #disposeAllTextureCache()} or invoke {@link #dispose()} method to
     * free the native cache.</b>
     * 
     * @param cacheEnable
     */
    public static void setTextureCacheEnable(boolean cacheEnable) {
        sCacheEnable = cacheEnable;
        native_setTextureCacheEnable(cacheEnable);
    }

    /**
     * return current pvr texture cache enabled
     * 
     * @return
     */
    public static boolean isTextureCacheEnabled() {
        return sCacheEnable;
    }

    /**
     * dispose all texture cache
     */
    public static void disposeAllTextureCache() {
        native_disposeAllTextureCache();
    }

    static final String TAG = "PVRTextureData";

    static {
        System.loadLibrary("pvrtexture");
    }

    String filePath;
    boolean useMipMaps;
    int width = 0;
    int height = 0;
    boolean isPrepared = false;
    AssetManager mAssetManager = null;
    int resourceType;

    private int objectId;

    /**
     * the asset folder pvr texture
     * 
     * @param assetManager
     *            , the asset manager
     * @param file
     *            , the file path in asset folder
     */
    public PVRTextureData(AssetManager assetManager, String file) {
        init(assetManager, file, TYPE_RESOURCE_ASSETS);
    }

    /**
     * the external folder resource pvr texture
     * 
     * @param externalFilePath
     */
    public PVRTextureData(String externalFilePath) {
        init(null, externalFilePath, TYPE_RESOURCE_EXTERNAL);
    }

    private void init(AssetManager assetManager, String file, int type) {
        mAssetManager = assetManager;
        filePath = file;
        if (filePath == null) {
            throw new RuntimeException("file path is null");
        }
        objectId = filePath.hashCode();
        resourceType = type;
    }

    /**
     * dispose the texture data
     */
    public void dispose() {
        native_dispose(objectId);
    }

    @Override
    public TextureDataType getType() {
        return TextureDataType.Compressed;
    }

    @Override
    public boolean isPrepared() {
        return isPrepared;
    }

    @Override
    public void prepare() {
        if (isPrepared) {
            throw new GdxRuntimeException("Already prepared");
        }
        native_preparePVRTexture(objectId, mAssetManager, filePath, resourceType);

        width = native_getWidth(objectId);
        height = native_getHeight(objectId);
        useMipMaps = native_useMipMaps(objectId);

        isPrepared = true;
    }

    @Override
    public void consumeCompressedData() {
        if (!isPrepared) {
            throw new GdxRuntimeException("Call prepare() before calling consumeCompressedData()");
        }
        native_consumeCompressedData(objectId);
        isPrepared = false;
    }

    @Override
    public Pixmap consumePixmap() {
        throw new GdxRuntimeException("This TextureData implementation does not return a Pixmap");
    }

    @Override
    public boolean disposePixmap() {
        throw new GdxRuntimeException("This TextureData implementation does not return a Pixmap");
    }

    @Override
    public int getWidth() {
        return width;
    }

    @Override
    public int getHeight() {
        return height;
    }

    @Override
    public Format getFormat() {
        return Format.RGB565;
    }

    @Override
    public boolean useMipMaps() {
        return useMipMaps;
    }

    @Override
    public boolean isManaged() {
        return true;
    }

    public native void native_preparePVRTexture(int objectId, AssetManager assetManager, String path, int type);

    public native void native_consumeCompressedData(int objectId);

    public native int native_getWidth(int objectId);

    public native int native_getHeight(int objectId);

    public native boolean native_useMipMaps(int objectId);

    public native void native_dispose(int objectId);

    public static native void native_setTextureCacheEnable(boolean cacheEnable);

    public static native void native_disposeAllTextureCache();

}
