#pragma once

class MovingBox
{
public:
	MovingBox(b2Body* b) : body{ b }, target{ b2Vec2_zero } {}

	b2Body* GetBody() { return body; }
	void SetBody(b2Body* b) { body = b; }

	const b2Vec2& GetTarget() const { return target; }
	void SetTarget(const b2Vec2& t) { target = t; }

	void OnFrame()
	{
		b2Body* body = GetBody();
		b2Vec2 bPos = body->GetPosition();
		if ((target - bPos).Length() < 0.1)
			body->SetLinearVelocity(b2Vec2_zero);
		else
			body->SetLinearVelocity(target - bPos);
	}

private:
	b2Body* body;
	b2Vec2 target;
};