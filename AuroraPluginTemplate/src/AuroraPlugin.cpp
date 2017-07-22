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
void initPlugin() {

	enableEnergy();
	enableBeatFeatures();

	LayoutData* layoutData;
	FrameSlice_t* frameSlices = NULL;
	int nFrameSlices = 0;
	static int currentAuroraRotation = 0;

	RGB_t * colorPalette = NULL;
	int nColors = 0;
	RGB_t pulseColorRight;
	RGB_t pulseColorLeft;
	RGB_t netColor
	RGB_t baseColor

	int lifeTime1, lifeTime2;

	getFrameSlicesFromLayoutForTriangle(layoutData, &frameSlices, &nFrameSlices, currentAuroraRotation);

	getColorPalette(&colorPalette, &nColors);

	if (nColors == 0){
	        pulseColorRight = {255, 255, 255};
	        pulseColorLeft = {0, 0, 0};
	    }
	    else if (nColors == 1){
	        pulseColorRight = colorPalette[0];
	        pulseColorLeft = {0, 0, 0};
	    }
	    else if (nColors >= 2){
	        pulseColorRight = colorPalette[0];
	        pulseColorLeft = colorPalette[1];
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

	if (getIsBeat) && (lifeTime1 == maxTravel){
lifeTime = 0;

	}

}

/**
 * @description: called once when the plugin is being closed.
 * Do all deallocation for memory allocated in initplugin here
 */
void pluginCleanup() {
	//do deallocation here
}
