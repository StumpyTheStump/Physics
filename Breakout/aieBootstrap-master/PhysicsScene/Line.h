#include "PhysicsObject.h"
class Line : public PhysicsObject
{
public:
	Line(glm::vec2 normal, float distance, glm::vec4 colour);
	~Line();

	virtual void fixedUpdate(glm::vec2 gravity, float timeStep);
	virtual void debug() {}
	virtual void makeGizmo();
	virtual void resetPosition();

	glm::vec2 getNormal() { return m_normal; }
	float getDistance() { return m_distanceToOrigin; }

protected:
	glm::vec2 m_normal;
	float m_distanceToOrigin;
	glm::vec4 m_colour;
};

