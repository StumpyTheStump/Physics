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

void Line::resolveCollision(RigidBody * actor2)
{
	glm::vec2 normal = m_normal;
	glm::vec2 relativeVelocity = actor2->getVelocity();

	float elasticity = actor2->getElasticity();
	float j = glm::dot(-(1 + elasticity) * (relativeVelocity), normal) / glm::dot(normal, normal* ((1.0f / actor2->getMass())));

	glm::vec2 force = normal * j;

	actor2->applyForce( force);
}
