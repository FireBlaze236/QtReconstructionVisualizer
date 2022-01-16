#version 330

uniform sampler2D mainTexture;

in vec2 texCoord;

out vec4 color;

void main()
{
  color = texture(mainTexture, texCoord) ;
  color.a = 0.5;
}
