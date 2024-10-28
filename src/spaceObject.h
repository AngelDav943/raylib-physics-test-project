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
	float rotation = 0;

	Object() {}

	virtual void Update() {}
	virtual void Draw() = 0;
	virtual void Unload() {}

private:
};

const Vector2 gravity = {0, 10};

class PhysicsObject : public Object
{
public:
	Vector2 velocity;
	Vector2 force;
	float mass = 1.0f;
	bool hasGravity = true;

	// Returns the collision normal
	Vector2 CheckScreenCollisions(Vector2 pos, Vector2 sizeBounds) const
	{
		Vector2 halfSize = {sizeBounds.x * 0.5f, sizeBounds.y * 0.5f};

		int top = -((pos.y - halfSize.y) < 0);
		int left = -((pos.x - halfSize.x) < 0);
		int bottom = (pos.y + halfSize.y) > GetScreenHeight();
		int right = (pos.x + halfSize.x) > GetScreenWidth();

		return {
			static_cast<float>(right + left),
			static_cast<float>(top + bottom),
		};
	}

	void Update() override
	{

		if (hasGravity)
		{
			force = Vector2Add(force, Vector2Scale(gravity, mass));
		}

		velocity = Vector2Add(velocity, Vector2Divide(force, {mass, mass}));

		if (isinf(velocity.x) || isnan(velocity.x))
		{
			velocity.x = 0;
		}

		if (isinf(velocity.y) || isnan(velocity.y))
		{
			velocity.y = 0;
		}

		// cout << "x:" << velocity.x << " y:" << velocity.y << endl;
		Vector2 newPosition = Vector2Add(position, Vector2Scale(velocity, GetFrameTime()));

		Vector2 screenHitNormals = CheckScreenCollisions(newPosition, size);

		if (screenHitNormals.x != 0 || screenHitNormals.y != 0)
		{
			Vector2 reflections = {
				static_cast<float>(abs(screenHitNormals.x) == 1 ? -1 : 1),
				static_cast<float>(abs(screenHitNormals.y) == 1 ? -1 : 1)};

			Vector2 newVelocity = {
				velocity.x * reflections.x,
				velocity.y * reflections.y,
			};

			Vector2 drag = {
				screenHitNormals.y != 0 ? 0.9f : 1,
				screenHitNormals.x != 0 ? 0.9f : 1,
			};

			velocity = Vector2Multiply(newVelocity, drag);
		}

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
		// position = newPosition;
		// force = {0, 0}; // Reset net force
	}
};

class PhysicsCube : public PhysicsObject
{
public:
	void Draw() override
	{
		DrawRectanglePro({position.x, position.y, size.x, size.y}, {size.x * 0.5f, size.y * 0.5f}, rotation, ORANGE);
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

class ObjectManager
{
private:
	vector<Object *> children;

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