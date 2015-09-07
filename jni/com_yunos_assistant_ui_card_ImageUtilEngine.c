#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "com_yunos_assistant_ui_card_ImageUtilEngine.h"

#include <android/log.h>
#include <android/bitmap.h>
#include <math.h>
#define LOG_TAG "Filters"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

/*-------------------*/
/* utility functions */
/*-------------------*/

int min(int x, int y) {
	return (x <= y) ? x : y;
}
int max(int x, int y) {
	return (x >= y) ? x : y;
}
int alpha(int color) {
	return (color >> 24) & 0xFF;
}
int red(int color) {
	return (color >> 16) & 0xFF;
}
int green(int color) {
	return (color >> 8) & 0xFF;
}
int blue(int color) {
	return color & 0xFF;
}
int ARGB(int alpha, int red, int green, int blue) {
	return (alpha << 24) | (red << 16) | (green << 8) | blue;
}
inline int clamp(int t, int low, int high) {
	if (t < high) {
		return ((t > low) ? t : low);
	}
	return high;
}
int* clone(int* src, int* dst, int length) {
	int i = 0;
	for (i = 0; i < length; i++) {
		dst[i] = src[i];
	}
}

void _stackBlurH(jint* src, jint* dst, int height, int width, int radius) {
	int x, y, i, curPixel;
	int pixA, pixR, pixG, pixB;
	int preColor, curColor, nextColor;

	radius += 1;

	for (y = 0; y < height; y++) {
		int rSum = 0, gSum = 0, bSum = 0;
		int rStackIn = 0, gStackIn = 0, bStackIn = 0;
		int rStackOut = 0, gStackOut = 0, bStackOut = 0;

		curPixel = y * width;
		curColor = src[curPixel];

		rStackOut = red(curColor) * radius;
		gStackOut = green(curColor) * radius;
		bStackOut = blue(curColor) * radius;

		rSum = (rStackOut * (radius - 1)) >> 1;
		gSum = (gStackOut * (radius - 1)) >> 1;
		bSum = (bStackOut * (radius - 1)) >> 1;

		for (i = 0; i < radius; i++) {
			curColor = src[i + curPixel];
			rStackIn += red(curColor);
			gStackIn += green(curColor);
			bStackIn += blue(curColor);

			rSum += rStackIn;
			gSum += gStackIn;
			bSum += bStackIn;
		}

		for (x = 0; x < width; x++, curPixel++) {
			curColor = src[curPixel];
			pixA = alpha(curColor);

			pixR = rSum >> 8;
			pixG = gSum >> 8;
			pixB = bSum >> 8;

			dst[curPixel] = ARGB(pixA, pixR, pixG, pixB);

			if (x + radius < width) {
				nextColor = src[curPixel + radius];
			} else {
				nextColor = src[(y + 1) * width - 1];
			}

			if (x - radius > 0) {
				preColor = src[curPixel - radius];
			} else {
				preColor = src[y * width];
			}

			rStackIn += red(nextColor);
			gStackIn += green(nextColor);
			bStackIn += blue(nextColor);

			rStackIn -= red(curColor);
			gStackIn -= green(curColor);
			bStackIn -= blue(curColor);

			rStackOut += red(curColor);
			gStackOut += green(curColor);
			bStackOut += blue(curColor);

			rStackOut -= red(preColor);
			gStackOut -= green(preColor);
			bStackOut -= blue(preColor);

			rSum += rStackIn;
			gSum += gStackIn;
			bSum += bStackIn;

			rSum -= rStackOut;
			gSum -= gStackOut;
			bSum -= bStackOut;
		}
	}

}

