#pragma once

class BoxQuery : public b2QueryCallback
{
public:
	BoxQuery() :b2QueryCallback{}, canCreate{ true } {}

	bool canCreate;

	bool ReportFixture(b2Fixture* fixture) override
	{
		canCreate = false;
		return false;
	}
};

class ClickQuery : public b2QueryCallback
{
public:
	ClickQuery(const b2Vec2& p) : point{ p }, fixture{ nullptr } {}

	bool ReportFixture(b2Fixture* f)
	{
		b2Body* body = f->GetBody();
		if (body->GetType() == b2_dynamicBody)
		{
			bool inside = f->TestPoint(point);
			if (inside)
			{
				fixture = f;
				return false;
			}
		}
		return true;
	}

	b2Vec2 point;
	b2Fixture* fixture;
};