/* Third party includes */
#include <glad/glad.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// #include <GLFW/glfw3.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <filesystem>
#include <vector>
#include "shader.hpp"

class TextureFileMapping {
    public:
        std::string filepath;
        GLuint tex;
        std::filesystem::file_time_type lastWrite;

    TextureFileMapping(std::string filepath, GLuint tex) {
        this->filepath = filepath;
        this->tex = tex;
        this->lastWrite = std::filesystem::last_write_time(filepath);
    }

    void checkForUpdate() {
        auto currentWrite = std::filesystem::last_write_time(filepath);

        if (currentWrite != lastWrite) {
            lastWrite = currentWrite;
            std::cout << "image was updated. updating texture data" << std::endl;
            updateTexData();
        }


    }

    void updateTexData() {
        int texImageWidth, texImageHeight, numOfChannels;
        stbi_set_flip_vertically_on_load(true); // redundant to do it here but harmless
        unsigned char *data = stbi_load(filepath.c_str(), &texImageWidth, &texImageHeight, &numOfChannels, 0);
        GLenum texFormat = GL_RGB;
        if (numOfChannels == 3) texFormat = GL_RGB;
        else if (numOfChannels == 4) texFormat = GL_RGBA;
        
        glBindTexture(GL_TEXTURE_2D, this->tex);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, texFormat, texImageWidth, texImageHeight, 0, texFormat, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Failed to load texture '" << filepath << "'" << std::endl;
        }
        stbi_image_free(data);

    }


};


float g_lutValue = 1.0f;
glm::mat3 g_colorMatrix;

void updateColorMatrix();
void updateLut();
void computeLutAndLoadToTex(float);
std::string boolStr(bool);


bool doColorCorrectionInLinear = false;
void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

int imageIndex = 0;
int imageCount = 0;
bool g_applyCorrections = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        std::cout << "q pressed" << std::endl;
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        g_applyCorrections = !g_applyCorrections;
        // std::cout << "apply corrections: " << boolStr(g_applyCorrections) << std::endl;
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        doColorCorrectionInLinear = !doColorCorrectionInLinear;
        std::cout << "do color correction in linear: " << boolStr(doColorCorrectionInLinear) << std::endl;
    }


    if ((key == GLFW_KEY_LEFT || key == GLFW_KEY_A) && action == GLFW_PRESS) {
        imageIndex--;
    }
    if ((key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) && action == GLFW_PRESS) {
        imageIndex++;
    }
    imageIndex = std::min(imageCount-1, imageIndex);
    imageIndex = std::max(0, imageIndex);


}

std::string boolStr(bool b) {
    if (b) return "true";
    else return "false";
}


/* Checks if a file containing a 3x3 matrix has been changed. If it was,
it reads that file and updates the global colorMatrix with the values inside the file */
std::string colorMatrixFilePath = "colorMatrix.txt";
auto lastWrite_colorMatrix = std::filesystem::last_write_time(colorMatrixFilePath);
void checkForColorMatrixUpdate() {
    auto currentWrite = std::filesystem::last_write_time(colorMatrixFilePath);
    if (currentWrite != lastWrite_colorMatrix) {
        lastWrite_colorMatrix = currentWrite;
        updateColorMatrix();
    }
}
void updateColorMatrix() {
    std::cout << "updateColorMatrix()" << std::endl;
    std::string line;
    std::ifstream File("colorMatrix.txt");
    float a, b, c;
    char comma;
    int matrixRow = 0;
    while (getline(File, line)) {
        std::cout << line << std::endl;
        std::stringstream ss(line);
        ss >> a >> comma >> b >> comma >> c;
        glm::vec3 row = glm::vec3(a, b, c);
        g_colorMatrix[matrixRow] = row;
        matrixRow++;
    }
}


