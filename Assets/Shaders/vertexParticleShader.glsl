#version 330 core
layout	(location = 0) in vec4 vertex; // <vec2 positionm vec2 texcoord

out vec2 Texcoord;
out vec4 ParticleColor;

uniform mat4 projection;
uniform vec2 offset;
uniform vec4 color;

void main(){

float scale = 10.0f;

Texcoord = vertex.zw;  // не изменяем текстурные координаты
ParticleColor = color; // не изменяем цвет 
gl_Position = projection * vec4((vertex.xy * scale) + offset, 0.0f,1.0); // скейлим обьект в 10 раз и добавляем оффсет 


}
