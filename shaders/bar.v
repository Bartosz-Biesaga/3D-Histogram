#version 110

#define A 0.2

uniform float time;
uniform mat4 MVP;

attribute vec2 position;

varying vec4 varColor;

void main()
{
    float yPosition = 0.;
    for(float d = 2.; d < 30.; d += d)
        yPosition += cos(3. * position.x * d  + 3.7 * position.y * d + (2. + 0.07 * position.x * position.y) * time * cos(d)) / d;
    yPosition *= A;
    gl_Position = MVP * vec4(position.x, yPosition, position.y, 1.);

    vec2 position2 = position;
    vec4 color = vec4(0., 0., 0., 0.);
    for(int i = 0; i < 90; ++i)
        color += (cos(abs(position.x + 2.)*.5+time+vec4(0,2,4,3))+1.3)/32./abs(position.y + 2.);
    color = color / (1.0 + abs(color)) * 0.8;
    color.a = 1.;
    varColor = color;
}