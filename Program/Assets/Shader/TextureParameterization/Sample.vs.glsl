#version 410 core

in vec4 vPosition;

void
main()
{
    gl_Position = vPosition;
}