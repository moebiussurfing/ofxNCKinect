#include "ncKinectPCTriangulator.h"

//--------------------------------------------------------------
void ncKinectPCTriangulator::setup(vector<ncKinectUser*> &_users, ncKinectv2Core &_core,nCKinectCamera &_camera, ofPixels &_usermap, ofMesh & _pointcloud,int _id) {
		
	id = _id;
	users = &_users;
	kinectcamera = &_camera;
	usermap = &_usermap;
	pointcloud = &_pointcloud;
	core = &_core;

	kinect2sscene.setup(*users,id);

	cam.setDistance(5);
	cam.setNearClip(0.01);
	ofxLoadCamera(cam, "ofEasyCamSettings");

	//gui.setup("Mesh Triangulator"+ofToString(_id), "_settings/triangulator"+ofToString(_id)+".xml");
	//gui.add(bDrawGrid.set("draw grid", false));
	//gui.add(bRotate.set("rotate", false));
	//gui.add(bClampPosition.set("clamp position", true));
	//gui.add(colorblend.set("color blend", 0.5,0,1));
	//gui.add(bDoPointCloud.set("point cloud", false));
	//gui.add(pointSize.set("point size",1,1,10));
	//gui.add(bDrawContours.set("draw debug contours", false));

	gui.setup();
	params.setName("Mesh Triangulator");
	params.add(bDrawGrid.set("draw grid", false));
	params.add(bRotate.set("rotate", false));
	params.add(bClampPosition.set("clamp position", true));
	params.add(colorColorBlend.set("color", ofColor(255,255), ofColor(0,0), ofColor(255,255)));
	params.add(colorblend.set("color blend", 0.5, 0, 1));
	params.add(bDoPointCloud.set("point cloud", false));
	params.add(pointSize.set("point size", 1, 0.1, 10));
	params.add(bDrawContours.set("draw debug contours", false));
	gui.add(params);

	gui.setPosition(250, 10);

	//gui.loadFromFile("_settings/triangulator"+ofToString(id)+".xml");
	
	//MESH STUFF
	sampleSize = 2;

	depthWidth = 512;
	depthHeight = 424;

	sampledWidth = depthWidth / sampleSize;
	sampledHeight = depthHeight / sampleSize;


	ofFbo::Settings settings;
	settings.internalformat = GL_LUMINANCE;
	settings.useDepth = false;
	settings.width = 512;
	settings.height = 424;
	
	silhouettebuffer.allocate(settings);
	silhouettespixels.allocate(512, 424, OF_IMAGE_GRAYSCALE);
	
	vCount = 0;
	tCount = 0;
	//startThread();
}

//--------------------------------------------------------------
void ncKinectPCTriangulator::update() {
	kinect2sscene.update();
	silhouettebuffer.begin();
	ofClear(0, 0, 0, 255);
	kinect2sscene.drawFilledContours(*usermap);
	silhouettebuffer.end();
	silhouettebuffer.readToPixels(silhouettespixels);
	
	estimateUserVertices(vCount, tCount);

	usermesh.clear();

	
	int index = 0, vIndex = 0, tIndex = 0, xyIndex = 0;

	vertices.resize(vCount);
	triangles.resize(6 * tCount);
	uvs.resize(vCount);
	vector<ofFloatColor> colors;
	colors.resize(vCount);
	
	for (int y = 0; y < depthHeight; y += sampleSize) {
		int xyStartIndex = xyIndex;
		
		for (int x = 0; x < depthWidth; x += sampleSize) {
			
			glm::vec3 vSpacePos = pointcloud->getVertex(xyIndex);
			
			if (vertexType[index] != 0 && !isinf(vSpacePos.x) && !isinf(vSpacePos.y) && !isinf(vSpacePos.z)) {
				
				ofVec2f  colorPoint = core->convert2dPointToColorSpacePoint(glm::vec2(x, y));
				// Set Color to Point
				int colorX = static_cast<int>(colorPoint.x + 0.5f);
				int colorY = static_cast<int>(colorPoint.y + 0.5f);

				ofColor col;
				
				if ((0 <= colorX) && (colorX < 1920) && (0 <= colorY) && (colorY < 1080)) {
					col = core->getColorPixels().getColor(colorX, colorY);
					ofColor bcolor(ofColor::blueSteel);
					//col.lerp(bcolor, colorblend);
					col.lerp(colorColorBlend.get(), colorblend);
					colors[vIndex] = col;
				}

				
				vertices[vIndex] = vSpacePos;
				uvs[vIndex] = ofVec2f((float)x / depthWidth, (float)y / depthHeight);
				vIndex++;

				if (vertexType[index] == 3) {

					triangles[tIndex++] = vertexIndex[index];  // top left
					triangles[tIndex++] = vertexIndex[index + 1];  // top right
					triangles[tIndex++] = vertexIndex[index + sampledWidth];  // bottom left

					triangles[tIndex++] = vertexIndex[index + sampledWidth];  // bottom left
					triangles[tIndex++] = vertexIndex[index + 1];  // top right
					triangles[tIndex++] = vertexIndex[index + sampledWidth + 1];  // bottom right

				}
			}
			
			index++;
			xyIndex += sampleSize;
		}
		xyIndex = xyStartIndex + sampleSize * depthWidth;
	
	}
	usermesh.addTexCoords(uvs);
	usermesh.addVertices(vertices);
	usermesh.addIndices(triangles);
	usermesh.addColors(colors);
	ofxMeshUtils::calcNormals(usermesh);
}

