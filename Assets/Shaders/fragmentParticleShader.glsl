#version 330 core
in vec2 Texcoord;
in vec4 ParticleColor;
out vec4 color;

uniform sampler2D sprite;

void main()
{
color = (texture(sprite,Texcoord) * ParticleColor); // Берём цвет с текстуры и умножаем на цвет партикла 

}
