
package com.yunos.assistant.ui.card;

import android.app.Activity;
import android.app.WallpaperManager;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ImageView;

public class MainActivity extends Activity {
    private ImageView img;
    private Bitmap bm;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        img = (ImageView) findViewById(R.id.img);
        
        
        Drawable dr = WallpaperManager.getInstance(this.getApplicationContext()).getDrawable();
        BitmapDrawable bd = (BitmapDrawable) dr;
        bm = bd.getBitmap();
        
        ImageUtilEngine imageEngine = new ImageUtilEngine();
        int scaleWidth = bm.getWidth();
        int scaleHeight = bm.getHeight();
        int[] colorArray = new int[scaleWidth * scaleHeight];
        bm.getPixels(colorArray, 0, scaleWidth, 0, 0, scaleWidth, scaleHeight);
        Log.i("Filters", "starting....");
        int isBlack = imageEngine.isBlackBackground(colorArray,scaleWidth, scaleHeight);
        if(isBlack == 0) {
            Log.i("Filters", "black");
        }else {
            int[] arrayOut = imageEngine.stackBlur(colorArray, scaleWidth, scaleHeight, 15, 8, 0);
            img.setBackground(new BitmapDrawable(Bitmap.createBitmap(arrayOut, scaleWidth, scaleHeight, Bitmap.Config.ARGB_8888)));
        }        
    }

    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub
        super.onDestroy();
        //bm.recycle();
    }
    
    
}
