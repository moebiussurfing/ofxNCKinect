
#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxGuiPlus\ofxSliderPlus.h"
#include "ncKinectUser.h"

class ncKinectInteractionSpace :public ofNode {

public:

	ofMesh mesh;

	ofMesh userselectmesh;

	ofVec3f uppercorner;
	ofVec3f lowercorner;

	bool isPointInsideInteractionSpace(ofVec3f &_point) {

		ofVec3f newlower = lowercorner + getPosition();
		ofVec3f newhigher = uppercorner + getPosition();

		if (_point.x > newlower.x && _point.x < newhigher.x &&
			_point.y > newlower.y && _point.y <newhigher.y &&
			_point.z > newlower.z && _point.z <newhigher.z) {
			return true;
		}

		//If not, then return false
		return false;

	}


	void create(ofVec2f _widthdepth) {

		mesh.clear();
		ofBoxPrimitive space;
		space.set(_widthdepth.x , 3, _widthdepth.y);
		
			ofMesh _oldmesh = space.getMesh();
			for (auto &it : _oldmesh.getColors()) {
				mesh.addColor(it);
			}

			for (auto &it : _oldmesh.getVertices()) {

				ofVec3f oldvertex = it;
				mesh.addVertex(ofVec3f(it.x, it.y + (space.getHeight() / 2.0f), it.z + -(space.getDepth() / 2.0f)));
			}

			for (auto &it : _oldmesh.getNormals()) {
				mesh.addNormal(it);
			}

			for (auto &it : _oldmesh.getTexCoords()) {
				mesh.addTexCoord(it);
			}

			for (auto &it : _oldmesh.getIndices()) {
				mesh.addIndex(it);
			}


			float distanceabovezero = 0;
			float distanceunderzero = 0;

			for (int i = 0; i< mesh.getNumVertices(); i++) {


				ofVec3f vertex = mesh.getVertex(i);

				float distance = (vertex.z) + (vertex.y) + (vertex.x);
				//check higestdistance above 0
				if (distance > distanceabovezero) {
					uppercorner = vertex;
					distanceabovezero = distance;
				}
				//check higestdistance below 0
				if (distance < distanceunderzero) {
					lowercorner = vertex;
					distanceunderzero = distance;
				}
			}
	
	}

	void draw() {
		transformGL();
		mesh.drawWireframe();
		restoreTransformGL();
	}
	
};


class ncKinectDepthFrustrum :public ofNode {

public:

	ofPlanePrimitive plane;
	ofMesh mesh;
	void create(float fov, float aspectratio, float throwdistance) {

		mesh.clear();

		float frustumHeight = 2.0f * throwdistance* tan(fov * 0.5f * (PI / 180));
		float frustrumWidth = frustumHeight * (aspectratio);

		plane.set(frustrumWidth, frustumHeight);
		plane.setPosition(0, 0, -throwdistance);


		ofVec3f v0 = ofVec3f(0, 0, 0);
		ofVec3f v1 = ofVec3f(plane.getWidth() / 2.0f, plane.getHeight() / 2.0f, -throwdistance);
		ofVec3f v2 = ofVec3f(-plane.getWidth() / 2.0f, plane.getHeight() / 2.0f, -throwdistance);
		ofVec3f v3 = ofVec3f(plane.getWidth() / 2.0f, -plane.getHeight() / 2.0f, -throwdistance);
		ofVec3f v4 = ofVec3f(-plane.getWidth() / 2.0f, -plane.getHeight() / 2.0f, -throwdistance);

		ofFloatColor color = ofFloatColor(1.0,0.0,0.0,1);
		vector<ofVec3f> vertices;
		vector<ofFloatColor> colors;
		vertices.push_back(v0);
		colors.push_back(color);
		vertices.push_back(v1);
		colors.push_back(color);
		vertices.push_back(v2);
		colors.push_back(color);
		vertices.push_back(v3);
		colors.push_back(color);
		vertices.push_back(v4);
		colors.push_back(color);
		vertices.push_back(v0);
		colors.push_back(color);
		vertices.push_back(v1);
		colors.push_back(color);
		vertices.push_back(v2);
		colors.push_back(color);
		vertices.push_back(v3);
		colors.push_back(color);
		vertices.push_back(v4);
		colors.push_back(color);

		mesh.addVertices(vertices);
		mesh.addColors(colors);

		mesh.addTriangle(0, 1, 2);
		mesh.addTriangle(0, 1, 3);
		mesh.addTriangle(0, 2, 4);
		mesh.addTriangle(0, 3, 4);
		mesh.addTriangle(1, 2, 4);
		mesh.addTriangle(4, 1, 3);

		mesh.addTriangle(0, 1, 2);
		mesh.addTriangle(0, 1, 3);
		mesh.addTriangle(0, 2, 4);
		mesh.addTriangle(0, 3, 4);
		mesh.addTriangle(1, 2, 4);
		mesh.addTriangle(4, 1, 3);


		ofVec3f n0 = ofVec3f(0, 0, 1);
		ofVec3f n1 = ofVec3f(0, 0, 1);
		ofVec3f n2 = ofVec3f(0, 0, 1);
		ofVec3f n3 = ofVec3f(0, 0, 1);
		ofVec3f n4 = ofVec3f(0, 0, 1);

		ofVec3f n5 = ofVec3f(0, 0, -1);
		ofVec3f n6 = ofVec3f(0, 0, -1);
		ofVec3f n7 = ofVec3f(0, 0, -1);
		ofVec3f n8 = ofVec3f(0, 0, -1);
		ofVec3f n9 = ofVec3f(0, 0, -1);


		vector<ofVec3f> normals;
		normals.push_back(n0);
		normals.push_back(n1);
		normals.push_back(n2);
		normals.push_back(n3);
		normals.push_back(n4);
		normals.push_back(n5);
		normals.push_back(n6);
		normals.push_back(n7);
		normals.push_back(n8);
		normals.push_back(n9);

		mesh.addNormals(normals);

	}

	void draw() {
		transformGL();
		mesh.drawWireframe();
		restoreTransformGL();
		
	}
};


class ncKinectAreaManager {

private:
	//GUI
	ofxPanel gui;
	ofParameter <float> kinectspacedepth;
	ofParameter <float> interactionspacewidth;
	ofParameter <float> interactionspacedepth;
	ofParameter <bool> bDrawDepthFrustrum;
	ofParameter <bool> bDrawInteractionSpace;

	void setupGUI();

	void setKinectFrustrum();
	void setInteractionSpace();

	ncKinectDepthFrustrum kinectdepthfrustrum;
	ncKinectInteractionSpace interactionspace;

	void kinectSpaceDepthChanged(float &arg);
	void interactionSpaceDepthChanged(float &arg);
	void interactionSpaceWidthChanged(float &arg);

	ofPlanePrimitive floorplane;

	int loadcounter;

public:
	~ncKinectAreaManager();
	ncKinectAreaManager();

	
	void setup();
	void update();
	void updatePositionRotation(ofVec3f _pos, ofQuaternion _rot);

	void draw();
	void drawGUI();

	bool isUserInInteractionSpace(ofVec3f _pos);
	bool isUserInInteractionSpace(ncKinectUser _user);
	bool isThereAUserInInteractiveSpace(vector<ncKinectUser> _users);
	vector<ncKinectUser> returnUsersInInteractionspace(vector<ncKinectUser> _users);
	int returnClosestUserInInteractionSpace(vector<ncKinectUser> _users);


};