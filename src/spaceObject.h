#pragma once

#include <vector>
#include <string>
using namespace std;

#include "raylib.h"
#include "raymath.h"

#include <iostream>

class Object
{
public:
	string name = "";
	Vector2 position;
	Vector2 size;
	Vector2 origin = {0.5f, 0.5f};
	float rotation = 0;

	Object() {}

	const Rectangle getRec()
	{
		return {
			position.x - (size.x * origin.x),
			position.y - (size.y * origin.y),
			size.x,
			size.y};
	}

	virtual void Update() {}
	virtual void Draw() = 0;
	virtual void Unload() {}

private:
};

const Vector2 gravity = {0, 10}; // x: 0, y: 10

// Returns the collision normal
Vector2 CheckScreenCollisions(Vector2 pos, Vector2 sizeBounds)
{
	Vector2 halfSize = {sizeBounds.x * 0.5f, sizeBounds.y * 0.5f};

	int top = (pos.y - halfSize.y) < 0 ? 1 : 0;
	int left = (pos.x - halfSize.x) < 0 ? 1 : 0;
	int bottom = (pos.y + halfSize.y) > GetScreenHeight() ? 1 : 0;
	int right = (pos.x + halfSize.x) > GetScreenWidth() ? 1 : 0;

	return {
		static_cast<float>(right + left),
		static_cast<float>(top + bottom),
	};
}

class PhysicsObject : public Object
{
public:
	Vector2 velocity;
	Vector2 force;
	float mass = 1.0f;
	bool hasGravity = true;

	const float terminalVelocity = 500.0f; // Max velocity (pixels per second)
	const float idleThreshold = 25.0f;

	bool isIdle = false;
	bool colliding = false;

	void Update() override
	{
		isIdle = Vector2Length(velocity) < idleThreshold && Vector2Length(force) < idleThreshold && colliding;

		if (hasGravity)
		{
			force = Vector2Add(force, Vector2Scale(gravity, mass));
		}

		// Update velocity by adding force divided by mass (F = ma -> a = F / m)
		velocity = Vector2Clamp(
			Vector2Add(velocity, Vector2Scale(force, 1.0f / mass)),
			Vector2Scale({1, 1}, -terminalVelocity),
			Vector2Scale({1, 1}, terminalVelocity));

		// Reset the force after updating velocity (to avoid accumulation of forces)
		// force = {0, 0};

		Vector2 newPosition = Vector2Add(position, Vector2Scale(velocity, GetFrameTime()));
		Vector2 screenHitNormals = CheckScreenCollisions(newPosition, size);

		if (screenHitNormals.x != 0 || screenHitNormals.y != 0)
		{
			Vector2 reflections = {
				static_cast<float>(abs(screenHitNormals.x) == 1 ? -1 : 1),
				static_cast<float>(abs(screenHitNormals.y) == 1 ? -1 : 1)};

			velocity = Vector2Multiply(velocity, reflections);

			Vector2 drag = {
				(screenHitNormals.y != 0 ? 0.9f : 1.0f),
				(screenHitNormals.x != 0 ? 0.9f : 1.0f)};

			velocity = Vector2Multiply(velocity, drag);
		}

		// Update the position if no collision occurs and reset force to prevent drift
		if (screenHitNormals.x == 0)
		{
			position.x = newPosition.x;
			force.x = 0;
		}
		if (screenHitNormals.y == 0)
		{
			position.y = newPosition.y;
			force.y = 0;
		}
	}
};

class PhysicsCube : public PhysicsObject
{
public:
	Texture2D texture;

	void Draw() override
	{
		Rectangle source = {0, 0, (float)texture.width, (float)texture.height};
		Rectangle dest = {position.x, position.y, size.x, size.y};
		Vector2 texOrigin = {size.x * origin.x, size.y * origin.y};

		DrawTexturePro(texture, source, dest, texOrigin, rotation, WHITE);
	}
};

class Cube : public Object
{
public:
	void Draw() override
	{
		Rectangle cubeRect = this->getRec();
		DrawRectanglePro(cubeRect, {0, 0}, rotation, RED);
	}
};

void drawRectLine(Vector2 startingPoint, Vector2 endingPoint, float thickness, Color LineColor)
{
	Rectangle rect = {
		startingPoint.x - (thickness / 2),
		startingPoint.y - (thickness / 2),
		(endingPoint.x + (thickness / 2)) - startingPoint.x,
		(endingPoint.y + (thickness / 2)) - startingPoint.y};

	DrawRectangle(rect.x, rect.y, rect.width, rect.height, LineColor);
}

