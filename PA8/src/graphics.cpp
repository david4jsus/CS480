#include "graphics.h"

Graphics::Graphics()
{
	broadphase = new btDbvtBroadphase();
	collisionConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	solver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
	
	dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
}

Graphics::~Graphics()
{
    if( dynamicsWorld != NULL )
    {
        delete dynamicsWorld;

        dynamicsWorld = NULL;
    }

    if( solver != NULL )
    {
        delete solver;

        solver = NULL;
    }

    if( dispatcher != NULL )
    {
        delete dispatcher;

        dispatcher = NULL;
    }

    if( collisionConfig != NULL )
    {
        delete collisionConfig;

        collisionConfig = NULL;
    }

    if( broadphase != NULL )
    {
        delete broadphase;

        broadphase = NULL;
    }
}

bool Graphics::Initialize(int width, int height, std::string file)
{
  // Used for the linux OS
  #if !defined(__APPLE__) && !defined(MACOSX)
    // cout << glewGetString(GLEW_VERSION) << endl;
    glewExperimental = GL_TRUE;

    auto status = glewInit();

    // This is here to grab the error that comes from glew init.
    // This error is an GL_INVALID_ENUM that has no effects on the performance
    glGetError();

    //Check for error
    if (status != GLEW_OK)
    {
      std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
      return false;
    }
  #endif

  // For OpenGL 3
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Init Camera
  m_camera = new Camera();
  if(!m_camera->Initialize(width, height))
  {
    printf("Camera Failed to Initialize\n");
    return false;
  }

  // Create objects
  //Object* Skybox = new Object(this, "Skybox.obj",      0, 0.0f, 0.0f, 0.0f, 25.0f, 0, 0);
  Object* board  = new Object(this, "Disboard.obj",    0, 0.0f, 0.0f, 0.0f, 1.0f,  0, 0);
  Object* ball   = new Object(this, "awesomeball.obj", 0, 0.0f, 0.0f, 0.0f, 0.01f, 1, 1);
  Object* cube = new Object(this, "cube.obj", 0, 0.0f, 0.0f, 0.0f, 0.5f, 1, 2);
  Object* cylinder = new Object(this, "cylinder.obj", 0, 0.0f, 0.0f, 0.0f, 0.5f, 0, 3);
  Object* backWall   = new Object(this, "awesomeball.obj", 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 4);
  Object* frontWall   = new Object(this, "awesomeball.obj", 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 5);
  Object* leftWall   = new Object(this, "awesomeball.obj", 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 6);
  Object* rightWall   = new Object(this, "awesomeball.obj", 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 7);

  // Waiting Song while the planets load
  gameSound.LoadSound("../assets/NGGUP.wav");
  gameSound.PlaySound();
  
  // Push objects onto list
  //m_cubes.push_back(Skybox);
  m_cubes.push_back(board);
  m_cubes.push_back(ball);
  m_cubes.push_back(cube);
  m_cubes.push_back(cylinder);
  m_cubes.push_back(backWall);
  m_cubes.push_back(frontWall);
  m_cubes.push_back(leftWall);
  m_cubes.push_back(rightWall);
  
  // get rigidbody for the cube
  rigidBody = m_cubes[2]->GetRigidBody();

  // Set up the shaders
  m_shader = new Shader();
  if(!m_shader->Initialize())
  {
    printf("Shader Failed to Initialize\n");
    return false;
  }

  // Add the vertex shader
  if(!m_shader->AddShader(GL_VERTEX_SHADER))
  {
    printf("Vertex Shader failed to Initialize\n");
    return false;
  }

  // Add the fragment shader
  if(!m_shader->AddShader(GL_FRAGMENT_SHADER))
  {
    printf("Fragment Shader failed to Initialize\n");
    return false;
  }

  // Connect the program
  if(!m_shader->Finalize())
  {
    printf("Program to Finalize\n");
    return false;
  }

  // Locate the projection matrix in the shader
  m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
  if (m_projectionMatrix == INVALID_UNIFORM_LOCATION) 
  {
    printf("m_projectionMatrix not found\n");
    return false;
  }

  // Locate the view matrix in the shader
  m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
  if (m_viewMatrix == INVALID_UNIFORM_LOCATION) 
  {
    printf("m_viewMatrix not found\n");
    return false;
  }

  // Locate the model matrix in the shader
  m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");
  if (m_modelMatrix == INVALID_UNIFORM_LOCATION) 
  {
    printf("m_modelMatrix not found\n");
    return false;
  }

  //enable depth testing
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  return true;
}

void Graphics::Update(unsigned int dt)
{
  // Update the objects
  for(unsigned int i = 0; i < m_cubes.size(); i++)
  {
    m_cubes[i]->Update(dt);
  }
}

Camera* Graphics::getCamera()
{
   return m_camera;
}

Object* Graphics::GetObject(int index)
{
  return m_cubes[index];
}

int Graphics::numberOfCubes()
{
  return m_cubes.size();
}

btDiscreteDynamicsWorld* Graphics::GetDynamicsWorld()
{
  return dynamicsWorld;
}

void Graphics::Render()
{
  //clear the screen
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Start the correct program
  m_shader->Enable();

  // Send in the projection and view to the shader
  glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection())); 
  glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView())); 

  // Render the objects
  for(unsigned int i = 0; i < m_cubes.size(); i++)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_cubes[i]->GetModel()));
    m_cubes[i]->Render();
  }

  // Get any errors from OpenGL
  auto error = glGetError();
  if ( error != GL_NO_ERROR )
  {
    string val = ErrorString( error );
    std::cout<< "Error initializing OpenGL! " << error << ", " << val << std::endl;
  }
}

std::string Graphics::ErrorString(GLenum error)
{
  if(error == GL_INVALID_ENUM)
  {
    return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
  }

  else if(error == GL_INVALID_VALUE)
  {
    return "GL_INVALID_VALUE: A numeric argument is out of range.";
  }

  else if(error == GL_INVALID_OPERATION)
  {
    return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
  }

  else if(error == GL_INVALID_FRAMEBUFFER_OPERATION)
  {
    return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
  }

  else if(error == GL_OUT_OF_MEMORY)
  {
    return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
  }
  else
  {
    return "None";
  }
}
