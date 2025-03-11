#version 330 core
layout(location = 0 ) in vec4 vertex; //vec2 position, vec2 texcoords

out vec2 TexCoords;

uniform bool chaos;
uniform bool confuse;
uniform bool shake;
uniform float time;

void main()
{
gl_Position = vec4(vertex.xy, 0.0f, 1.0f);
vec2 texture = vertex.zw; //координаты текстур
if(chaos){
	float strength = 0.3;
	vec2 pos = vec2(texture.x + sin(time) * strength, texture.y + cos(time) * strength); //придаем им движение 
	TexCoords = pos;

}else if (confuse){  //инвертируем координаты
	TexCoords = vec2(1.0f - texture.x, 1.0 - texture.y); // текстуры хранятся от 0 до 1, мы так инвертируем их  (0:1) -> ( -1; 0)

}else // передаем дефолтные координаты
{
	TexCoords = texture;

}
if(shake)
{
float strength = 0.01;
gl_Position.x += cos(time * 10) * strength;  //cos 0 = 1 -> -1 sin 0 0 = 1 -> -1
gl_Position.y += cos(time * 15) * strength;
}


}