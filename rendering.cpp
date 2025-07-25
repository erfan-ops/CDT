#include "rendering.h"

#include <Windows.h>
#include <gl/GL.h>
#include <math.h>

#define TAU_F 6.2831853071f

void filledCircle(const float centerX, const float centerY, const float radius, const float r, const float g, const float b, const float a, const int nSegments) {
	const float nSegments_f = (float)(nSegments);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	glColor4f(r, g, b, a);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(centerX, centerY);
	for (int i = 0; i <= nSegments; i++) {
		float theta = TAU_F * (float)i / nSegments_f;
		float x = radius * cosf(theta);
		float y = radius * sinf(theta);
		glVertex2f(x + centerX, y + centerY);
	}
	glEnd();
}

void filledTriangle(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3, const float r, const float g, const float b, const float a) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	glColor4f(r, g, b, a);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glVertex2f(x3, y3);
	glEnd();
}
