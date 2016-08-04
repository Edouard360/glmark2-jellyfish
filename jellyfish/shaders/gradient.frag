#version 330 core

#ifdef GL_ES
precision highp float;
#endif


uniform vec3 color1;
uniform vec3 color2;

in vec2 uv;

out vec4 color;

void main()
{
    color = vec4(mix(color1, color2, uv.x * uv.y),1.0);
}
