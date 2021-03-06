#include "object.h"
#include <iostream>
#include <fstream>

using namespace std;

Object::Object()
{
  // To reverse direction of rotation based on keyboard input
  directionReversed = false;
  // To rotate in place
  rotating = true;
  // To orbit around
  orbiting = true;

  angle = 0.0f;
  rotAngle = 0.0f;
  
  objName = "";
  parent = 0;
  orbitRadius = 5.0f;
  orbitSpeedMultiplier = 1.0f;
  rotateSpeedMultiplier = 1.0f;
  rsm = rotateSpeedMultiplier;
  osm = orbitSpeedMultiplier;
  size = 1;
  m_mass = 0;
}

Object::Object(Graphics* graphicsObject, std::string filePath, Object* objParent, float objOrbitRadius, float objOrbitMultiplier,
  float objRotateMultiplier, float objSize, float mass, int whichModel): Object()
{
  // local variables
  //objName.erase(objName.end()-4, objName.end());

  m_graphics = graphicsObject;
  objFilePath = filePath;
  parent = objParent;
  orbitRadius = objOrbitRadius;
  orbitSpeedMultiplier = objOrbitMultiplier;
  rotateSpeedMultiplier = objRotateMultiplier;
  size = objSize;
  m_mass = mass;
  modelNum = whichModel;
  
  // create object
  createObject();
}

Object::~Object()
{
  Vertices.clear();
  Indices.clear();
}

void Object::createObject()
{
  
  correctModelLoad = loadOBJ(objFilePath, myVertices, myIndices);
  
  std::cout << "Loading " << objFilePath << "..." << std::endl;
  
  glGenBuffers(1, &VB);
  glBindBuffer(GL_ARRAY_BUFFER, VB);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * myVertices.size(), &myVertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &IB);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * myIndices.size(),
    &myIndices[0], GL_STATIC_DRAW);
  
  // Collider
  /*colliderShape = new btBvhTriangleMeshShape(objTriMesh, true);
  btDefaultMotionState *shapeMotionState = new btDefaultMotionState();
  btScalar mass(m_mass);
  cout << m_mass << "||" << endl;
  btVector3 inertia(0, 0, 0);
  colliderShape->calculateLocalInertia(mass, inertia);
  btRigidBody::btRigidBodyConstructionInfo shapeRigidBodyCI(mass, shapeMotionState, colliderShape, inertia);
  rigidBody = new btRigidBody(shapeRigidBodyCI);
  m_graphics->GetDynamicsWorld()->addRigidBody(rigidBody);*/
  
  // motion state 
  btDefaultMotionState *shapeMotionState;
  
  // bottom of the board
  if(modelNum == 0)
  {
    // create a plane collider
      // btVector3(0, 1, 0) means normal is facing upward so the plane is on the bottom
      // Note: plane colliders extend out infinitely
    btVector3 planeNormal = btVector3(0, 1, 0);
    btScalar planeConstant = 0.0; 
    colliderShape = new btStaticPlaneShape(planeNormal, planeConstant);
    
    // set orientation and position of object
    shapeMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
  }
  
  // pinball
  else if(modelNum == 1)
  {
    // create a sphere collider
    btScalar radius = 1.0;
    colliderShape = new btSphereShape(radius);
    
    // set orientation and position of object
    shapeMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
  }
  
  // cube
  else if(modelNum == 2)
  {
    // create a box collider
    btVector3 boxHalfExtents = btVector3(2.0, 2.0, 2.0);
    colliderShape = new btBoxShape(boxHalfExtents);
    
    // set orientation and position of object
    shapeMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
  }
  
  // cylinder
  else if(modelNum == 3)
  {
    // create sphere collider
    btScalar radius = 5.0;
    colliderShape = new btSphereShape(radius);
    
    // set orientation and position of object
    shapeMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(40, 0, 0)));
  }
  
  // wall on far end of board
  else if(modelNum == 4)
  {
    // create a plane collider
      // btVector3(0, 0, -1) means normal is facing outwards out of the screen
      // Note: positive on the z-axis means towards the user out of the screen
    btVector3 planeNormal = btVector3(0, 0, 1);
    btScalar planeConstant = 0.0; 
    colliderShape = new btStaticPlaneShape(planeNormal, planeConstant);
    
    // set orientation and position of object
    shapeMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, -45)));
  }
  
  // wall which is close to the viewer
  else if(modelNum == 5)
  {
    // create a plane collider
      // btVector3(0, 0, -1) means normal is facing inwards into the screen
    btVector3 planeNormal = btVector3(0, 0, -1);
    btScalar planeConstant = 0.0; 
    colliderShape = new btStaticPlaneShape(planeNormal, planeConstant);
    
    // set orientation and position of object
    shapeMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 45)));
  }
  
  // left wall for board
  else if(modelNum == 6)
  {
    // create a plane collider
      // btVector3(1, 0, 0) means normal is facing towards the right
    btVector3 planeNormal = btVector3(1, 0, 0);
    btScalar planeConstant = 0.0; 
    colliderShape = new btStaticPlaneShape(planeNormal, planeConstant);
    
    // set orientation and position of object
    shapeMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-45, 0, 0)));
  }
  
  // right wall for board
  else if(modelNum == 7)
  {
    // create a plane collider
      // btVector3(-1, 0, 0) means normal is facing towards the left 
    btVector3 planeNormal = btVector3(-1, 0, 0);
    btScalar planeConstant = 0.0; 
    colliderShape = new btStaticPlaneShape(planeNormal, planeConstant);
    
    // set orientation and position of object
    shapeMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(45, 0, 0)));
  }
  
  // set mass and inertia
  btScalar mass(m_mass);
  btVector3 inertia(0, 0, 0);
  
  // calculate inertia
  colliderShape->calculateLocalInertia(mass, inertia);
  
  // brind together all data needed to create a rigidbody
  btRigidBody::btRigidBodyConstructionInfo shapeRigidBodyCI(mass, shapeMotionState, colliderShape, inertia);
  
  // create rigidbody
  rigidBody = new btRigidBody(shapeRigidBodyCI);
  
  // set bounciness of rigidbody
  rigidBody->setRestitution(1.0);
  
  if(modelNum == 2)
  {
   rigidBody->setRestitution(0.0);
  }
  
  // add rigidbody to world
  m_graphics->GetDynamicsWorld()->addRigidBody(rigidBody);
  
  // disable the deactivation of the rigidbody
  rigidBody->setActivationState(DISABLE_DEACTIVATION);
}

