STB_DIR = third_party/stb
GLAD_DIR = third_party/glad
GLM_DIR = third_party/glm
GLFW_INCLUDE_DIR = third_party/GLFW/include
GLFW_LIB_DIR = third_party/GLFW/lib-mingw-w64

THIRD_PARTY = third_party

#g++ src/main.cpp $(STB)/stb_image_impl.cpp $(GLAD)/glad.c -I$(GLAD_DIR) -I$(GLM_DIR) -lglfw -ldl -o build/main
all:
	g++ src/main.cpp $(STB_DIR)/stb_image_impl.cpp $(GLAD_DIR)/glad.c -I$(THIRD_PARTY) -I$(GLM_DIR) -I$(GLFW_INCLUDE_DIR) -L$(GLFW_LIB_DIR) -lglfw3dll -ldl -o build/main -static-libstdc++ -static-libgcc

run: all
	./build/main


clean:
	rm -f build/main


