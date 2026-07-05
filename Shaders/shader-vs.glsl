#version 410 core
layout (location = 0) in vec3 posicion;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangente;
layout (location = 4) in vec3 bitangente;

out salidaVS {
    vec2 texCoordV;
    vec3 posicionTg;
    vec3 posLuzTg;
    vec3 dirLuzTg;
    vec4 coordenadasSombra; // <-- NUEVO: Coordenadas para el shadow map
} salida;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 uPosicionLuzV;
uniform vec3 uDireccionLuzV;
uniform mat4 matrizSombras; // <-- NUEVO: Matriz de sombras

void main() {
    // 1. Transformar tangente, bitangente y normal al espacio de visión
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    vec3 T = normalize(vec3(view * vec4(normalMatrix * tangente, 0.0)));
    vec3 B = normalize(vec3(view * vec4(normalMatrix * bitangente, 0.0)));
    vec3 N = normalize(vec3(view * vec4(normalMatrix * normal, 0.0)));

    // 2. Calcular matriz TBN
    mat3 TBN = mat3(T, B, N);
    mat3 TBNinv = transpose(TBN);

    // 3. Transformar posición del vértice al espacio tangente
    vec4 posVision = view * model * vec4(posicion, 1.0);
    salida.posicionTg = TBNinv * posVision.xyz;

    // 4. Transformar luz al espacio tangente
    salida.posLuzTg = TBNinv * uPosicionLuzV;
    salida.dirLuzTg = TBNinv * uDireccionLuzV;

    // 5. Calcular coordenadas de sombra
    salida.coordenadasSombra = matrizSombras * vec4(posicion, 1.0);

    salida.texCoordV = texCoord;
    gl_Position = projection * posVision;
}