#include <algorithm>
#include <array>
#include <thread>
#include <vector>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>

using namespace sf;

class Visualizer
{
private:
	enum class Direction
	{
		down,
		left,
		right,
		up
	};

	struct Particle
	{
		Particle() = default;
		Particle(int coord_1, int coord_2, Direction dir) : x(coord_1), y(coord_2), direction(dir) {}
		int x;
		int y;
		Direction direction;
	};

	void move()
	{
		std::for_each(std::begin(particles), std::end(particles), [this](auto& particle) {step(particle); });
	}

	void step(Particle& particle)
	{
		switch (particle.direction)
			//endless screen
		{
		case Direction::left:
			if (particle.x == 0)
			{
				particle.x = 999;
			}
			else
			{
				--particle.x;
			}
			break;
		case Direction::right:
			if (particle.x == 999)
			{
				particle.x = 1;
			}
			else
			{
				++particle.x;
			}
			break;
		case Direction::up:
			if (particle.y == 0)
			{
				particle.y = 999;
			}
			else
			{
				--particle.y;
			}
			break;
		case Direction::down:
			if (particle.y == 999)
			{
				particle.y = 1;
			}
			else
			{
				++particle.y;
			}
			break;
		default:
			break;
		}
		if (m_bd(m_mt))
		{
			particle.direction = static_cast<Direction>(m_uid(m_mt));
		}
	}

	void draw()
	{
		for (auto& column : m_field)
		{
			for (auto& cell : column)
			{
				cell = 0;
			}
		}
		std::for_each(std::begin(particles), std::end(particles), [this](const Particle& particle)
			{
				++m_field[particle.x / squares_size][particle.y / squares_size];
			});
		RectangleShape big_rect(Vector2f(squares_size, squares_size));
		for (auto i = 0; i < m_squares_in_column; ++i)
		{
			for (auto j = 0; j < m_squares_in_column; ++j)
			{
				Color color;
				const std::size_t particles_num = m_field[i][j];
				if (!particles_num)
				{
					color = Color::White;
				}
				else
				{
					auto saturation = particles_num / change_num;
					saturation = (saturation > colors_num - 1 ? colors_num - 1 : saturation);
					color = m_colors[saturation];
				}
				big_rect.setPosition(static_cast <float> (i * squares_size), static_cast <float> (j * squares_size));
				big_rect.setFillColor(color);
				m_window.draw(big_rect);
			}
		}
		RectangleShape small_rect(Vector2f(1.0f, 1.0f));
		std::for_each(std::begin(particles), std::end(particles), [this, &small_rect](const Particle& particle)
			{
				small_rect.setPosition(static_cast<float> (particle.x), static_cast<float>(particle.y));
				small_rect.setFillColor(m_particle_color);
				m_window.draw(small_rect);
			});
		m_window.display();
	}
	
	//squares & window
	const std::string m_window_name = "Brownian motion";
	std::mt19937_64 m_mt;
	std::uniform_int_distribution<> m_uid;
	std::bernoulli_distribution m_bd;
	inline static const std::size_t m_width = 1000;
	inline static const std::size_t m_height = 1000;
	inline static const std::size_t m_particles_num = 10000;
	std::vector<Particle> particles;
	std::vector<std::vector<std::size_t>> m_field;
	RenderWindow m_window;
	inline static const std::size_t colors_num = 10;
	const std::array<Color, colors_num> m_colors =
	{ Color(0, 0, 200), Color(0, 0, 180), Color(0, 0, 160), Color(0, 0, 140), Color(0, 0, 120), Color(0, 0, 100), Color(0, 0, 80), Color(0, 0, 60), Color(0, 0, 40), Color(0, 0, 20) };
	inline static const std::size_t change_num = 5;
	const Color m_particle_color = { 255, 255, 255 };
	inline static const std::size_t m_squares_in_column = 20;
	inline static const std::size_t squares_size = 50;

public:
	Visualizer() :
		m_window(VideoMode(m_width, m_height), m_window_name),
		m_uid(0, 3), m_bd(0.1),
		m_field(m_squares_in_column, std::vector<std::size_t>(m_squares_in_column, 0))
	{
		std::uniform_int_distribution uid(0, static_cast<int>(m_width) - 1);
		particles.reserve(m_particles_num);
		for (auto i = 0; i < m_particles_num; ++i)
		{
			auto x = uid(m_mt);
			auto y = uid(m_mt);
			auto dir = static_cast<Direction>(m_uid(m_mt));
			++m_field[x / squares_size][y / squares_size];
			particles.emplace(std::end(particles), Particle(x, y, dir));
		}
	};

	~Visualizer() noexcept = default;

	void start_motion()
	{
		while (m_window.isOpen())
		{
			Event event;
			while (m_window.pollEvent(event))
			{
				if (event.type == Event::Closed)
				{
					m_window.close();
				}
			}
			move();
			m_window.clear();
			draw();
		}
	}
	
};

int main()
{
	Visualizer visualizer;
	visualizer.start_motion();
	system("pause");
	return 0;
}