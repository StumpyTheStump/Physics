#include "PhysicsScene.h"
#include "PhysicsObject.h"
#include "RigidBody.h"
#include "Circle.h"
#include "Line.h"
#include "Rectangle.h"
#include <algorithm>
#include <iostream>
#include <list>

PhysicsScene::PhysicsScene() : m_timeStep(0.01f), m_gravity(glm::vec2(0, 0))
{	
}

void PhysicsScene::addActor(PhysicsObject * actor)
{
	m_actors.push_back(actor);
}

bool PhysicsScene::removeActor(PhysicsObject * actor)
{
	for (int i = 0; i < m_actors.size(); ++i)
	{
		if (m_actors[i] == actor)
		{
			m_actors.erase(m_actors.begin() + i);
			return true;
		}
	}
	return false;
}


// Function pointer array for doing collisions
typedef bool(*fn) (PhysicsObject*, PhysicsObject*);

static fn collisionFunctionArray[] =
{
	PhysicsScene::Line2Line, PhysicsScene::Line2Circle, PhysicsScene::Line2Rect,
	PhysicsScene::Circle2Line, PhysicsScene::Circle2Circle, PhysicsScene::Circle2Rect,
	PhysicsScene::Rect2Line, PhysicsScene::Rect2Circle, PhysicsScene::Rect2Rect,
};

void PhysicsScene::checkForCollision()
{
	int actorCount = m_actors.size();

	// Check for collisions against all objects except this one
	for (int outer = 0; outer < actorCount - 1; outer++)
	{
		for (int inner = outer + 1; inner < actorCount; inner++)
		{
			PhysicsObject* object1 = m_actors[outer];
			PhysicsObject* object2 = m_actors[inner];
			int shapeID1 = object1->getShapeID();
			int shapeID2 = object2->getShapeID();

			int functionIndex = (shapeID1 * SHAPE_COUNT) + shapeID2;
			fn collisionFucntionPtr = collisionFunctionArray[functionIndex];
			if (collisionFucntionPtr != nullptr)
			{
				collisionFucntionPtr(object1, object2);
			}

		}
	}
}

bool PhysicsScene::Line2Line(PhysicsObject * obj1, PhysicsObject * obj2)
{
	Line *line1 = dynamic_cast<Line*>(obj1);
	Line *line2 = dynamic_cast<Line*>(obj2);


	if (line1 != nullptr && line2 != nullptr)
	{
		return true;
	}
	return false;
}

bool PhysicsScene::Line2Circle(PhysicsObject *obj1, PhysicsObject * obj2)
{
	return Circle2Line(obj1, obj2);
}

bool PhysicsScene::Line2Rect(PhysicsObject * obj1, PhysicsObject * obj2)
{
	return Rect2Line(obj2, obj1);
}

bool PhysicsScene::Circle2Line(PhysicsObject * obj1, PhysicsObject * obj2)
{
	Circle *circle = dynamic_cast<Circle*>(obj1);
	Line *line = dynamic_cast<Line*>(obj2);

	if (circle != nullptr && line != nullptr)
	{
		//glm::vec2 displacement = circle->getPosition() - line->getNormal();
		glm::vec2 collisionNormal = line->getNormal();
		float circleToLine = glm::dot(circle->getPosition(), collisionNormal) - line->getDistance();
		float intersection = circle->getRadius() - circleToLine;
		//float length = glm::length(displacement);
		//float collisionPoint = circle->getRadius() + line->getDistance();
		//float overlap = collisionPoint - length;

		if (intersection > 0)
		{


			circle->setPosition(circle->getPosition() + (intersection * collisionNormal));
			line->resolveCollision(circle);
			return true; // resolve collision for plane
		}
		
	}
	return false;
}

