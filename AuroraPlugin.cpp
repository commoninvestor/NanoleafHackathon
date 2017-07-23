/*
    Copyright 2017 Nanoleaf Ltd.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

#include "AuroraPlugin.h"
#include "LayoutProcessingUtils.h"
#include "ColorUtils.h"
#include "DataManager.h"
#include "PluginFeatures.h"
#include "Logger.h"
#include <stdlib.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

	void initPlugin();
	void getPluginFrame(Frame_t* frames, int* nFrames, int* sleepTime);
	void pluginCleanup();

#ifdef __cplusplus
}
#endif

LayoutData* layoutData;
FrameSlice_t* frameSlices = NULL;
int nFrameSlices = 0;

static int currentAuroraRotation = 0;

RGB_t * colorPalette = NULL;
int nColors = 0, colorIndex = 1;
RGB_t sliceColor, baseColor;

/**
 * @description: Initialize the plugin. Called once, when the plugin is loaded.
 * This function can be used to enable rhythm or advanced features,
 * e.g., to enable energy feature, simply call enableEnergy()
 * It can also be used to load the LayoutData and the colorPalette from the DataManager.
 * Any allocation, if done here, should be deallocated in the plugin cleanup function
 *
 */
void initPlugin(){

	enableBeatFeatures();

	getFrameSlicesFromLayoutForTriangle(layoutData, &frameSlices, &nFrameSlices, currentAuroraRotation);

	getColorPalette(&colorPalette, &nColors);

	baseColor = {255, 255, 255}; // always white
	if (nColors >= 1){
		sliceColor = colorPalette[0];
	} else {
		sliceColor = {0, 0, 0};
	}

}

/**
 * @description: this the 'main' function that gives a frame to the Aurora to display onto the panels
 * To obtain updated values of enabled features, simply call get<feature_name>, e.g.,
 * getEnergy(), getIsBeat().
 *
 * If the plugin is a sound visualization plugin, the sleepTime variable will be NULL and is not required to be
 * filled in
 * This function, if is an effects plugin, can specify the interval it is to be called at through the sleepTime variable
 * if its a sound visualization plugin, this function is called at an interval of 50ms or more.
 *
 * @param frames: a pre-allocated buffer of the Frame_t structure to fill up with RGB values to show on panels.
 * Maximum size of this buffer is equal to the number of panels
 * @param nFrames: fill with the number of frames in frames
 * @param sleepTime: specify interval after which this function is called again, NULL if sound visualization plugin
 */
void getPluginFrame(Frame_t* frames, int* nFrames, int* sleepTime){

	int frameIndex = 0, sliceToLight = 0, slicePreviousLit = 0;

	//light up according to beat
	if (getIsBeat){

		// prevent same column of panels from being lit up two consecutive times
		do {
			sliceToLight = rand() % (nFrameSlices);
		} while (sliceToLight == slicePreviousLit);

		//select color for the column to light up in --> sequential
		if (nColors >= 2){
			if (colorIndex >= nColors){
				colorIndex = 1;
			}
			sliceColor = colorPalette[colorIndex++];
		}

		//light up the slice in color then fade back to white
		for (int i = 0; i < frameSlices[sliceToLight].panelIds.size(); i ++){
			frames[frameIndex].panelId = frameSlices[sliceToLight].panelIds[i];
			frames[frameIndex].r = sliceColor.R;
			frames[frameIndex].b = sliceColor.B;
			frames[frameIndex].g = sliceColor.G;
			frames[frameIndex].transTime = 1;

			usleep(50000); //waits 50 ms

			frames[frameIndex].r = baseColor.R;
			frames[frameIndex].b = baseColor.B;
			frames[frameIndex].g = baseColor.G;
			frames[frameIndex].transTime = 1;

			frameIndex ++;

		}

		//updates the most recently lit frame slice
		slicePreviousLit = sliceToLight;

	}

	*nFrames = frameIndex;
	*sleepTime = 1;
}

/**
 * @description: called once when the plugin is being closed.
 * Do all deallocation for memory allocated in initplugin here
 */
void pluginCleanup(){
	//do deallocation here
	 freeFrameSlices(frameSlices);
}
