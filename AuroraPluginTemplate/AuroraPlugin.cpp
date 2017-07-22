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
#include <limits.h>
#include "AveragingFilter.h"

#ifdef __cplusplus
extern "C" {
#endif

void initPlugin();
void getPluginFrame(Frame_t* frames, int* nFrames, int* sleepTime);
void pluginCleanup();

#ifdef __cplusplus
}
#endif

/**
 * @description: Initialize the plugin. Called once, when the plugin is loaded.
 * This function can be used to enable rhythm or advanced features,
 * e.g., to enable energy feature, simply call enableEnergy()
 * It can also be used to load the LayoutData and the colorPalette from the DataManager.
 * Any allocation, if done here, should be deallocated in the plugin cleanup function
 *
 */
int nColors = 0;
int colorIndex = 1;

void initPlugin() {
	enableEnergy();
	enableBeatFeatures();

	LayoutData* layoutData = NULL;
	FrameSlice_t* frameSlices = NULL;
	int nFrameSlices = 0;
	static int currentAuroraRotation = 0;

	RGB_t * colorPalette = NULL;
	RGB_t barColor;
	RGB_t netColor;
	RGB_t baseColor;



	getFrameSlicesFromLayoutForTriangle(layoutData, &frameSlices,
			&nFrameSlices, currentAuroraRotation);

	getColorPalette(&colorPalette, &nColors);

	if (nColors == 0) {
		barColor = {255, 255, 255};
		baseColor = {0, 0, 0};
	}
	else if (nColors == 1) {
		barColor = colorPalette[0];
		baseColor = {0, 0, 0};
	}
	else if (nColors >= 2) {
		barColor = colorPalette[0];
		baseColor = colorPalette[1];
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
void getPluginFrame(Frame_t* frames, int* nFrames, int* sleepTime) {

	static const int32_t maxBarLength = 100;
	static int32_t barMarker = 0;
	static const int barMarkerDelay = 45.0;
	int frameIndex = 0;
	uint16_t energy = getEnergy();

	af.feedFilter(energy);
	double avgEnergy = af.getAverage();

	if (nColors >= 2)
	{
		if (colorIndex >= nColors) {
			colorIndex = 1;
		}
		barColor = colorPalette[colorIndex++];
		barColorTimer = 0;
	}

	static double maxEnergy = energy;
	if (avgEnergy > maxEnergy) {
		maxEnergy = maxEnergy / 0.8;
	} else {
		maxEnergy = maxEnergy * 0.8;
	}

	if (maxEnergy < 512) {
		maxEnergy = 512;
	}
	int32_t barLength = (energy * maxBarLength) / (2 * maxEnergy);

	if (barLength > barMarker) {
		barMarker = barLength;
	}

	//map the length of the soundbar to the Aurora Layout. In this case,
	//calculate the number of frames that are affected from the total number of frame slices
	int nFramesAffected = (barMarker * nFrameSlices) / maxBarLength;
	if (nFramesAffected > nFrameSlices) {
		nFramesAffected = nFrameSlices;
	}

	int x_start = 450;
	int x = x_start;
	int x_step = (nFramesAffected == 0) ? 0 : x_start / nFramesAffected;
	for (int i = 0; i < nFramesAffected; i++) {
		x = x - x_step;
		int x_t = (x > 255) ? 255 : x;
		//the net color is a mix between a weighted base color and bar color.
		//As the frameSlices moves towards the end of the bar, the effect of the bar color decreases
		//and the base color becomes stronger and stronger.
		//In other words the bar color fades into the base color
		netColor =
				(((barColor * x_t) / 255) + ((baseColor * (255 - x_t))) / 255);
		netColor = limitRGB(netColor, 255, 0);
		for (unsigned int j = 0; j < frameSlices[i].panelIds.size(); j++) {
			frames[frameIndex].panelId = frameSlices[i].panelIds[j];
			frames[frameIndex].r = netColor.R;
			frames[frameIndex].g = netColor.G;
			frames[frameIndex].b = netColor.B;
			frames[frameIndex].transTime = 1;
			frameIndex++;
		}
	}
	for (int i = nFramesAffected; i < nFrameSlices; i++) {
		for (unsigned int j = 0; j < frameSlices[i].panelIds.size(); j++) {
			frames[frameIndex].panelId = frameSlices[i].panelIds[j];
			frames[frameIndex].r = baseColor.R;
			frames[frameIndex].g = baseColor.G;
			frames[frameIndex].b = baseColor.B;
			frames[frameIndex].transTime = 1;
			frameIndex++;
		}
	}

	if (barMarker > 0) {
		//the soundbar relaxes to 0 slowly, instead of jumping to the level of the sound immediately.
		//gives a more 'organic' effect.
		barMarker -= barMarkerInertialRelaxationStep;
		if (barMarker < 0) {
			barMarker = 0;
		}
	}

	*nFrames = frameIndex;
}

/**
 * @description: called once when the plugin is being closed.
 * Do all deallocation for memory allocated in initplugin here
 */
void pluginCleanup() {
	//do deallocation here
	freeFrameSlices (frameSlices);
}