bool PhysicsScene::Circle2Circle(PhysicsObject * obj1, PhysicsObject * obj2)
{
	Circle *circle1 = dynamic_cast<Circle*>(obj1);
	Circle *circle2 = dynamic_cast<Circle*>(obj2);	

	if (circle1 != nullptr && circle2 != nullptr)
	{
		// Get Displacement
		glm::vec2 displacement = circle1->getPosition() - circle2->getPosition();
		glm::vec2 collisionNormal = glm::normalize(displacement);
		float length = glm::length(displacement);
		float radialSum = circle1->getRadius() + circle2->getRadius();
		float overlap = radialSum - length;
		if (length <= (radialSum))
		{
			circle1->setPosition(circle1->getPosition() + (overlap * collisionNormal));
			circle2->setPosition(circle2->getPosition() - (overlap * collisionNormal));
			circle1->resolveCollision(circle2);
			

			return true; //Resolve collision for circles
		}
		
	}
	return false;
}

bool PhysicsScene::Circle2Rect(PhysicsObject * obj1, PhysicsObject * obj2)
{
	Circle *circle1 = dynamic_cast<Circle*>(obj1);
	Rectangle *rect1 = dynamic_cast<Rectangle*>(obj2);

	if (circle1 != nullptr && rect1 != nullptr)
	{
		glm::vec2 max1 = rect1->getPosition() + (rect1->getSize());
		glm::vec2 min1 = rect1->getPosition() - (rect1->getSize());
		glm::vec2 revmax1 = rect1->getPosition() + (-rect1->getSize().x, rect1->getSize().y);
		glm::vec2 revmin1 = rect1->getPosition() - (-rect1->getSize().x, rect1->getSize().y);

		glm::vec2 checkCollison = glm::clamp(circle1->getPosition(), min1, max1);
		glm::vec2 displacement = checkCollison - circle1->getPosition();

		glm::vec2 offset = glm::normalize(displacement);
		float length = glm::length(displacement);
		float overlap = circle1->getRadius() - length;

		if (length < circle1->getRadius())
		{
			circle1->setPosition(circle1->getPosition() - (overlap * offset)* 0.5f);
			rect1->setPosition(rect1->getPosition() + (overlap * offset)* 0.5f);
			circle1->resolveCollision(rect1);
			
		}
		return true;
	}
	return false;
}

bool PhysicsScene::Rect2Circle(PhysicsObject * obj1, PhysicsObject * obj2)
{
	return Circle2Rect(obj1, obj2);
}

bool PhysicsScene::Rect2Rect(PhysicsObject * obj1, PhysicsObject * obj2)
{
	Rectangle *rect1 = dynamic_cast<Rectangle*>(obj1);
	Rectangle *rect2 = dynamic_cast<Rectangle*>(obj2);

	glm::vec2 max1 = rect1->getPosition() + (rect1->getSize());
	glm::vec2 max2 = rect2->getPosition() + (rect2->getSize());
	glm::vec2 min1 = rect1->getPosition() - (rect1->getSize());
	glm::vec2 min2 = rect2->getPosition() - (rect2->getSize());

	float xOverlap = glm::min(max1.x - min2.x, max2.x - min1.x);
	float yOverlap = glm::min(max1.y - min2.y, max2.y - min1.y);


	if (rect1 != nullptr && rect2 != nullptr)
	{
		if (max1.x < min2.x || max2.x < min1.x || max1.y < min2.y || max2.y < min1.y)
		{
			
		}
		else 
		{

			if (xOverlap < yOverlap)
			{
				rect1->setPosition(rect1->getPosition() + 0.5f * xOverlap * glm::vec2(1, 0));
				rect2->setPosition(rect2->getPosition() - 0.5f * xOverlap * glm::vec2(1, 0));
			}
			else
			{
				rect1->setPosition(rect1->getPosition() - 0.5f * yOverlap * glm::vec2(0, 1));
				rect2->setPosition(rect2->getPosition() + 0.5f * yOverlap * glm::vec2(0, 1));
			}
			
			
			rect1->resolveCollision(rect2);
			return true;
		}
		
	}
	return false;
}

