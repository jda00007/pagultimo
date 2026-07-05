#version 410 core
layout (location = 0) in vec3 posicion;
uniform mat4 matrizModVisProy;
void main() {
    gl_Position = matrizModVisProy * vec4(posicion, 1.0);
}