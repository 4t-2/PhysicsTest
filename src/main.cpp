#include "../lib/AGL/agl.hpp"
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
			force = {0, 0};

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
		std::vector<Circle> circle;
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

		void addCircle(agl::Vec<float, 2> position, float radius, agl::Vec<float, 2> force)
		{
			circle.emplace_back(Circle(shape));
			circle[circle.size() - 1].position = position;
			circle[circle.size() - 1].radius   = radius;
			circle[circle.size() - 1].force = force;
		}

		void update()
		{
			// update position
			for (Circle &circle : circle)
			{
				circle.update();
				circle.force += agl::Vec<float, 2>{0, gravity} * circle.mass;
			}

			// collision detection
			for (Circle &circle : circle)
			{
				agl::Vec<float, 2> boundryPosition = {500, 500};
				float			   boundryRadius   = 500;

				agl::Vec<float, 2> offset = circle.position;
				offset					  = offset - boundryPosition;

				float distance = offset.length();

				float overlap = distance + circle.radius - boundryRadius;

				if (overlap > 0)
				{
					agl::Vec<float, 2> pushback = offset.normalized() * -overlap;

					circle.position += pushback;

					circle.force += (pushback * circle.mass);
				}
			}
		}

		void drawFunction(agl::RenderWindow &window) override
		{
			for (Circle &circle : circle)
			{
				window.draw(circle);
			}
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
	phySim.setGravity(1);

	phySim.addCircle({500, 900}, 100, {1000, 0});

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
	}

	window.close();
}
