#include "stdafx.h"  
#include "Game.h"  



int main()  
{  
   sf::RenderWindow window(sf::VideoMode(1280, 720), "Dead Paradise", sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize);  
   sf::Clock deltaClock;  
   Renderer renderer(window);  

   init(window);  
   while (window.isOpen())  
   {  
       float deltaTime = deltaClock.restart().asSeconds();  

       sf::Event ev{};  
       while (window.pollEvent(ev))  
       {  
           if (ev.type == sf::Event::Closed)  
               window.close();  
           if (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Escape)  
               window.close();  
       }  
       window.setView(camera.getView(sf::Vector2f(window.getSize())));  
       Update(deltaTime);  
       window.clear();  
       Render(renderer);  
       window.display();  
   }  

   // Fin de la aplicación  
}