class PhysicsManager
{
public:
	void Solve(PhysicsObject *target, vector<Object *> children)
	{
		size_t childrenSize = children.size();
		bool hasCollided = false;
		for (size_t i = 0; i < childrenSize; i++)
		{
			Object *element = children[i];

			// Ignore self-collision
			if (element == target)
				continue;

			// AABB Collision detection here...
			bool checkAABB = CheckCollisionRecs(target->getRec(), element->getRec());

			// If there's no collision continue...
			if (checkAABB == false)
				continue;

			Rectangle targetRect = target->getRec();
			Rectangle elementRect = element->getRec();
			Rectangle overlapRect = GetCollisionRec(targetRect, elementRect);

			Vector2 hitNormal = {0, 0};
			float depth = 0.0f;

			// Check collision sides and determine normal and depth
			if (overlapRect.width < overlapRect.height)
			{
				hitNormal = {targetRect.x < elementRect.x ? -1.0f : 1.0f, 0.0f};
				depth = overlapRect.width;
			}
			else
			{
				hitNormal = {0.0f, targetRect.y < elementRect.y ? -1.0f : 1.0f};
				depth = overlapRect.height;
			}

			hasCollided = (depth > 0.1f);

			// Apply separation to both objects after collision
			Vector2 separation = Vector2Scale(hitNormal, depth);

			float friction = 0.0f;

			if (PhysicsObject *physElement = dynamic_cast<PhysicsObject *>(element))
			{
				separation = Vector2Scale(hitNormal, depth / 2);

				// Apply to the other object as well (if it's a PhysicsObject)
				physElement->position = Vector2Subtract(physElement->position, separation);

				// Reflect and adjust its velocity
				float dotProductElement = Vector2DotProduct(physElement->velocity, hitNormal);
				if (dotProductElement < 0)
				{
					Vector2 velocityScale = Vector2Scale(Vector2Negate(hitNormal), 2 * dotProductElement);
					physElement->velocity = Vector2Subtract(physElement->velocity, Vector2ClampValue(velocityScale, 0, 100000));
				}

				float restitution = 1.0f;

				// Handle bouncyness
				Vector2 velocityChange = Vector2Scale(hitNormal, 2 * Vector2DotProduct(physElement->velocity, hitNormal) * (1 + restitution));
				physElement->velocity = Vector2Subtract(physElement->velocity, velocityChange);

				// Add friction to the velocity after the collision
				Vector2 frictionForce = Vector2Scale(physElement->velocity, 1 - friction); // Apply friction
				physElement->velocity = frictionForce;

				Vector2 relativeVelocity = Vector2Subtract(target->velocity, physElement->velocity);
				float velocityAlongNormal = Vector2DotProduct(relativeVelocity, hitNormal); // Project relative velocity onto the normal

				// If the objects are moving apart, no need to do anything
				if (velocityAlongNormal < 0)
				{
					// Calculate the mass-based velocity transfer (elastic collision)
					float massA = target->mass;
					float massB = physElement->mass;

					// Calculate the change in velocity for both objects along the collision normal
					float impulse = velocityAlongNormal / (massA + massB);

					// Update the velocities based on the impulse
					physElement->velocity = Vector2Add(physElement->velocity, Vector2Scale(hitNormal, impulse * massA));
				}
			}

			// Reflect velocity based on collision normal (bounce effect)
			float dotProductTarget = Vector2DotProduct(target->velocity, hitNormal);
			if (dotProductTarget < 0)
			{
				Vector2 velocityScale = Vector2Scale(hitNormal, 2 * dotProductTarget);
				target->velocity = Vector2Subtract(target->velocity, Vector2ClampValue(velocityScale, 0, 100000));
			}

			// Add friction to the velocity after the collision
			Vector2 frictionForce = Vector2Scale(target->velocity, 1 - friction); // Apply friction
			target->velocity = frictionForce;

			target->position = Vector2Add(target->position, separation);

			// cout << "COLLISION -- " << target->name << endl;
			// cout << "	targetVel: {x:" << target->velocity.x << ", y:" << target->velocity.y << "}" << endl;
			// cout << "	hitNormal: {x:" << hitNormal.x << ", y:" << hitNormal.y << "}" << endl;
			// cout << "	separation: {x:" << separation.x << ", y:" << separation.y << "}" << endl;
			// cout << "	depth: " << depth << endl
			// 	 << endl;
		}

		target->colliding = hasCollided;

		// Handle screen boundary collisions
		// HandleScreenCollisions(target);
		target->position = Vector2Add(target->position, Vector2Scale(target->velocity, GetFrameTime()));
	}
};

class ObjectManager
{
private:
	vector<Object *> children;
	PhysicsManager physicsManager;

public:
	template <typename T>
	T *GetElementById(std::string identificator)
	{
		if (children.empty())
		{
			return nullptr;
		}

		for (size_t i = 0; i < children.size(); i++)
		{
			Object *currentElement = children[i];
			if (T *element = dynamic_cast<T *>(currentElement))
			{
				if (currentElement->name == identificator)
					return element;
			}
		}
	}

	Object *AddElement(string name, Object *element)
	{
		element->name = name;
		children.push_back(element);
		return element;
	}

	vector<Object *> GetChildren()
	{
		return children;
	}

	void Update()
	{
		for (size_t i = 0; i < children.size(); i++)
		{
			if (PhysicsObject *element = dynamic_cast<PhysicsObject *>(children[i]))
			{
				// if (element->isIdle) continue;

				cout << element->name << "(IDLE-" << (element->isIdle) << ", COLLIDING-" << element->colliding << "): " << "velocity(x:" << element->velocity.x << ", y:" << element->velocity.y << ")" << " force(x:" << element->force.x << ", y:" << element->force.y << ")" << endl;
				physicsManager.Solve(element, children);
			}
			children[i]->Update();
		}
	}

	void Draw()
	{
		for (size_t i = 0; i < children.size(); i++)
		{
			children[i]->Draw();
		}
	}

	void Unload()
	{
		for (size_t i = 0; i < children.size(); i++)
		{
			children[i]->Unload();
			delete children[i];
		}
		children.clear();
	}
};