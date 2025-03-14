#version 330 core
layout	(location = 0) in vec4 vertex; // <vec2 positionm vec2 texcoord

out vec2 Texcoord;
out vec4 ParticleColor;

uniform mat4 projection;
uniform vec2 offset;
uniform vec4 color;

void main(){

float scale = 10.0f;

Texcoord = vertex.zw;  // �� �������� ���������� ����������
ParticleColor = color; // �� �������� ���� 
gl_Position = projection * vec4((vertex.xy * scale) + offset, 0.0f,1.0); // ������� ������ � 10 ��� � ��������� ������ 


}
