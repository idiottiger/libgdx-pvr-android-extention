package pkg.example.pvr;

import pkg.libgdx.pvrextention.PVRTextureData;

import android.content.Context;
import android.content.res.AssetManager;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Screen;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;

public class GameScreen extends Game {

    private SpriteBatch spriteBatch;
    private Texture pvrTexture1;

    private AssetManager assetManager;

    public GameScreen(Context context) {
        assetManager = context.getAssets();
    }

    @Override
    public void create() {
        spriteBatch = new SpriteBatch();
        setScreen(new MyScreen());
    }

    class MyScreen implements Screen {

        @Override
        public void dispose() {
            // TODO Auto-generated method stub
            if (pvrTexture1 != null) {
                pvrTexture1.dispose();
            }
        }

        @Override
        public void hide() {
            // TODO Auto-generated method stub

        }

        @Override
        public void pause() {
            // TODO Auto-generated method stub
        }

        @Override
        public void render(float arg0) {
            spriteBatch.begin();
            spriteBatch.draw(pvrTexture1, 0, 0);
            spriteBatch.end();
        }

        @Override
        public void show() {
            if (pvrTexture1 == null) {
                pvrTexture1 = new Texture(new PVRTextureData(assetManager, "test.pvr"));
            }
        }

        @Override
        public void resize(int arg0, int arg1) {
            // TODO Auto-generated method stub

        }

        @Override
        public void resume() {
            // TODO Auto-generated method stub

        }

    }

}
