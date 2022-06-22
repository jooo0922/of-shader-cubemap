#pragma once

#include "ofMain.h"
#include "ofxEasyCubemap.hpp"

// 카메라의 현재 위치 및 fov(시야각)값을 받는 구조체 타입 지정. (구조체 타입은 ts interface 랑 비슷한 개념이라고 생각하면 될 것 같음.)
struct CameraData {
    glm::vec3 pos;
    float fov;
};

struct DirectionalLight {
    glm::vec3 direction; // 조명의 방향 (이 값을 c++ 에서 뒤집고, 정규화하여 셰이더에서 조명벡터로 사용할거임)
    glm::vec3 color; // 조명의 색상 (셰이더에서 계산된 노말벡터와 조명벡터의 내적값(= 디퓨즈 라이팅 값)과 곱해줄거임)
    float intensity; // 조명의 강도 (c++ 에서 조명의 색상과 곱해줘서 조명색의 밝기를 결정함.)
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    // ofApp.cpp 에서 큐브메쉬를 그리는 함수를 따로 추출하기 위해 선언한 메서드.
    void drawCube(glm::mat4& proj, glm::mat4& view);
    
    ofShader cubemapShader; // cube.ply 에 큐브맵 텍스쳐를 적용하기 위해 사용할 셰이더 객체 변수 선언
    ofxEasyCubemap cubemap; // 오픈프레임웍스는 큐브맵을 지원하지 않으므로, 큐브맵 로드 및 유니폼 변수 전송에 필요한 커스텀 클래스 객체 변수 선언
    ofMesh cubeMesh; // cube.ply 모델링 파일을 로드해서 사용할 메쉬 객체 변수 선언
    
    CameraData cam; // 카메라 위치 및 fov(시야각)의 현재 상태값을 나타내는 구조체를 타입으로 갖는 멤버변수 cam 선언
};