void Object::Update(unsigned int dt)
{
  // Determine center
  glm::mat4 center;

  // Set position
  model = glm::translate(center, pos);
  
  // Pass this to any children objects
  modelForChild = model;
  
  // Physics
  btTransform trans;
  btScalar m[16];
  m_graphics->GetDynamicsWorld()->stepSimulation(dt, 10);
  rigidBody->getMotionState()->getWorldTransform(trans);
  trans.getOpenGLMatrix(m);
  model = glm::make_mat4(m);
  
  // Scaling
  model = glm::scale(model, glm::vec3(size, size, size));
}

glm::mat4 Object::GetModel()
{
  return model;
}

glm::mat4 Object::GetModelForChild()
{
  return modelForChild;
}

glm::vec3 Object::objectPosition()
{
   glm::vec3 objPos = pos;
   
   if (parent != 0)
   {
      objPos += parent->objectPosition();
   }
   
   return objPos;
}

void Object::setPosition(glm::vec3 newPos)
{
   pos = newPos;
}

std::string Object::GetObjectName()
{
   return objName;
}

btCollisionShape* Object::GetCollisionShape()
{
   return colliderShape;
}

void Object::UpdateRotationSpeed(float rotateMultiplier)
{  
  // Set the new speed with the multiplier
  float defaultSpeed;

  // Get the speed that was read from the file
  defaultSpeed = GetRotationSpeed();
  //cout << "Read In Speed: " << defaultSpeed << endl;
 
  defaultSpeed *= rotateMultiplier;
  //cout << "New Speed: " << defaultSpeed << endl;
  
  rotateSpeedMultiplier = defaultSpeed;
}

void Object::UpdateOrbitSpeed(float orbitMultiplier)
{
  // Set the new speed with the multiplier
  float defaultSpeed;

  // Get the speed that was read from the file
  defaultSpeed = GetOrbitSpeed();
  //cout << "Read In Speed: " << defaultSpeed << endl;
 
  defaultSpeed *= orbitMultiplier;
  //cout << "New Speed: " << defaultSpeed << endl;
  
  orbitSpeedMultiplier = defaultSpeed;
}

float Object::GetRotationSpeed()
{
 return rsm;
}

