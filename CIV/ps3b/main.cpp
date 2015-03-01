#include "body.hpp"

int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    // ./NBody 157788000.0 25000.0 < planets.txt
    std::cout << "Usage: ./NBody [simulation time] [time step] < planets.txt\n";
    return -1;
  }

  // Get the simulation time / time step from the command line arguments
  std::string sim_time(argv[1]);
  std::string step_time(argv[2]);
  std::string::size_type sz;     // alias of size_t

  std::cout << "Simulation time: " << sim_time << "\n";
  std::cout << "Time Step: " << step_time << "\n\n";

  // Convert these strings to doubles
  double simulation_time = 0;
  double simu_time = std::stod(sim_time, &sz);
  double time_step = std::stod(step_time, &sz);

  // Get the first two numbers in the text file. The first should be an int telling us
  // how many planets there are. The second should be a double telling us the radius of
  // the universe.
  std::string num_planets, radius;

  // Use cin to redirect the input
  std::cin >> num_planets;
  std::cin >> radius;

  // Now we know how many planets + the radius. Convert these from std::strings to int / float
  int number_planets = std::stoi(num_planets, &sz);
  double universe_radius = std::stod(radius, &sz);

  // Debugging
  std::cout << "Num of planets: " << number_planets << std::endl;
  std::cout << "Radius: " << universe_radius << std::endl << std::endl;

  // Create a vector of body objects
  std::vector<body> body_vector;

  // Loop through, create 5 (or how ever many we're asked for) body objects using the input file.
  for(int i = 0; i < number_planets; i++)
  {
    // Create a new object
    body* tmp = new body();

    // Read input into the object
    std::cin >> *tmp;

    // Set the radius and the planet positions.
    tmp->set_radius(universe_radius);
    tmp->set_position();

    // Save the object to the vector
    body_vector.push_back(*tmp);

    // Test the object (debugging)
    std::cout << *tmp;
  }

  // SFML Window
  sf::RenderWindow window(sf::VideoMode(window_side, window_height), "The Solar System");

  // Change the framerate to make it easier to see the image moving.
  window.setFramerateLimit(60);

  // Background image
  sf::Image background_image;

  // Background image
  if (!background_image.loadFromFile("stars.jpg"))
  {
    return -1;    // Quit if the file doesn't exist.
  }

  // Declare and load a font
  sf::Font time_font;
  time_font.loadFromFile("arial.ttf");

  sf::Text time_text;

  // Select the font
  time_text.setFont(time_font); // font is a sf::Font

  // Set the character size
  time_text.setCharacterSize(14);      // in pixels, not points!

  // Set the color
  time_text.setColor(sf::Color::White);

  // Load the music file
  sf::Music music;
  if(!music.openFromFile("2001.ogg"))
  {
    return -1;    // error
  }
//   music.play();

  // Load the image into a texture
  sf::Texture background_texture;
  background_texture.loadFromImage(background_image);

  // Load the texture into a sprite
  sf::Sprite background_sprite;
  background_sprite.setTexture(background_texture);

  // Set the position to make the background look cool
  background_sprite.setPosition(sf::Vector2f(-700, -700));

  // Calculate the forces on the objects
  std::vector<body>::iterator it;
  std::vector<body>::iterator x, y;
//   for(it = body_vector.begin(); it != body_vector.end(); it++)
//   {
//     it->find_force(*it, *it);
//
//   }

  // Window loop
  while (window.isOpen())
  {
    // Process events
    sf::Event event;

    while(window.pollEvent(event))
    {
      // Close window : exit
      if (event.type == sf::Event::Closed)
      {
        window.close();
      }

      // Pressing escape will quit the program.
      else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
      {
        window.close();
      }
    }

    window.clear();

    // Draws the starry background (black backgrounds are so lame for a solar system)
    window.draw(background_sprite);

    // Update the time string - I cast to an int to keep the time sane looking
    time_text.setString("Elapsed time: " + std::to_string((int)simulation_time));

    // Display the time in the left hand corner of the window
    window.draw(time_text);

    x = body_vector.begin();
    double force_x, force_y;

    for(int a = 0; a < number_planets; a++)
    {
      y = body_vector.begin();
      force_x = 0;
      force_y = 0;

      for(int b = 0; b < number_planets; b++)
      {
        if(a != b)
        {
          force_x += find_forcex(*x, *y);
          force_y += find_forcey(*x, *y);
        }
        y++;
      }
      x->set_forces(force_x, force_y);
      x++;
    }
//     x = body_vector.begin();
//     y = x++;
//     x->find_force(*x, *y);
//     y->find_force(*y, *x);

    // Display the vector of objects
    for(it = body_vector.begin(); it != body_vector.end(); it++)
    {
      window.draw(*it);
      std::cout << *it << std::endl;
      it->set_position();
      it->step(time_step);
    }

    window.display();

    // Increase simulation time variable by the simulation step
    simulation_time += time_step;

    // Stop when we've reached the simulation time
    if(simulation_time == simu_time)
    {
      break;
    }
  }

  // For debugging to compare against princeton's data
  std::cout << "\n\n\n";
  for(it = body_vector.begin(); it != body_vector.end(); it++)
  {
    std::cout << *it << std::endl;
  }

  return 0;
}