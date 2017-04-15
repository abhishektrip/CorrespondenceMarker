#version 330 core
in vec2 UV;
out vec3 color;
in vec3 colorValue;
uniform sampler2D myTextureSampler;
void main(){
  color = texture( myTextureSampler, UV ).rgb;
}

