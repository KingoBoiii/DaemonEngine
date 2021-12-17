#type vertex
#version 330 core
layout(location = 0) in vec3 POS;
layout(location = 1) in vec4 COL;

out vec4 color;
uniform mat4 ProjectionView;

void main() {
	gl_Position = ProjectionView * vec4(POS, 1.0);
	color = COL;
}

#type pixel
#version 330 core
layout(location = 0) out vec4 FragColor;

in vec4 color;

void main() {
	FragColor = color;
}


