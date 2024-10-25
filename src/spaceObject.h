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
	float rotation = 0;

	Object() {}

	virtual void Update() {}
	virtual void Draw() = 0;
	virtual void Unload() {}

private:
};

class CubeObject : public Object
{
public:
	void Draw() override
	{
		DrawRectanglePro({0, 0, 100, 100}, position, rotation, ORANGE);
	}
};

class PhysicsCube : public CubeObject
{
public:
	Vector2 velocity;
	bool hasGravity = true;

	void Update() override
	{
		Vector2 resultVelocity = Vector2Add(velocity, {
			0.0f, hasGravity ? -100.0f : 0.0f
		});
		position = Vector2Add(position, Vector2Scale(resultVelocity, GetFrameTime()));
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