#pragma once

#include "ofMain.h"
#include "ofxGui.h"


class nCKinectCamera : public ofNode {

private:

	void draw();
	ofVec4f floorplane;

	ofParameterGroup params;
	ofParameterGroup getParams() { return params; }

	//GUI
	ofxPanel gui;
	ofParameter <float> kinectcamxposin3dworld;
	ofParameter <float> kinectcamyposin3dworld;
	ofParameter <float> kinectcamzposin3dworld;
	ofParameter <float> kinectyawin3dworld;
	ofParameter <bool> freezeFloorplane;
	ofParameter <bool> drawCamera;
	ofParameter <bool> mapKinectto3DWorld;
	void setupGUI();
	int loadcounter;
	int freezefloorplanecounter;
	bool floorplaneshouldbein;
public:

	~nCKinectCamera();
	nCKinectCamera();

	ofBoxPrimitive model;
	ofMatrix4x4 realworldkinecttransformmatrix;

	void setup();

	void begin();

	void end();

	void update(ofVec4f _floorplane);

	void drawGUI();

	ofVec4f getFloorPlane();

	ofVec3f getKinectCameraPosition();
	ofQuaternion getKinectCameraRotation();

	void setTransformMatrix(const ofMatrix4x4 &m44);

	
};