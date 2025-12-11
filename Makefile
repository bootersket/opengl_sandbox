STB_DIR = third_party/stb
GLAD_DIR = third_party/glad
GLM_DIR = third_party/glm

THIRD_PARTY = third_party

#g++ src/main.cpp $(STB)/stb_image_impl.cpp $(GLAD)/glad.c -I$(GLAD_DIR) -I$(GLM_DIR) -lglfw -ldl -o build/main
all:
	g++ src/main.cpp src/UserInput.cpp $(STB_DIR)/stb_image_impl.cpp $(GLAD_DIR)/glad.c -I$(THIRD_PARTY) -I$(GLM_DIR) -lglfw -ldl -o build/main

run: all
	./build/main


clean:
	rm -f build/main




