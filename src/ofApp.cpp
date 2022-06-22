#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex(); // 스크린 픽셀 좌표를 사용하는 텍스쳐 관련 오픈프레임웍스 레거시 지원 설정 비활성화. (uv좌표계랑 다르니까!)
    ofEnableDepthTest(); // 깊이테스트를 활성화하여 z버퍼에 저장해서 각 요소에서 카메라와의 거리를 기준으로 앞뒤를 구분하여 렌더링할 수 있도록 함.
    
    // ofApp 맨 처음 설정에서 cubedMesh 를 바라보기 적당한 카메라 위치와 시야각을 지정함.
    cam.pos = glm::vec3(0, 0.75f, 1.0f); // 카메라 위치는 z축으로 1만큼 안쪽으로 들어가게 하고, 조명 연산 결과를 확인하기 위해 y축으로도 살짝 올려줌
    cam.fov = glm::radians(90.0f); // 원근 프러스텀의 시야각은 일반 PC 게임에서는 90도 전후의 값을 사용함. -> 라디안 각도로 변환하는 glm 내장함수 radians() 를 사용함.
    
    cubeMesh.load("cube.ply"); // cubeMesh 메쉬로 사용할 모델링 파일 로드
    cubemapShader.load("cubemap.vert", "cubemap.frag"); // cubeMesh 에 큐브맵 텍스쳐를 적용한 셰이더를 적용하기 위한 셰이더 파일 로드
    
    // 커스텀 큐브맵 클래스를 활용해서 큐브맵 텍스쳐를 로드함. (이때, ofxEasyCubemap.cpp 의 load 함수가 받는 텍스쳐 순서를 잘 보고 전달할 것.)
    cubemap.load("cube_front.jpg", "cube_back.jpg", "cube_right.jpg", "cube_left.jpg", "cube_top.jpg", "cube_bottom.jpg");
}

// cubeMesh 의 각종 변환행렬을 계산한 뒤, 유니폼 변수들을 전송해주면서 드로우콜을 호출하는 함수
void ofApp::drawCube(glm::mat4& proj, glm::mat4& view) {
    using namespace glm; // 이제부터 이 함수블록 내에서 glm 라이브러리에서 꺼내 쓸 함수 및 객체들은 'glm::' 을 생략해서 사용해도 됨.
    
    static float rotAngle = 0.0; // static 을 특정 함수 내에서 사용하는 것을 '정적 지역 변수'라고 하며, 이 할당문은 drawCube() 함수 최초 호출 시 1번만 실행됨.
    rotAngle += 0.2f; // 매 프레임마다 회전각을 0.2도씩 증가시킴으로써 큐브에 회전 애니메이션을 적용하도록 함.
    
    // cubeMesh 의 모델행렬 계산 (회전행렬, 이동행렬, 크기행렬 모두 적용)
    mat4 rotation = rotate(radians(rotAngle), vec3(0, 1, 0)); // cubeMesh 회전행렬은 y축 기준으로 매 프레임마다 증가하는 rotAngle 만큼 회전시킴.
    mat4 s = scale(vec3(0.4, 0.4, 0.4)); // cubeMesh 의 크기행렬은 x, y, z축 모두를 기준으로 0.4배로 축소시킴
    mat4 model = translate(vec3(0.0, 0.75, 0.0f)) * rotation * s; // y축으로 올려주는 이동행렬을 만든 뒤, 회전행렬 및 크기행렬과 곱해서 모델행렬을 만듦.
    // 열 우선 행렬이므로, 원하는 행렬 곱셈과 반대순서인 이동행렬 * 회전행렬 * 크기행렬 순으로 곱해줌
    
    // 최적화를 위해 c++ 단에서 투영 * 뷰 * 모델행렬을 한꺼번에 곱해서 버텍스 셰이더에 전송함.
    mat4 mvp = proj * view * model; // 열 우선 행렬이라 원래의 곱셈 순서인 '모델 -> 뷰 -> 투영'의 반대 순서로 곱해줘야 함.
    
    ofShader& shd = cubemapShader; // 참조자 shd 는 ofShader 타입의 멤버변수 waterShader 를 참조하도록 함.
    
    // shd(cubemapShader 를 참조) 를 바인딩하여 사용 시작
    shd.begin();
    shd.setUniformMatrix4f("mvp", mvp); // 위에서 한꺼번에 합쳐준 mvp 행렬을 버텍스 셰이더 유니폼 변수로 전송
    shd.setUniformTexture("envMap", cubemap.getTexture(), 0); // 커스텀 큐브맵 클래스로 셰이더의 유니폼 변수에 큐브맵 텍스쳐를 전송할 경우, 명시적으로 getTexture() 를 호출해야 함.
    
    cubeMesh.draw(); // cubeMesh 메쉬 드로우콜 호출하여 그림.
    
    shd.end();
    // shd(cubemapShader) 사용 중단
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    using namespace glm; // 이제부터 현재 블록 내에서 glm 라이브러리에서 꺼내 쓸 함수 및 객체들은 'glm::' 을 생략해서 사용해도 됨.
    
    // 조명구조체 dirLight 에 조명데이터를 할당해 줌.
    DirectionalLight dirLight; // 조명데이터 구조체인 DirectionLight 타입의 객체 변수 dirLight 선언
    dirLight.direction = glm::normalize(glm::vec3(0.5, -1, -1)); // 조명벡터 방향 지정
    dirLight.color = glm::vec3(1, 1, 1); // 조명색상은 흰색으로 지정
    dirLight.intensity = 1.0f; // 조명강도도 1로 지정. 참고로, 1보다 큰값으로 조명강도를 조명색상에 곱해줘봤자, 프래그먼트 셰이더는 (1, 1, 1, 1) 이상의 색상값을 처리할 수 없음.
    
    // 물 셰이더에서 스펙큘러를 더 잘 보이게 하기 위해, 조명방향만 변경해 준 조명데이터를 따로 만듦.
    DirectionalLight waterLight;
    waterLight.direction = glm::normalize(glm::vec3(0.5, -1, 1)); // 조명벡터 방향만 바꿈
    waterLight.color = vec3(1, 1, 1);
    waterLight.intensity = 1.0f;
    
    // 투영행렬 계산
    float aspect = 1024.0f / 768.0f; // main.cpp 에서 정의한 윈도우 실행창 사이즈를 기준으로 원근투영행렬의 종횡비(aspect)값을 계산함.
    mat4 proj = perspective(cam.fov, aspect, 0.01f, 10.0f); // glm::perspective() 내장함수를 사용해 원근투영행렬 계산.
    
    // 카메라 변환시키는 뷰행렬 계산. 이동행렬만 적용
    mat4 view = inverse(translate(cam.pos)); // 뷰행렬은 카메라 움직임에 반대방향으로 나머지 대상들을 움직이는 변환행렬이므로, glm::inverse() 내장함수로 역행렬을 구해야 함.

    // cubeMesh 메쉬 드로우 함수를 추출하여 정의한 뒤 호출함.
    drawCube(proj, view);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
