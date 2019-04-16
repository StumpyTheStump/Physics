#include "Line.h"
#include <Gizmos.h>

Line::Line(glm::vec2 normal, float distance, glm::vec4 colour) : PhysicsObject::PhysicsObject(ShapeType::LINE)
{
	m_normal = glm::normalize(normal);
	m_distanceToOrigin = distance;
	m_colour = colour;
}


Line::~Line()
{
}

void Line::fixedUpdate(glm::vec2 gravity, float timeStep)
{
}

void Line::makeGizmo()
{
	float lineSegmentLenght = 300;
	glm::vec2 centrePoint = m_normal * m_distanceToOrigin;
	glm::vec2 parallel(m_normal.y, -m_normal.x);
	glm::vec4 m_colour(1, 1, 1, 1);
	glm::vec2 start = centrePoint + (parallel * lineSegmentLenght);
	glm::vec2 end = centrePoint - (parallel * lineSegmentLenght);
	aie::Gizmos::add2DLine(start, end, m_colour);



}

void Line::resetPosition()
{
	return;
}
