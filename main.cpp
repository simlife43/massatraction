// Simulation mehrerer Massen mit Gravitationsanziehung (2D) in SDL2 
#include <SDL2/SDL.h>
#include <iostream>
#include <cxxopts.hpp>
#include "Game.h"

int main(int argc, char* argv[]) {

    cxxopts::Options options("massatraction", "A mass atraction simulation");

    options.add_options()
        ("n,number", "Number of mass", cxxopts::value<int>()->default_value("1000"))
        ("g,gravity", "Gravity constant", cxxopts::value<float>()->default_value("0.5"))
        ("r,reflexion", "switch reflexion on border on", cxxopts::value<bool>()->default_value("false"))
        ("i,histogramm", "show histogramm", cxxopts::value<bool>()->default_value("false"))
        ("a,addmassiv", "add a massive object in center (number is the mass of the object)", cxxopts::value<int>()->default_value("10"))
        ("m,merge", "switch on that mass will be merged", cxxopts::value<bool>()->default_value("false"))
        ("d,mindistance", "mindistance it should have to calculate",cxxopts::value<float>()->default_value("10"))
        ("h,help", "Show help");

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help();
        return 0;
    }

    Game game;

    game.world.bodies_count = result["number"].as<int>();
    game.world.G = result["gravity"].as<float>(); 
    game.world.reflexion_on = result["reflexion"].as<bool>();
    game.world.histogramm_on = result["histogramm"].as<bool>();
    game.world.massive_mass = result["addmassiv"].as<int>();
    game.world.mindistance = result["mindistance"].as<float>(); 
    game.world.mass_merge = result["merge"].as<bool>();

    std::cout<<"start with bodies count: "<<game.world.bodies_count<<std::endl;
    std::cout<<"min distance to calculate: "<<game.world.mindistance<<std::endl;
    if(game.world.mass_merge)
        std::cout<<"mass merge is activated"<<std::endl;
    if(game.world.histogramm_on)
        std::cout<<"draw histogramm is activated"<<std::endl;

    if (!game.init("massatraction")) {
        std::cerr<<"Errors on Game init()\n\n";
        return -1;
    }

    game.run();
    game.clean();
    return 0;
}
