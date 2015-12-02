
package com.yunos.assistant.ui.card;

public class ImageUtilEngine {

    static {
        System.loadLibrary("StackBlur");
    }

    public native int[] stackBlur(int[] imageIn, int width, int height, int radius, int shift, int iterations);
    public native int isBlackBackground(int[] imageIn, int width, int height);
}
