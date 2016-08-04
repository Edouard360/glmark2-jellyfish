#version 330 core

#ifdef GL_ES
precision highp float;
#endif
 

uniform sampler2D uSampler;
uniform sampler2D uSampler1;

uniform float uCurrentTime;

in vec2 vTextureCoord;
in vec4 vWorld;
in vec3 vDiffuse;
in vec3 vAmbient;
in vec3 vFresnel;

out vec4 color;

void main(void)
{
	vec4 caustics = texture(uSampler1, vec2(vWorld.x / 24.0 + uCurrentTime / 20.0, (vWorld.z - vWorld.y)/48.0 + uCurrentTime / 40.0));
    vec4 colorMap = texture(uSampler, vTextureCoord);
    float transparency = colorMap.a + pow(vFresnel.r, 2.0) - 0.3;  //(1.-colorMap.r)*(1.-colorMap.g)*(1.-colorMap.b) smoothstep
    color = vec4((vAmbient + vDiffuse + caustics.rgb) * colorMap.rgb,transparency);
}
