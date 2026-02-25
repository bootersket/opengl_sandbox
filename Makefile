STB_DIR = third_party/stb
GLAD_DIR = third_party/glad
GLM_DIR = third_party/glm

THIRD_PARTY = third_party

main:
	g++ src/main.cpp $(STB_DIR)/stb_image_impl.cpp $(GLAD_DIR)/glad.c -I$(THIRD_PARTY) -I$(GLM_DIR) -lglfw -ldl -o build/main

run-main: main
	./build/main

brick:
	g++ src/brickbounce/brickbounce.cpp src/brickbounce/numbers.cpp src/brickbounce/mesh.cpp $(GLAD_DIR)/glad.c -I$(THIRD_PARTY) -lglfw -ldl -o build/brickbounce

sandbox:
	g++ src/sandbox/sandbox.cpp $(GLAD_DIR)/glad.c -I$(THIRD_PARTY) -lglfw -ldl -o build/sandbox

run-brick: brick
	./build/brickbounce

run-sandbox: sandbox
	./build/sandbox

clean:
	rm -f build/main