/* Checks if a file containing a scalar been changed. If it was,
it reads that file and re-computes the 1D LUT and loads the new
computed values to the texture */
std::string lutFilePath = "lut.txt";
auto lastWrite_lut = std::filesystem::last_write_time(lutFilePath);
void checkForLutUpdate() {
    auto currentWrite = std::filesystem::last_write_time(lutFilePath);
    if (currentWrite != lastWrite_lut) {
        lastWrite_lut = currentWrite;
        updateLut();
    }
}
/* Reads scalar value from file and passes it to function
that re-computes the 1D LUT and loads the new vlaues to
the texture */
void updateLut() {
    std::cout << "update lut" << std::endl;
    std::string line;
    std::ifstream File(lutFilePath);
    float value;
    while (getline(File, line)) {
        std::stringstream ss(line);
        ss >> value;
    }
    computeLutAndLoadToTex(value);
    g_lutValue = value;
}

GLuint g_lutTex;
void computeLutAndLoadToTex(float value) {
    /* Precision are in bits */
    int inputPrecision = 8;
    int outputPrecision = 8;

    int numOfElements = 1 << inputPrecision;
    int numOfComponents = 3; // Components, channels, etc. Same thing. Meaning R G B, etc.

    /* Create LUT texture */
    std::vector<float> lutData(numOfElements * numOfComponents);
    // float lutData[numOfElements*numOfComponents];
    for (int i=0; i<numOfElements; i++) {
        // float x = i / 255.0f;
        float in = i;

        /* Example curve */
        float out = pow(in, value);
        // float out = in / ((1<<inputPrecision)-1) * ((1<<outputPrecision)-1);

        lutData[i*3 + 0] = out;
        lutData[i*3 + 1] = out;
        lutData[i*3 + 2] = out;

        std::cout << i << "=" << out << ", " << out << ", " << out << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, g_lutTex);
             /*  target         level  internalFormat   width   height  border    format  type      data    */
    glTexImage2D(GL_TEXTURE_2D, 0,     GL_RGB32F,       256,    1,      0,        GL_RGB, GL_FLOAT, lutData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

GLuint createDefaultLutTex() {
    /* Create a 1D LUT with the value of 1.
    This is to enable functionality of toggling the
    corrections on and off: when not applying corrections,
    we'll send this default LUT to the shader; when we are
    applying corrections, we'll send the computed LUT (which gets its
    value from lut.txt) */
    /* Precision are in bits */
    int inputPrecision = 8;
    int outputPrecision = 8;

    int numOfElements = 1 << inputPrecision;
    int numOfComponents = 3; // Components, channels, etc. Same thing. Meaning R G B, etc.

    /* Create LUT texture */
    std::vector<float> lutData(numOfElements * numOfComponents);

    GLuint defaultLUTTex;
    glGenTextures(1, &defaultLUTTex);

    /* Create LUT data */
    // float lutData[256*3];
    for (int i=0; i<numOfElements; i++) {
        // float x = i / 255.0f;
        float in = i;

        /* Example curve */
        float out = pow(in, 1.0f);

        lutData[i*3 + 0] = out;
        lutData[i*3 + 1] = out;
        lutData[i*3 + 2] = out;
    }
    glBindTexture(GL_TEXTURE_2D, defaultLUTTex);
             // todo: does width need to map to numOfElements? still confused about texture stuff
             /*  target         level  internalFormat   width   height  border    format  type      data    */
    glTexImage2D(GL_TEXTURE_2D, 0,     GL_RGB32F,       256,    1,      0,        GL_RGB, GL_FLOAT, lutData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return defaultLUTTex;
}


unsigned int createTexture(std::string filename) {
    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int texImageWidth, texImageHeight, numOfChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename.c_str(), &texImageWidth, &texImageHeight, &numOfChannels, 0);
    GLenum texFormat = GL_RGB;
    if (numOfChannels == 3) texFormat = GL_RGB;
    else if (numOfChannels == 4) texFormat = GL_RGBA;
    
    if (data) {
      glTexImage2D(GL_TEXTURE_2D, 0, texFormat, texImageWidth, texImageHeight, 0, texFormat, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
      std::cout << "Failed to load texture '" << filename << "'" << std::endl;
    }
    stbi_image_free(data);

    return tex;
}


int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return -1;
    }

    int WINDOW_WIDTH = 1200;
    int WINDOW_HEIGHT = 800;
    float ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
	
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    glfwSetKeyCallback(window, key_callback);


    /* Shader for reference image */
    Shader shader_ref("src/shaders/shader.vert", "src/shaders/shader_reference.frag");
    Shader shader("src/shaders/shader.vert", "src/shaders/shader.frag");

    /* TEXTURE STUFF */
    // unsigned int tex_ref = createTexture("gi_images/gi_0_ref.png"); /* Texture for reference GI275 image */
    // unsigned int tex_miscal = createTexture("gi_images/gi_0_miscal.png"); /* Texture for miscalibrated GI275 image */
    std::vector<std::string> refImageNames = {
        "color_test_ref.png",
        "landscape_ref.png",
        "gi0_ref.png",
        "gi1_ref.png",
        "gi2_ref.png",
        "gi3_ref.png",
        "gi4_ref.png",
        "gi5_ref.png",
        "gi6_ref.png",
        "gi7_ref.png",
        "gi8_ref.png",
        "gi9_ref.png",
    };
    std::vector<std::string> miscalImageNames = {
        "color_test_miscal.png",
        "landscape_miscal.png",
        "gi0_miscal.png",
        "gi1_miscal.png",
        "gi2_miscal.png",
        "gi3_miscal.png",
        "gi4_miscal.png",
        "gi5_miscal.png",
        "gi6_miscal.png",
        "gi7_miscal.png",
        "gi8_miscal.png",
        "gi9_miscal.png",
    };

    std::vector<TextureFileMapping> miscalTextureFileMappings;

    imageCount = refImageNames.size();

    std::vector<unsigned int> refTextures;
    std::vector<unsigned int> miscalTextures;

    std::string imageDir = "C:/Users/gregurichcoli/color_sandbox/batch";
    for (int i=0; i<refImageNames.size(); i++) {
        std::string refFullPath = imageDir + "/" + refImageNames[i];
        std::string miscalFullPath = imageDir + "/" + miscalImageNames[i];
        GLuint refTex = createTexture(refFullPath);
        GLuint miscalTex = createTexture(miscalFullPath);
        refTextures.push_back(refTex);
        miscalTextures.push_back(miscalTex);

        /* Create an object mapping the file to the texture so we can
        check for updates to the underlying file and update texture
        data accordingly (saves us from having to restart the program 
        when changing the effect on the miscal image) */
        TextureFileMapping texFile(miscalFullPath, miscalTex);
        miscalTextureFileMappings.push_back(texFile);
    }

    /* Set up textures for display status of color correction (hacky way to display text info on screen) */
    GLuint tex_ccOn = createTexture("C:/Users/gregurichcoli/graphics-learning/opengl_sandbox/color_correction_on.png");
    GLuint tex_ccOff = createTexture("C:/Users/gregurichcoli/graphics-learning/opengl_sandbox/color_correction_off.png");



    float vertices[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };



    GLuint vao;
    GLuint vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int pos_loc, color_loc, texCoords_loc;
    pos_loc = glGetAttribLocation(shader.programID, "aPos");
    texCoords_loc = glGetAttribLocation(shader.programID, "aTexCoords");
    std::cout << "pos_loc= " << pos_loc << std::endl;
    std::cout << "texCoords_loc= " << texCoords_loc << std::endl;

    glVertexAttribPointer(pos_loc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(texCoords_loc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(sizeof(float)*2));

    glEnableVertexAttribArray(pos_loc);
    glEnableVertexAttribArray(texCoords_loc);





    /* Basic MVP setup */
    glm::mat4 view = glm::mat4(1.0f);
    // glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::mat4 projection = glm::ortho(0.0f, 100.0f*ASPECT_RATIO, 0.0f, 100.0f);

    float scaleFactor = 50.0f;
    float transY = 25.0f;
    glm::mat4 orig_model = glm::mat4(1.0f);
    orig_model = glm::translate(orig_model, glm::vec3(0.0f, transY, 0.0f));
    orig_model = glm::scale(orig_model, glm::vec3(scaleFactor, scaleFactor, 1.0f));

    glm::mat4 altered_model = glm::mat4(1.0f);
    altered_model = glm::translate(altered_model, glm::vec3(55.0f, transY, 0.0f));
    altered_model = glm::scale(altered_model, glm::vec3(scaleFactor, scaleFactor, 1.0f));

    glm::mat4 ccIndicator_model = glm::mat4(1.0f);
    ccIndicator_model = glm::translate(ccIndicator_model, glm::vec3(0.0f, 0.0f, 0.0f));
    /* Using magic scale values here because the underlying image for this texture is
    not a square, but I'm re-using the vertices for the ref&miscal images that
    are square. So using weird scale values to give the color correction status indicator
    a less stretched look */
    ccIndicator_model = glm::scale(ccIndicator_model, glm::vec3(20.0f, 8.0f, 0.0f)); 


    shader.use();
    unsigned int model_loc = glGetUniformLocation(shader.programID, "model");
    unsigned int view_loc = glGetUniformLocation(shader.programID, "view");
    unsigned int projection_loc = glGetUniformLocation(shader.programID, "projection");
    unsigned int uColorMatrix_loc = glGetUniformLocation(shader.programID, "uColorMatrix");

    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

    shader_ref.use();
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));



    /* Generate tex here; it gets populated once before render loop 
    and then whenever lut.txt is written to */
    glGenTextures(1, &g_lutTex);
    updateLut();

    GLuint defaultLUTTex = createDefaultLutTex();
    glm::mat3 defaultColorMatrix(1.0f);

    std::string colorMatrixFilePath = "colorMatrix.txt";
    updateColorMatrix();
    while (!glfwWindowShouldClose(window)) {
        /* These funcs check for changes in .txt files that allow me
        to tinker with how the image is being altered in live time */
        checkForColorMatrixUpdate();
        checkForLutUpdate();
        /* Check for updates to texture images */
        for (int i=0; i<miscalTextureFileMappings.size(); i++) {
            miscalTextureFileMappings[i].checkForUpdate();
        }


        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform1i(glGetUniformLocation(shader.programID, "uTex"), 0);
        glUniform1i(glGetUniformLocation(shader.programID, "lut"), 1);

        glBindVertexArray(vao);

        /*====================
        DRAW REFERENCE IMAGE
        =====================*/
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, refTextures[imageIndex]);
        shader_ref.use();
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(orig_model));
        glDrawArrays(GL_TRIANGLES, 0, 6);


        /*====================
        DRAW COLOR CORRECTION STATUS INDICATOR
        =====================*/
        glActiveTexture(GL_TEXTURE0);
        if (g_applyCorrections) {
            glBindTexture(GL_TEXTURE_2D, tex_ccOn);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, tex_ccOff);
        }
        shader_ref.use();
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(ccIndicator_model));
        glDrawArrays(GL_TRIANGLES, 0, 6);


        /*====================
        DRAW ALTERED IMAGE
        =====================*/
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, miscalTextures[imageIndex]);
        shader.use();
        glUniformMatrix3fv(uColorMatrix_loc, 1, GL_FALSE, glm::value_ptr(g_colorMatrix));

        /* Apply different color matrix + LUT depending on if we want to display
        the miscalibrated image with corrections or without */
        if (g_applyCorrections) {
            /* Use matrix read from colorMatrix.txt */
            glUniformMatrix3fv(uColorMatrix_loc, 1, GL_FALSE, glm::value_ptr(g_colorMatrix));

            /* Use LUT computed from value in lut.txt */
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, g_lutTex);
        }
        else {
            /* Use identity matrix to cause no 3x3 correction effect */
            glUniformMatrix3fv(uColorMatrix_loc, 1, GL_FALSE, glm::value_ptr(defaultColorMatrix));

            /* Use LUT value of 1.0 to cause no 1D LUT effect */
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, defaultLUTTex);
        }
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(altered_model));
        glDrawArrays(GL_TRIANGLES, 0, 6);



        /*====================
        TEXT STUFF
        =====================*/

        
        /*====================
        RENDER LOOP STUFF
        =====================*/
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