void _stackBlurV(jint* src, jint* dst, int height, int width, int radius) {
	int x, y, i, curPixel;
	int pixA, pixR, pixG, pixB;
	int preColor, curColor, nextColor;

	radius += 1;
	const int c_blockStep = radius * width;

	for (x = 0; x < width; x++) {
		int rSum = 0, gSum = 0, bSum = 0;
		int rStackIn = 0, gStackIn = 0, bStackIn = 0;
		int rStackOut = 0, gStackOut = 0, bStackOut = 0;

		curPixel = x;
		curColor = src[curPixel];

		rStackOut = red(curColor) * radius;
		gStackOut = green(curColor) * radius;
		bStackOut = blue(curColor) * radius;

		rSum = (rStackOut * (radius - 1)) >> 1;
		gSum = (gStackOut * (radius - 1)) >> 1;
		bSum = (bStackOut * (radius - 1)) >> 1;

		for (i = 0; i < c_blockStep; i += width) {
			curColor = src[curPixel + i];
			rStackIn += red(curColor);
			gStackIn += green(curColor);
			bStackIn += blue(curColor);

			rSum += rStackIn;
			gSum += gStackIn;
			bSum += bStackIn;
		}

		for (y = 0; y < height; y++, curPixel += width) {
			curColor = src[curPixel];
			pixA = alpha(curColor);

			pixR = rSum >> 8;
			pixG = gSum >> 8;
			pixB = bSum >> 8;

			dst[curPixel] = ARGB(pixA, pixR, pixG, pixB);

			if (y + radius < height) {
				nextColor = src[curPixel + c_blockStep];
			} else {
				nextColor = src[x + (height - 1) * width];
			}

			if (y - radius > 0) {
				preColor = src[curPixel - c_blockStep];
			} else {
				preColor = src[x];
			}

			rStackIn += red(nextColor);
			gStackIn += green(nextColor);
			bStackIn += blue(nextColor);

			rStackIn -= red(curColor);
			gStackIn -= green(curColor);
			bStackIn -= blue(curColor);

			rStackOut += red(curColor);
			gStackOut += green(curColor);
			bStackOut += blue(curColor);

			rStackOut -= red(preColor);
			gStackOut -= green(preColor);
			bStackOut -= blue(preColor);

			rSum += rStackIn;
			gSum += gStackIn;
			bSum += bStackIn;

			rSum -= rStackOut;
			gSum -= gStackOut;
			bSum -= bStackOut;
		}
	}
}

/*------------------*/
/* filter functions */
/*------------------*/

jintArray Java_com_yunos_assistant_ui_card_ImageUtilEngine_stackBlur(
		JNIEnv* env, jobject thiz, jintArray buf, jint width, jint height,
		jint radius, jint iterations) {

	clock_t start, finish;
	start = clock();

	jint * cbuf;
	cbuf = (*env)->GetIntArrayElements(env, buf, 0);

	int newSize = width * height;
	jint * rbuf = (jint *) malloc(newSize * sizeof(jint)); // 新图像像素值

	if (radius > width || radius > height)
		LOGE("radius: %d is invalid in stack blur", radius);

	radius = 15;
	_stackBlurH(cbuf, rbuf, height, width, radius);
	_stackBlurV(rbuf, cbuf, height, width, radius);

	free(rbuf);

	finish = clock();
	double duration = (double) (finish - start) / CLOCKS_PER_SEC;
	LOGE("STACK BLUR TACKS %f sec", duration);

	return buf;
}