//--------------------------------------------------------------
void ncKinectPCTriangulator::draw() {
	if (bDrawContours) {
		kinect2sscene.drawUserMap2d(*usermap, 0, 0);
		kinect2sscene.drawContourLines(*usermap, 512, 0);

		silhouetteimage.setFromPixels(silhouettespixels);
		silhouetteimage.draw(512, 0);
	}
	cam.begin();
	ofEnableDepthTest();

	if (bDrawGrid) {
		if (bRotate) {
			ofPushMatrix();
			ofRotate(ofGetFrameNum()*0.3, 0, 1, 0);
		}
		ofSetColor(ofColor::pink, 150);
		drawGridOneColor(0.5, 10, false, false, true, false);
		ofSetColor(255);
		if (bRotate) {
			ofPopMatrix();
		}
	}

	kinectcamera->begin();

	if (bDoPointCloud) {
		glPointSize(pointSize);
		usermesh.drawVertices();
		glPointSize(1);
	}
	else {
		//usermesh.draw();
		ofPushStyle();
		ofSetLineWidth(pointSize.get());
		usermesh.drawWireframe();
		ofPopStyle();
	}
	kinectcamera->end();

	ofDisableDepthTest();
	cam.end();

	
}

//--------------------------------------------------------------
void ncKinectPCTriangulator::drawGUI() {
	gui.draw();
	kinect2sscene.drawGUI();
}

ofMesh & ncKinectPCTriangulator::getUserMesh() {
	return usermesh;
}

void ncKinectPCTriangulator::threadedFunction() {
	while (isThreadRunning()) {
		
		estimateUserVertices(vCount, tCount);

		
	}
}

void ncKinectPCTriangulator::estimateUserVertices(int & vCount, int & tCount) {

	vertexType.clear();
	vertexType.resize(sampledWidth * sampledHeight);

	vertexIndex.clear();
	vertexIndex.resize(sampledWidth * sampledHeight);

	vector<glm::vec3> vSpacePos;
	vSpacePos.assign(4, glm::vec3(0));

	int rowIndex = 0;

	int counter = 0;

	for (int y = 0; y < sampledHeight - 1; y++) {

		int pixIndex = rowIndex;

		for (int x = 0; x < sampledWidth - 1; x++) {

			if (isUserSampleValid(x, y, vSpacePos[0]) && isUserSampleValid(x + 1, y, vSpacePos[1]) && isUserSampleValid(x, y + 1, vSpacePos[2]) && isUserSampleValid(x + 1, y + 1, vSpacePos[3])) {

				if (isSpacePointsClose(vSpacePos, 0.1f)) {
					vertexType[pixIndex] = 3;
					vertexType[pixIndex + 1] = 1;
					vertexType[pixIndex + sampledWidth] = 1;
					vertexType[pixIndex + sampledWidth + 1] = 1;
				}
			}
			pixIndex++;
		}
		rowIndex += sampledWidth;
	}

	// estimate counts
	vCount = 0;
	tCount = 0;

	for (int i = 0; i < vertexType.size(); i++) {
		if (vertexType[i] != 0) {
			vertexIndex[i] = vCount;
			vCount++;
		}
		else
		{
			vertexIndex[i] = 0;
		}

		if (vertexType[i] == 3)
		{
			tCount++;
		}
	}
}

bool ncKinectPCTriangulator::isUserSampleValid(int x, int y, glm::vec3 & vSpacePos) {
	int startIndex = y * sampleSize * depthWidth + x * sampleSize;

	int pixelIndex = startIndex;

	vSpacePos = pointcloud->getVertex(pixelIndex);

	if (silhouettespixels[pixelIndex] != 0 && !isinf(vSpacePos.x) && !isinf(vSpacePos.y) && !isinf(vSpacePos.z)) {
		return true;
	}

	pixelIndex++;

	startIndex += depthWidth;

	return false;

}

bool ncKinectPCTriangulator::isSpacePointsClose(vector < glm::vec3 > vSpacePos, float fMinDistSquared) {
	int iPosLength = vSpacePos.size();

	for (int i = 0; i < iPosLength; i++)
	{
		for (int j = i + 1; j < iPosLength; j++)
		{
			glm::vec3 vDist = vSpacePos[j] - vSpacePos[i];

			float sqrMag = vDist.x * vDist.x + vDist.y * vDist.y + vDist.z * vDist.z;

			if (sqrMag > fMinDistSquared) {
				return false;
			}
		}
	}
	return true;
}

//--------------------------------------------------------------
void ncKinectPCTriangulator::drawGridOneColor(float stepSize, size_t numberOfSteps, bool labels, bool x, bool y, bool z) {

	if (x) {
		ofDrawGridPlane(stepSize, numberOfSteps, labels);
	}
	if (y) {
		ofMatrix4x4 m;
		m.makeRotationMatrix(90, 0, 0, -1);
		ofPushMatrix();
		ofMultMatrix(m);
		ofDrawGridPlane(stepSize, numberOfSteps, labels);
		ofPopMatrix();
	}
	if (z) {
		ofMatrix4x4 m;
		m.makeRotationMatrix(90, 0, 1, 0);
		ofPushMatrix();
		ofMultMatrix(m);
		ofDrawGridPlane(stepSize, numberOfSteps, labels);
		ofPopMatrix();
	}

	if (labels) {
		float labelPos = stepSize * (numberOfSteps + 0.5);
		ofDrawBitmapString("x", labelPos, 0, 0);
		ofDrawBitmapString("y", 0, labelPos, 0);
		ofDrawBitmapString("z", 0, 0, labelPos);
	}
}
