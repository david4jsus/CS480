#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
using namespace std;

#include "graphics_headers.h"
#include "camera.h"
#include "shader.h"
#include "object.h"
#include "sound.h"
#include <btBulletDynamicsCommon.h>
#include "objectInfo.h"

class Object;

class Graphics
{
  public:
    Graphics();
    Graphics(string vLightingVertFilePath, string vLightingFragFilePath, string fLightingVertFilePath, string fLightingFragFilePath, glm::vec3 storedEngineStartingCameraPos, 
             float storedEngineYaw, float storedEnginePitch, std::vector<ObjectInfo> allObjectsInfo);
    ~Graphics();
    bool Initialize(int width, int height, std::string file);
    void Update(unsigned int dt);
    void Render();
    Camera* GetCamera();
    void SwitchShaders();
    void SetVertexShader();
    void SetFragmentShader();
    
    // To get a specified object, specified by array index
    Object* GetObject(int index);
    
	int GetNumberOfObjects();
	 
	// Bullet 
	btDiscreteDynamicsWorld* GetDynamicsWorld();
	 
	float GetAmbientLightingScale();
	float SetAmbientLightingScale(float setAmbientLighting);
	 
	float GetSpecularScale();
	float SetSpecularScale(float setSpecularScale);
	 
	btRigidBody* GetRigidBody(int objectIndex);
	
	void setCameraStartingPos();

  private:
    std::string ErrorString(GLenum error);

    Camera *m_camera;
    Shader *m_PerVertexShader;
    Shader *m_PerFragmentShader;

    // Per vertex
    string vLightingVertexShaderFilePath;
    string vLightingFragmentShaderFilePath;
    GLint m_vprojectionMatrix;
    GLint m_vviewMatrix;
    GLint m_vmodelMatrix;
    GLint m_vlightPos;
    GLint m_vambientColor;
    GLint m_vdiffuseColor;
    GLint m_vspecularColor;
    GLint m_vshininess;
    
    // Per fragment
    string fLightingVertexShaderFilePath;
    string fLightingFragmentShaderFilePath;
    GLint m_fprojectionMatrix;
    GLint m_fviewMatrix;
    GLint m_fmodelMatrix;
    GLint m_flightPos;
    GLint m_fambientColor;
    GLint m_fdiffuseColor;
    GLint m_fspecularColor;
    GLint m_fshininess;

    std::vector<Object*> m_objects;

    Sound gameSound;
    
    // Bullet Members
    btBroadphaseInterface *broadphase;
    btDefaultCollisionConfiguration *collisionConfig;
    btCollisionDispatcher *dispatcher;
    btSequentialImpulseConstraintSolver *solver;
    btDiscreteDynamicsWorld *dynamicsWorld;
    
    // Shader switching
    bool shaderToggle;
    
    // ambient lighting values
    float ambientLightingScale;
    float specularScale;
    
    std::vector<btRigidBody*> rigidBodies;
    
    bool meshLoaded;
    
    // camera starting position
    glm::vec3 storedGraphicsStartingCameraPos;
    
    // camera starting orientation
    float storedGraphicsYaw;
    float storedGraphicsPitch;
    
    std::vector<ObjectInfo> objectsInfo;
};

#endif /* GRAPHICS_H */
