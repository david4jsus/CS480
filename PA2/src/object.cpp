#include "object.h"

Object::Object()
{  
  /*
    # Blender File for a Cube
    o Cube
    v 1.000000 -1.000000 -1.000000
    v 1.000000 -1.000000 1.000000
    v -1.000000 -1.000000 1.000000
    v -1.000000 -1.000000 -1.000000
    v 1.000000 1.000000 -0.999999
    v 0.999999 1.000000 1.000001
    v -1.000000 1.000000 1.000000
    v -1.000000 1.000000 -1.000000
    s off
    f 2 3 4
    f 8 7 6
    f 1 5 6
    f 2 6 7
    f 7 8 4
    f 1 4 8
    f 1 2 4
    f 5 8 6
    f 2 1 6
    f 3 2 7
    f 3 7 4
    f 5 1 8
  */

  Vertices = {
    {{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}},
    {{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    {{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}},
    {{1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}},
    {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
    {{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
    {{-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}}
  };

  Indices = {
    2, 3, 4,
    8, 7, 6,
    1, 5, 6,
    2, 6, 7,
    7, 8, 4,
    1, 4, 8,
    1, 2, 4,
    5, 8, 6,
    2, 1, 6,
    3, 2, 7,
    3, 7, 4,
    5, 1, 8
  };

  // The index works at a 0th index
  for(unsigned int i = 0; i < Indices.size(); i++)
  {
    Indices[i] = Indices[i] - 1;
  }

  angle = 0.0f;
  rotAngle = 0.0f;

  glGenBuffers(1, &VB);
  glBindBuffer(GL_ARRAY_BUFFER, VB);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &IB);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
  
  // To reverse direction of rotation based on keyboard input
  directionReversed = false;
  // To rotate in place
  rotating = true;
  // To orbit around
  orbiting = true;
  
}

Object::~Object()
{
  Vertices.clear();
  Indices.clear();
}

void Object::Update(unsigned int dt)
{
  // == Reverse direction and/or stop rotations based on keyboard input == //
  
  // Reverse direction of rotation based on keyboard input
  if (directionReversed)
  {
    angle -= dt * M_PI/1000;
  }
  else
  {
    angle += dt * M_PI/1000;
  }

  // Orbit rotation (first rotation by which translation is affected)
  if (orbiting)
  {
    model = glm::rotate(glm::mat4(1.0f), (angle/3), glm::vec3(0.0, 1.0, 0.0));

    // Move to the side
    model = glm::translate(model, glm::vec3(5.0, 0.0, 0.0));
  }
  else
  {
    model = glm::mat4(1.0f);
  }  
  
  // Self-centered rotation
  if (rotating)
  {
    if (directionReversed)
    {
      rotAngle -= dt * M_PI/1000;
    }
    else
    {
      rotAngle += dt * M_PI/1000;
    }
  }
  model = glm::rotate(model, (rotAngle), glm::vec3(0.0, 1.0, 0.0));
  
  // ===================================================================== //
}

glm::mat4 Object::GetModel()
{
  return model;
}

// To reverse the direction of rotation based on keyboard input
void Object::reverseDirection()
{
  directionReversed = !directionReversed;
}

// Toggle cube rotating (self-centered)
void Object::toggleRotation()
{
  rotating = !rotating;
}

// Toggle cube orbiting
void Object::toggleOrbit()
{
  orbiting = !orbiting;
}

void Object::Render()
{
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, VB);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

  glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}
