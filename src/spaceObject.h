#pragma once

#include <vector>
#include <string>
using namespace std;

#include "raylib.h"
#include "raymath.h"

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

const Vector2 gravity = {0, 10};

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

	void Update() override
	{

		if (hasGravity)
		{
			force = Vector2Add(force, Vector2Scale(gravity, mass));
		}

		// Update velocity by adding force divided by mass (F = ma -> a = F / m)
		velocity = Vector2Clamp(
			Vector2Add(velocity, Vector2Scale(force, 1.0f / mass)),
			Vector2Scale({1, 1}, -terminalVelocity),
			Vector2Scale({1, 1}, terminalVelocity));

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
		// DrawRectanglePro({position.x, position.y, size.x, size.y}, {size.x * 0.5f, size.y * 0.5f}, rotation, ORANGE);

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
		// cout << "hello coming from Cube { x:" << position.x << ", y:" << position.y << " }" << endl;
		DrawRectanglePro({position.x, position.y, 100, 100}, {50, 50}, rotation, RED);
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
		for (size_t i = 0; i < children.size(); i++)
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

			float dotProductTarget = Vector2DotProduct(target->velocity, hitNormal);

			if (dotProductTarget < 0)
			{
				target->velocity = Vector2Subtract(target->velocity, Vector2Scale(hitNormal, 2 * dotProductTarget));
			}

			// Solve collision here...
			Vector2 separation = Vector2Scale(hitNormal, depth);

			if (PhysicsObject *physElement = dynamic_cast<PhysicsObject *>(children[i]))
			{
				separation = Vector2Scale(hitNormal, depth / 2);
				float dotProductElement = Vector2DotProduct(physElement->velocity, hitNormal); //(physElement->velocity.x * hitNormal.x) + (physElement->velocity.y * hitNormal.y);

				if (dotProductElement < 0)
				{
					physElement->velocity = Vector2Subtract(physElement->velocity, Vector2Scale(hitNormal, 2 * dotProductElement));
				}

				Vector2 newPosition = Vector2Subtract(physElement->position, separation);
				Vector2 elementScreenNormals = CheckScreenCollisions(newPosition, physElement->size);

				if (elementScreenNormals.x == 0)
				{
					physElement->position.x = newPosition.x;
				}
				if (elementScreenNormals.y == 0)
				{
					physElement->position.y = newPosition.y;
				}
			}

			Vector2 targetNewPosition = Vector2Add(target->position, separation);
			Vector2 targetScreenNormals = CheckScreenCollisions(targetNewPosition, target->size);

			if (targetScreenNormals.x == 0)
			{
				target->position.x = targetNewPosition.x;
			}
			if (targetScreenNormals.y == 0)
			{
				target->position.y = targetNewPosition.y;
			}

			cout << "COLLISION -- " << target->name << endl;
			cout << "	targetVel: {x:" << target->velocity.x << ", y:" << target->velocity.y << "}" << endl;
			cout << "	hitNormal: {x:" << hitNormal.x << ", y:" << hitNormal.y << "}" << endl;
			cout << "	separation: {x:" << separation.x << ", y:" << separation.y << "}" << endl;
			cout << "	depth: " << depth << endl
				 << endl;
		}
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