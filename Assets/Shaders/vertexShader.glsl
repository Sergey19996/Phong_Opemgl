#version 330 core
layout (location = 0) in vec4 vertex;  //vec2 pos, vec2 texture

out vec2 Texcoords;
uniform mat4 model;
uniform mat4 projection;

void main(){
Texcoords = vertex.zw; // в координаты распаковываем 
gl_Position = projection * model * vec4(vertex.xy,0.0,1.0); // для 2д не надо матрица view так как нет глубины

}
