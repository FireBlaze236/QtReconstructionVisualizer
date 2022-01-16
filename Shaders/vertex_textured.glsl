#version 330

layout (location = 0) in vec3 layoutPos;
layout (location = 1) in vec2 layoutTexCoord;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec2 texCoord;
void main()
{
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(layoutPos, 1.0);
  texCoord = layoutTexCoord;
}
