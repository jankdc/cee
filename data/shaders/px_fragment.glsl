#version 330 core

uniform bool isFlipped;

out vec4 finalColor;

void main()
{
  if (isFlipped)
    finalColor = vec4(1.0f);
  else
    finalColor = vec4(0.0f);
}
