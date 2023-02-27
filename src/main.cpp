#include "../lib/AGL/agl.hpp"

class Circle : public agl::Drawable
{
	private:
		agl::Vec<float, 2> position;
		agl::Vec<float, 2> velocity;
		agl::Vec<float, 2> acceleration;

		float radius = 100;

		agl::Circle &shape;

	public:
		Circle(agl::Circle &shape) : shape(shape){}

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

int main()
{
	agl::RenderWindow window;
	window.setup({1920, 1080}, "PhysicsTest");
	window.setFPS(60);
	window.setClearColor(agl::Color::Gray);

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

	agl::Circle shape(32);

	Circle circle(shape);
	circle.setVelocity({1, 0});

	circle.setPosition({1000, 1000});

	while (!event.windowClose())
	{
		event.poll([](XEvent xev) {});

		window.clear();

		window.draw(circle);

		window.display();

		circle.update();
	}

	window.close();
}
