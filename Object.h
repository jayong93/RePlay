#pragma once

class MovingBox
{
public:
	MovingBox(b2Body* b) : body{ b }, target{ b2Vec2_zero } {}

	b2Body* GetBody() { return body; }
	void SetBody(b2Body* b) { body = b; }

	const b2Vec2& GetTarget() const { return target; }
	void SetTarget(const b2Vec2& t) { target = t; }

private:
	b2Body* body;
	b2Vec2 target;
};