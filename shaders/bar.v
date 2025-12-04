#version 110

uniform float time;
uniform mat4 MVP;

attribute vec2 position;

varying vec4 varColor;

void main()
{
    float yPosition = 0.5 * sin(time + position.x * 2.0);
    gl_Position = MVP * vec4(position.x, yPosition, position.y, 1.);
    varColor = vec4(0., 0., 0., 1.0);
}