bool PhysicsScene::Rect2Line(PhysicsObject * obj1, PhysicsObject * obj2)
{
	Rectangle *rect1 = dynamic_cast<Rectangle*>(obj1);
	Line *line1 = dynamic_cast<Line*>(obj2);
	

	if (rect1 != nullptr && line1 != nullptr)
	{
		glm::vec2 collisionNormal = line1->getNormal();

		glm::vec2 max1 = rect1->getPosition() + ( rect1->getSize());
		glm::vec2 min1 = rect1->getPosition() - ( rect1->getSize());
		glm::vec2 revmax1 = rect1->getPosition() + (-rect1->getSize().x, rect1->getSize().y);
		glm::vec2 revmin1 = rect1->getPosition() - (-rect1->getSize().x, rect1->getSize().y);


		float max1Distance = glm::dot(max1, line1->getNormal()) - line1->getDistance();
		float min1Distance = glm::dot(min1, line1->getNormal()) - line1->getDistance();
		float revmax1Distance = glm::dot(revmax1, line1->getNormal()) - line1->getDistance();
		float revmin1Distance = glm::dot(revmin1, line1->getNormal()) - line1->getDistance();

		float displacement = glm::dot(rect1->getPosition(), collisionNormal) - line1->getDistance();

		if (min1Distance < 0)
		{
			rect1->setPosition(rect1->getPosition() + (collisionNormal * displacement) * 0.5f);
			line1->resolveCollision(rect1);
			return true; // resolve collision for plane
		}
		if (max1Distance < 0)
		{
			rect1->setPosition(rect1->getPosition() + (collisionNormal * displacement) * 0.5f);
			line1->resolveCollision(rect1);
			return true; // resolve collision for plane
		}
		if (revmin1Distance < 0)
		{
			rect1->setPosition(rect1->getPosition() + (collisionNormal * displacement) * 0.5f);
			line1->resolveCollision(rect1);
			return true; // resolve collision for plane
		}
		if (revmax1Distance < 0)
		{
			rect1->setPosition(rect1->getPosition() + (collisionNormal * displacement) * 0.5f);
			line1->resolveCollision(rect1);
			return true; // resolve collision for plane
		}

	}
	return false;
}

void PhysicsScene::update(float dt)
{
	static std::list<PhysicsObject*> dirty;
	// Update physics at a fixed time step

	static float accumulatedTime = 0.0f;
	accumulatedTime += dt;

	while (accumulatedTime >= m_timeStep)
	{
		for (auto pActor : m_actors)
		{
			pActor->fixedUpdate(m_gravity, m_timeStep);
		}
		accumulatedTime -= m_timeStep;
	}

	checkForCollision();

	//// check for collisions
	//for (auto pActor : m_actors)
	//{
	//	for (auto pOther : m_actors)
	//	{
	//		if (pActor == pOther)
	//			continue;
	//		if (std::find(dirty.begin(), dirty.end(), pActor) != dirty.end() &&
	//			std::find(dirty.begin(), dirty.end(), pOther) != dirty.end())
	//			continue;

	//		RigidBody* pRigid = dynamic_cast<RigidBody*>(pActor);
	//		if (pRigid->checkCollision(pOther) == true)
	//		{
	//			pRigid->applyForceToActor(dynamic_cast<RigidBody*>(pOther), pRigid->getVelocity() * pRigid->getMass());
	//			dirty.push_back(pRigid);
	//			dirty.push_back(pOther);
	//		}
	//	}
	//}
	//dirty.clear();
}

void PhysicsScene::updateGizmos()
{
	for (auto pActor : m_actors)
	{
		pActor->makeGizmo();
	}
}

void PhysicsScene::debugScene()
{
	int count = 0;
	for (auto pActor : m_actors)
	{
		std::cout << count << " : ";
		pActor->debug();
		count++;
	}
}



PhysicsScene::~PhysicsScene()
{
	for (auto pActor : m_actors)
	{
		delete pActor;
	}
}



