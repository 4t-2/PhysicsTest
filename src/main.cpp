#include "../lib/AGL/agl.hpp"
#include <limits>
#include <vector>

class Circle : public agl::Drawable
{
	private:
		agl::Circle &shape;

	public:
		agl::Vec<float, 2> position;
		agl::Vec<float, 2> velocity;
		agl::Vec<float, 2> force;
		float			   radius = 1;
		float			   mass	  = 1;

		Circle(agl::Circle &shape) : shape(shape)
		{
		}

		void update()
		{
			velocity = velocity + (force * (1. / mass));
			force	 = {0, 0};

			position = position + velocity;
		}

		void drawFunction(agl::RenderWindow &window) override
		{
			shape.setPosition(position);
			shape.setSize({radius, radius});

			window.drawShape(shape);
		}
};

class PhySim : public agl::Drawable
{
	private:
		std::vector<Circle> circleVec;
		agl::Circle		   &shape;
		float				gravity;

	public:
		PhySim(agl::Circle &shape) : shape(shape)
		{
		}

		void setGravity(float gravity)
		{
			this->gravity = gravity;
		}

		void addCircle(agl::Vec<float, 2> position, float radius, agl::Vec<float, 2> force, float mass)
		{
			circleVec.emplace_back(Circle(shape));
			circleVec[circleVec.size() - 1].position = position;
			circleVec[circleVec.size() - 1].radius	 = radius;
			circleVec[circleVec.size() - 1].force	 = force;
			circleVec[circleVec.size() - 1].mass	 = mass;
		}

		void update()
		{
			// update position
			for (Circle &circle : circleVec)
			{
				circle.update();
				circle.force += agl::Vec<float, 2>{0, gravity} * circle.mass;
			}

			// collision detection
			for (Circle &circle : circleVec)
			{
				for (Circle &otherCircle : circleVec)
				{
					if (&otherCircle == &circle)
					{
						continue;
					}

					agl::Vec<float, 2> circleOffset = otherCircle.position - circle.position;

					float circleDistance = circleOffset.length();

					float circleOverlap = (otherCircle.radius + circle.radius) - circleDistance;

					if (circleOverlap > 0)
					{
						agl::Vec<float, 2> offsetNormal = circleOffset.normalized();
						agl::Vec<float, 2> pushback		= offsetNormal * circleOverlap;

						circle.position -= pushback * (otherCircle.mass / (circle.mass + otherCircle.mass));
						otherCircle.position += pushback * (circle.mass / (circle.mass + otherCircle.mass));

						float actingMass = circle.mass > otherCircle.mass ? otherCircle.mass : circle.mass;

						// agl::Vec<float, 2> velocityDelta = circle.velocity - otherCircle.velocity;
						//
						// agl::Vec<float, 2> forceBack =
						// 	agl::Vec<float, 2>{velocityDelta.x * offsetNormal.x, velocityDelta.y * offsetNormal.y} *
						// 	actingMass;
						//
						// circle.force -= forceBack;
						// otherCircle.force += forceBack;

						circle.force -= pushback * actingMass;
						otherCircle.force += pushback * actingMass;
					}
				}

				// boundry
				agl::Vec<float, 2> boundryPosition = {500, 500};
				float			   boundryRadius   = 500;

				agl::Vec<float, 2> offset = circle.position;
				offset					  = offset - boundryPosition;

				float distance = offset.length();

				float overlap = distance + circle.radius - boundryRadius;

				if (overlap > 0)
				{
					agl::Vec<float, 2> offsetNormal = offset.normalized();
					agl::Vec<float, 2> pushback		= offsetNormal * overlap;

					circle.position -= pushback;

					// circle.force -=
					// 	agl::Vec<float, 2>{circle.velocity.x * offsetNormal.x, circle.velocity.y * offsetNormal.y} *
					// 	circle.mass;

					circle.force -= pushback * circle.mass;
				}
			}
		}

		void drawFunction(agl::RenderWindow &window) override
		{
			for (Circle &circle : circleVec)
			{
				window.draw(circle);
			}
		}

		std::vector<Circle> &getCircleVec()
		{
			return circleVec;
		}
};

int main()
{
	agl::RenderWindow window;
	window.setup({1920, 1080}, "PhysicsTest");
	window.setFPS(60);
	window.setClearColor(agl::Color::Black);

	agl::ShaderBuilder vert;
	vert.setDefaultVert();
	std::string vertSrc = vert.getSrc();

	agl::ShaderBuilder frag;
	frag.setDefaultFrag();
	std::string fragSrc = frag.getSrc();

	agl::Shader shader;
	shader.compileSrc(vertSrc, fragSrc);
	window.getShaderUniforms(shader);
	shader.use();

	agl::Camera camera;
	camera.setOrthographicProjection(0, 1920, 1080, 0, 0.1, 100);
	camera.setView({0, 0, 10}, {0, 0, 0}, {0, 1, 0});

	window.updateMvp(camera);

	agl::Event event;
	event.setWindow(window);

	agl::Texture blank;
	blank.setBlank();

	agl::Circle shape(32);
	shape.setTexture(&blank);
	shape.setColor(agl::Color::White);

	PhySim phySim(shape);
	phySim.setGravity(0.1);

	phySim.addCircle({100, 500}, 100, {4, 0}, 2);
	phySim.addCircle({500, 500}, 100, {0, 0}, 1);

	agl::Circle boundry(32);
	boundry.setColor(agl::Color::Blue);
	boundry.setPosition({500, 500, -1});
	boundry.setSize({500, 500});
	boundry.setTexture(&blank);

	while (!event.windowClose())
	{
		event.poll([](XEvent xev) {});

		window.clear();

		window.draw(phySim);
		window.drawShape(boundry);

		window.display();

		phySim.update();

		float force1 = phySim.getCircleVec()[0].velocity.length() * phySim.getCircleVec()[0].mass;
		// float force2 = phySim.getCircleVec()[1].velocity.length() *
		// phySim.getCircleVec()[1].mass;

		std::cout << '\n';
		std::cout << force1 << '\n';
		// std::cout << force2 << '\n';
		// std::cout << force1 + force2 << '\n';
	}

	window.close();
}
