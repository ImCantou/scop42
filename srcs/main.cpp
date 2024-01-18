#include "scop.hpp"

int main(int ac, char **av) {
	if (ac != 2) {
		std::cout << "usage: ./Scop42 [path to .obj file]." << std::endl;
		return (0);
	}    

    try {
		VulkanEngine app(av[1]);
		
		app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}