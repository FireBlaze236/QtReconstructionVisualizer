#version 330

in vec3 vertexColor;
uniform vec3 inputColor;
out vec4 color;

void main()
{
  color = vec4(inputColor, 1.0);
}
