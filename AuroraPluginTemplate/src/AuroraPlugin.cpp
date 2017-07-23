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
int prevSliceLit = 0;
static int currentAuroraRotation = 0;

static bool didLoadPluginByLeo = false;

RGB_t * colorPalette = NULL;
int nColors = 0;
RGB_t sliceColor;
int colorIndex = 1;
RGB_t baseColor;

/**
 * @description: Initialize the plugin. Called once, when the plugin is loaded.
 * This function can be used to enable rhythm or advanced features,
 * e.g., to enable energy feature, simply call enableEnergy()
 * It can also be used to load the LayoutData and the colorPalette from the DataManager.
 * Any allocation, if done here, should be deallocated in the plugin cleanup function
 *
 */

void initPlugin() {
	//do allocation here
	enableEnergy();
	enableBeatFeatures();

	getColorPalette(&colorPalette, &nColors);

	layoutData = getLayoutData();

	//quantizes the layout into frameslices. See SDK documentation for more information
	getFrameSlicesFromLayoutForTriangle(layoutData, &frameSlices, &nFrameSlices,
			currentAuroraRotation);

	baseColor = {255, 255, 255};
	if (nColors == 0) {
		sliceColor = {0, 0, 0};
	}
	else {
		sliceColor = colorPalette[0];
	}
	//y = (rand() % (maxValue - minValue + 1)) + minValue;

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
void getPluginFrame(Frame_t* frames, int* nFrames, int* sleepTime) {
#define SKIP_COUNT 1
	// a tiny block of code that allows the developer to skips calls of this function.
	//if this plugin wishes to run only every 150ms for instance, the skip count would be set to 2
	static int cnt = 0;
	if (cnt < SKIP_COUNT) {
		cnt++;
		return;
	}
	cnt = 0;

	int sliceToLight;
	int frameIndex = 0;
	PRINTLOG("a function called\n");
	PRINTLOG("Max slice %d", nFrameSlices);
	if (nColors >= 2) {
		if (colorIndex > nColors) {
			colorIndex = 1;
		}
		sliceColor = colorPalette[colorIndex++];
	}
	PRINTLOG("\nSlicecolors: R: %d, G: %d, B: %d\n", sliceColor.R, sliceColor.G,
			sliceColor.B);

	for (int i = 0; i < nFrameSlices; i++) {
		for (unsigned int j = 0; j < frameSlices[i].panelIds.size(); j++) {
			PRINTLOG("i: %d j: %d\n", i, j);
			frames[j].panelId = frameSlices[i].panelIds[j];
			frames[j].r = baseColor.R;
			frames[j].g = baseColor.G;
			frames[j].b = baseColor.B;
			frames[j].transTime = 1;
			frameIndex++;
		}
	}
	*nFrames = frameIndex;
	if (getIsBeat()) {
		PRINTLOG("beatFunction \n");
		do {
			sliceToLight = (rand() % nFrameSlices);
		} while (sliceToLight == prevSliceLit);
		PRINTLOG("Random number: %d \n", sliceToLight);
		for (unsigned int j = 0; j < frameSlices[sliceToLight].panelIds.size();
				j++) {
			frames[j].panelId = frameSlices[sliceToLight].panelIds[j];
			frames[j].r = sliceColor.R;
			frames[j].g = sliceColor.G;
			frames[j].b = sliceColor.B;
			frames[j].transTime = 1;
		}
		prevSliceLit = sliceToLight;
	}

}

/**
 * @description: called once when the plugin is being closed.
 * Do all deallocation for memory allocated in initplugin here
 */
void pluginCleanup() {
//do deallocation here
	freeFrameSlices(frameSlices);
}