float Object::GetOrbitSpeed()
{
 return osm;
}

void Object::Render()
{
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, VB);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,texture));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

  if (correctModelLoad)
  {
    // Bind Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    
    // Draw
    glDrawElements(GL_TRIANGLES, myIndices.size(), GL_UNSIGNED_INT, 0);
  }
  else
  {
    // Bind Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    
    // Draw
    glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
  }

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
}

bool Object::loadOBJ(std::string path, std::vector<Vertex> &out_vertices,
  std::vector<unsigned int> &out_indices)
{
  // local variables
  unsigned int meshCounter, faceLooper, verticesLooper, indicesLooper;
  unsigned int lastValue = 0;
  glm::vec3 vertex;
  glm::vec3 color;
  glm::vec2 texture;
  btVector3 triArray[3];
  objTriMesh = new btTriangleMesh();

  // string that contains path to object file
  std::string completeFilePath = "../assets/models/" + path;

  // access information from object file
  scene = importer.ReadFile(completeFilePath, aiProcess_Triangulate);

  // check if object file was sucessfully accessed
  if(scene == NULL)
  {
    std::cout << "ERROR: Unable to open file!" << std::endl;
    return false;
  }

  // Get all meshes from object scene
  for(meshCounter = 0; meshCounter < scene->mNumMeshes; meshCounter++)
  {
    meshes.push_back(scene->mMeshes[meshCounter]);
        
    // Check if the model has a texture
    meshes[meshCounter]->HasTextureCoords(0);
    //cout << "has texture" << endl;
    
     // Loop through all faces
	  for(faceLooper = 0; faceLooper < meshes[meshCounter]->mNumFaces; faceLooper++)
	  {
		  // Loop through all indices
	    for(indicesLooper = 0; indicesLooper < 3; indicesLooper++)
		  {
        // Get position of index
		    out_indices.push_back(meshes[meshCounter]->mFaces[faceLooper].mIndices[indicesLooper] + lastValue);
		  
		    // Add face to collider
		    //aiVector3D position = meshes[meshCounter]->mVertices[out_indices.back()];
		    //triArray[indicesLooper] = btVector3(position.x, position.y, position.z);
		  }
		
		//objTriMesh->addTriangle(triArray[0], triArray[1], triArray[2]);
	  }

      // offest new next mesh's index poisition
	  lastValue = out_indices[out_indices.size() - 1] + 1;
			  
	  // loop through all vertexes
	  for(verticesLooper = 0; verticesLooper < meshes[meshCounter]->mNumVertices; verticesLooper++)
		{
		  // Texture coordinates
      aiVector3D vert = meshes[meshCounter]->mTextureCoords[0][verticesLooper];
      texture.x = vert.x;
      texture.y = vert.y * -1;
		
		  // get x, y, and z coordinates for each vertex
		  vertex.x = meshes[meshCounter]->mVertices[verticesLooper].x;
		  vertex.y = meshes[meshCounter]->mVertices[verticesLooper].y;
		  vertex.z = meshes[meshCounter]->mVertices[verticesLooper].z;

		  // assign color to a vertex
		  color.x = glm::sin(vertex.x);
		  color.y = glm::sin(vertex.y);
		  color.z = glm::sin(vertex.z);

      // store vertexes
		  Vertex batmanVertices(vertex, color, texture);
		  out_vertices.push_back(batmanVertices);
		}
		
	    // Read texture file
	    aiString assimpFilePath;
	    string imageFilePath;
	    scene->mMaterials[scene->mMeshes[meshCounter]->mMaterialIndex]->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), assimpFilePath);
	    imageFilePath = assimpFilePath.C_Str();
	    imageFilePath = "../assets/images/" + imageFilePath;
		
		  // Load Texture
      Magick::Blob blob;
      Magick::Image *image;
      image = new Magick::Image(imageFilePath);
      image->write(&blob, "RGBA");
      //cout << "Loaded Texture: " << assimpFilePath.C_Str() << endl;

      // Generate Texture
      glGenTextures(1, &Texture);
      glBindTexture(GL_TEXTURE_2D, Texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->columns(), image->rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, blob.data());
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      delete image;
      //cout << "Generated Texture" << endl;
	  }

  // object file sucessfully accessed
  return true;
}

btRigidBody* Object::GetRigidBody()
{
  return rigidBody;
}