int Java_com_yunos_assistant_ui_card_ImageUtilEngine_isBlackBackground(JNIEnv* env, jobject thiz, jintArray buf, jint width, jint height) {

	clock_t start, finish, middle;
	start = clock();

	jint * src;
	src = (*env)->GetIntArrayElements(env, buf, 0);

    int count = 400;
    int* array = (int *) malloc(count * sizeof(int));

    //sample only on horizon
    int left = ceil(width * 0.1);
    int right = floor(width * 0.9);
    int top = ceil(height * 0.15);
    int top2 = ceil(height * 0.25);
    int bottom = floor(height * 0.85);
    int bottom2 = floor(height * 0.75);
    int sampleWidth = right - left;
    LOGE("sampleWidth=  %d ", sampleWidth);
    int horizonStep = (sampleWidth * 4) / count;

    int i = 0;
    int step;
    int leftTopPointIndex = left + top * width;
    LOGE("leftTopPointIndex=  %d ", leftTopPointIndex);
    LOGE("horizonStep=  %d ", horizonStep);
    for(step = 0; step < sampleWidth && i < count; i++) {
    	array[i] = src[leftTopPointIndex + step];
    	step += horizonStep;
    }

    LOGE("i =  %d ", i);

    int leftTop2PointIndex = left + top2 * width;
    for(step = 0; step < sampleWidth && i < count; i++) {
    	array[i] = src[leftTopPointIndex + step];
    	step += horizonStep;
    }

    LOGE("i =  %d ", i);

    int leftBottomPointIndex = left + bottom * width;
    for(step = 0; step < sampleWidth && i < count; i++) {
       	array[i] = src[leftBottomPointIndex + step];
       	step += horizonStep;
       }

    LOGE("i =  %d ", i);

    int leftBottom2PointIndex = left + bottom2 * width;
    for(step = 0; step < sampleWidth && i < count; i++) {
       	array[i] = src[leftBottomPointIndex + step];
       	step += horizonStep;
       }

    LOGE("i =  %d ", i);


    /* sample on a rectangle
     *
    int left = ceil(width * 0.2);
    int right = floor(width * 0.8);
    int top = ceil(height * 0.2);
    int bottom = floor(height * 0.8);
    int sampleWidth = right - left;
    int sampleHeight = bottom - top;

    int horizonCount = count * ((float)sampleWidth / (sampleWidth + sampleHeight));
    int verticalCount = count - horizonCount;

    int horizonStep = (sampleWidth * 2) / horizonCount;
    int vertiacalStep = (sampleHeight * 2) / verticalCount;

    int leftTopPointIndex = left + top * width;
    int i = 0;
    int step;
    LOGE("sampleWidth=  %d ", sampleWidth);
    LOGE("leftTopPointIndex=  %d ", leftTopPointIndex);
    LOGE("horizonStep=  %d ", horizonStep);
    for(step = 0; step < sampleWidth && i < count; i++) {
    	array[i] = src[leftTopPointIndex + step];
    	step += horizonStep;
    }

    LOGE("i =  %d ", i);

    int leftBottomPointIndex = left + bottom * width;
    for(step = 0; step < sampleWidth && i < count; i++) {
       	array[i] = src[leftBottomPointIndex + step];
       	step += horizonStep;
       }

    LOGE("i =  %d ", i);

    for(step = vertiacalStep; step < sampleHeight && i < count; i++) {
       	array[i] = src[leftTopPointIndex + step * width];
       	step += vertiacalStep;
       }

    LOGE("i =  %d ", i);

    int rightTopPointIndex = (width - left) + top * width;
    for(step = 0; step <= sampleHeight && i < count; i++) {
       	array[i] = src[rightTopPointIndex + step * width];
       	step += vertiacalStep;
       }

    LOGE("i =  %d ", i);*/

    middle = clock();
    double duration2 = (double) (middle - start) / CLOCKS_PER_SEC;
    LOGE("STACK middle TACKS %f sec", duration2);


    int totalGrayPix = 0;
    int j;
    for(j = 0; j <= i; j++) {
    	 int color = array[j];
    	 int r = red(color);
    	 int g = green(color);
    	 int b = blue(color);
    	 int tmpValue = (r * 19595 + g * 38469 + b * 7472) >> 16;
    	 totalGrayPix += tmpValue;
    }

    free(array);

    int avgGray = totalGrayPix / (i + 1);

	finish = clock();
	double duration = (double) (finish - start) / CLOCKS_PER_SEC;
	LOGE("STACK end TACKS %f sec", duration);


    if (avgGray < 50)
    {
    	LOGE("so black");
    	return 0;
    }else {
    	LOGE("so else");
    	return -1;
    }
}





