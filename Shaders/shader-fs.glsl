#version 410 core
in salidaVS {
    vec2 texCoordV;
    vec3 posicionTg;
    vec3 posLuzTg;
    vec3 dirLuzTg;
    vec4 coordenadasSombra; // <-- NUEVO
} entrada;

out vec4 colorFragmento;

// Materiales
uniform vec3 uColorAmbiente;
uniform vec3 uColorDifuso;
uniform vec3 uColorEspecular;
uniform float uShininess;

// Texturas
uniform sampler2D uTextura;
uniform sampler2D uMapaNormales;
uniform bool uUsarNormalMap;

// Sombras
uniform sampler2DShadow muestreadorSombra; // <-- NUEVO
uniform bool uAplicarSombra;               // <-- NUEVO

// Propiedades de la luz actual
uniform vec3 uLuzAmbiente;
uniform vec3 uLuzDifusa;
uniform vec3 uLuzEspecular;
uniform float uAnguloApertura;
uniform float uExponenteBordes;

// === Función para obtener la normal ===
vec3 obtenerNormal() {
    if (uUsarNormalMap) {
        vec3 normalBM = texture(uMapaNormales, entrada.texCoordV).rgb;
        return normalize(normalBM * 2.0 - 1.0);
    } else {
        return vec3(0.0, 0.0, 1.0);
    }
}

// === Función para obtener el factor de sombra ===
float obtenerSombra() {
    if (uAplicarSombra) {
        // textureProj hace la división perspectiva y compara la profundidad
        return textureProj(muestreadorSombra, entrada.coordenadasSombra);
    }
    return 1.0; // Si no aplicamos sombras, el factor es 1 (sin oscurecer)
}

// === SUBRUTINAS DE ILUMINACIÓN ===
subroutine vec4 fCalcularIluminacion();
subroutine uniform fCalcularIluminacion uTipoLuz;

subroutine(fCalcularIluminacion)
vec4 luzAmbiente() {
    vec3 resultado = uLuzAmbiente * uColorAmbiente;
    return vec4(resultado, 1.0);
}

subroutine(fCalcularIluminacion)
vec4 luzPuntual() {
    vec3 n = obtenerNormal();
    vec3 l = normalize(entrada.posLuzTg - entrada.posicionTg);
    vec3 v = normalize(-entrada.posicionTg);
    vec3 r = reflect(-l, n);

    float diff = max(dot(n, l), 0.0);
    vec3 difusa = uLuzDifusa * uColorDifuso * diff;
    float spec = pow(max(dot(r, v), 0.0), uShininess);
    vec3 especular = uLuzEspecular * uColorEspecular * spec;

    // Las luces puntuales no proyectan sombras en esta implementación básica
    return vec4(difusa + especular, 1.0);
}

subroutine(fCalcularIluminacion)
vec4 luzDireccional() {
    vec3 n = obtenerNormal();
    vec3 l = normalize(-entrada.dirLuzTg);
    vec3 v = normalize(-entrada.posicionTg);
    vec3 r = reflect(-l, n);

    float diff = max(dot(n, l), 0.0);
    vec3 difusa = uLuzDifusa * uColorDifuso * diff;
    float spec = pow(max(dot(r, v), 0.0), uShininess);
    vec3 especular = uLuzEspecular * uColorEspecular * spec;

    // Aplicar factor de sombra
    float sombra = obtenerSombra();
    return vec4(sombra * (difusa + especular), 1.0);
}

subroutine(fCalcularIluminacion)
vec4 luzFoco() {
    vec3 n = obtenerNormal();
    vec3 l = normalize(entrada.posLuzTg - entrada.posicionTg);
    vec3 v = normalize(-entrada.posicionTg);
    vec3 r = reflect(-l, n);
    vec3 d = normalize(-entrada.dirLuzTg);

    float cosAngulo = dot(-l, d);
    float cosGamma = cos(uAnguloApertura);
    float factorCono = smoothstep(cosGamma, cosGamma + 0.01, cosAngulo);

    float diff = max(dot(n, l), 0.0);
    vec3 difusa = uLuzDifusa * uColorDifuso * diff * factorCono;
    float spec = pow(max(dot(r, v), 0.0), uShininess);
    vec3 especular = uLuzEspecular * uColorEspecular * spec * factorCono;

    // Aplicar factor de sombra
    float sombra = obtenerSombra();
    return vec4(sombra * (difusa + especular), 1.0);
}

// === SUBRUTINA 2: COLOR BASE ===
subroutine vec4 fObtenerColorBase();
subroutine uniform fObtenerColorBase uColorBase;

subroutine(fObtenerColorBase)
vec4 colorDeMaterial() {
    return vec4(uColorDifuso, 1.0);
}

subroutine(fObtenerColorBase)
vec4 colorDeTextura() {
    return texture(uTextura, entrada.texCoordV);
}

uniform int uModoVisualizacion;

void main() {
    vec4 colorBase = uColorBase();
    vec4 colorIluminado = uTipoLuz();

    if (uModoVisualizacion == 0) {
        colorFragmento = vec4(1.0, 0.0, 0.0, 1.0);
    } else {
        colorFragmento = colorIluminado * colorBase;
    }
}