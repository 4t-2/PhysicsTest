#include "../lib/AGL/agl.hpp"
#include <vector>

class Circle : public agl::Drawable
{
	private:
		agl::Vec<float, 2> position;
		agl::Vec<float, 2> velocity;
		agl::Vec<float, 2> acceleration;

		float radius = 1;

		agl::Circle &shape;

	public:
		Circle(agl::Circle &shape) : shape(shape)
		{
		}

		void setRadius(float radius)
		{
			this->radius = radius;
		}

		void setPosition(agl::Vec<float, 2> position)
		{
			this->position = position;
		}

		void setVelocity(agl::Vec<float, 2> velocity)
		{
			this->velocity = velocity;
		}

		void setAcceleration(agl::Vec<float, 2> acceleration)
		{
			this->acceleration = acceleration;
		}

		void update()
		{
			velocity = velocity + acceleration;
			position = position + velocity;
		}

		void drawFunction(agl::RenderWindow &window) override
		{
			shape.setPosition(position);
			shape.setSize({radius, radius});
			shape.setOffset({-radius / 2, -radius / 2});

			window.drawShape(shape);
		}
};

class PhySim : public agl::Drawable
{
	private:
		std::vector<Circle> circle;
		agl::Circle		   &shape;
		float gravity;

	public:
		PhySim(agl::Circle &shape) : shape(shape)
		{
		}

		void setGravity(float gravity)
		{
			this->gravity = gravity;
		}

		void addCircle(agl::Vec<float, 2> position, agl::Vec<float, 2> velocity, float radius)
		{
			circle.emplace_back(Circle(shape));
			circle[circle.size()-1].setPosition(position);
			circle[circle.size()-1].setVelocity(velocity);
			circle[circle.size()-1].setRadius(radius);
		}

		void update()
		{
			for (Circle &circle : circle)
			{
				circle.setAcceleration({0, gravity});
				circle.update();
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
	phySim.setGravity(0.1);

	phySim.addCircle({500, 500}, {3, -9}, 10);
	phySim.addCircle({400, 500}, {3, -3}, 10);

	while (!event.windowClose())
	{
		event.poll([](XEvent xev) {});

		window.clear();

		window.draw(phySim);

		window.display();

		phySim.update();
	}

	window.close();
}
