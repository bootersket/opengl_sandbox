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
#include <iterator>
#include <sstream>
#include <math.h>
#include <filesystem>
#include <vector>
#include "shader.hpp"

// todo maybe not the cleanest layout of this info as it could get out of sync, but at least it's all in one place which is an improvement over having a bunch of magic numbers instead of named macros
#define TEX_UNIT__IMAGE GL_TEXTURE0
#define TEX_INDEX__IMAGE 0


#define TEX_UNIT__PRE_LUT GL_TEXTURE1
#define TEX_INDEX__PRE_LUT 1

#define TEX_UNIT__POST_LUT GL_TEXTURE2
#define TEX_INDEX__POST_LUT 2

#define validateLoc(loc) { if (loc == -1) std::cout << "ERROR: problem getting loc: " << #loc << std::endl; }

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


float g_gammaExponent = 1.0f;
glm::mat3 g_colorMatrix;

void updateColorMatrix();
void updateGammaExponent();
void updateBlackPointValue();
void computeLutAndLoadToTex(float);
std::string boolStr(bool);


void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

int imageIndex = 0;
int imageCount = 0;
bool g_applyCorrections = false;
bool g_useLut = false;
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
        g_useLut = !g_useLut;
        std::cout << "g_useLut: " << boolStr(g_useLut) << std::endl;
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

void writeLutDataToFile(std::vector<float> data, std::string filename, int elements, int components) {
    if (components != 3) {
        std::cout << "ERROR: writeLutDataToFile() does not support " << components << " components!!" << std::endl;
        return;
    }
    std::ofstream file(filename);
    for (int i=0; i<elements; i++) {
        float a, b, c;
        a = data[i*3 + 0];
        b = data[i*3 + 1];
        c = data[i*3 + 2];
        file << i << "=" << a << ", " << b << ", " << c << "\n";
    }
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
    std::vector<std::vector<float>> fileRows;
    while (getline(File, line)) {
        std::cout << line << std::endl;
        std::stringstream ss(line);
        ss >> a >> b >> c;
        std::vector<float> fileRow = {a, b, c};
        fileRows.push_back(fileRow);
        // col0 = {a, b, c};
        // glm::vec3 row = glm::vec3(a, b, c);
        // g_colorMatrix[matrixRow] = row;
        // matrixRow++;
    }
    if (fileRows.size() != 3 || fileRows[0].size() != 3) {
        std::cout << "ERROR: color matrix is not 3x3." << std::endl;
        return;
    }

    /* Gotta do some funky stuff because according to glm/glsl a matrix is columns first.
    So if I were to do mat[0] = glm::vec3(1, 2, 3) this would define the first column
    in the matrix. Because colorMatrix.txt is row based (since that's what's intuitive to me),
    I need to do this funky conversion so that the matrix values are given to opengl correctly.
    
    Essentially a matrix like:
    a b c
    d e f
    g h i
    if passed into a glm::mat like:
    mat[0] = glm::vec3(a, b, c)
    mat[1] = glm::vec3(d, e, f)
    mat[2] = glm::vec3(g, h, i)
    Would become:
    a d g
    b e h
    c f i

    So the below logic accounts for this.
    */
    for (int i=0; i<3; i++) {
        a = fileRows[0][i];
        b = fileRows[1][i];
        c = fileRows[2][i];
        glm::vec3 col = glm::vec3(a, b, c);
        g_colorMatrix[i] = col;
    }
    // while (getline(File, line)) {
    //     std::cout << line << std::endl;
    //     std::stringstream ss(line);
    //     ss >> a >> comma >> b >> comma >> c;
    //     glm::vec3 row = glm::vec3(a, b, c);
    //     g_colorMatrix[matrixRow] = row;
    //     matrixRow++;
    // }
}

std::string blackPointFilePath = "blackpointvalue.txt";
auto lastWrite_bp = std::filesystem::last_write_time(blackPointFilePath);
void checkForBlackPointUpdate() {
    auto currentWrite = std::filesystem::last_write_time(blackPointFilePath);
    if (currentWrite != lastWrite_bp) {
        lastWrite_bp = currentWrite;
        std::cout << "Updating black point value" << std::endl;
        updateBlackPointValue();
    }
}

float g_blackPointValue;
void updateBlackPointValue() {
    std::string line;
    std::ifstream File(blackPointFilePath);
    float value;
    while (getline(File, line)) {
        std::stringstream ss(line);
        ss >> value;
    }
    g_blackPointValue = value;
}


/* Checks if a file containing a scalar been changed. If it was,
it reads that file and re-computes the 1D LUT and loads the new
computed values to the texture */
std::string gammaExponentFilePath = "gammaexponent.txt";
auto lastWrite_gammaExponent = std::filesystem::last_write_time(gammaExponentFilePath);
void checkForGammaExponentUpdate() {
    auto currentWrite = std::filesystem::last_write_time(gammaExponentFilePath);
    if (currentWrite != lastWrite_gammaExponent) {
        lastWrite_gammaExponent = currentWrite;
        updateGammaExponent();
    }
}
/* Reads scalar value from file and passes it to function
that re-computes the 1D LUT and loads the new vlaues to
the texture */
void updateGammaExponent() {
    std::string line;
    std::ifstream File(gammaExponentFilePath);
    float value;
    while (getline(File, line)) {
        std::stringstream ss(line);
        ss >> value;
    }
    computeLutAndLoadToTex(value);
    g_gammaExponent = value;
}

float calcPreGammaValue_sRGB(float normalizedIn, float outMax) {
    float preOut;
    if (normalizedIn <= 0.04045) preOut = normalizedIn/12.92;
    else preOut = (pow((normalizedIn+.055)/1.055, 2.4));
    preOut *= outMax;
    return preOut;
}
float calcPostGammaValue_sRGB(float normalizedIn, float outMax) {
    float postOut;
    if (normalizedIn <= .0031308) postOut = normalizedIn * 12.92;
    else postOut = 1.055*pow(normalizedIn, 1/2.4) - 0.055;
    postOut *= outMax;
    return postOut;
}

/*
SRGB8 is a custom curve made by Ryan Eakin. I don't yet fully understand the underlying
reason but here's what he said about it:

    See the sRG8 tab on the gamma curve generator spreadsheet below.  sRGB is commonly
    used for the B4 gamma table and it approximates a gamma of 2.2.  By default, the
    After gamma table is always the inverse of the B4 table.  But sRGB has a compatibility
    problem with the 10-bit gamma tables - it results in rounding errors in the smallest
    RGB values (lowest 36 8-bit values).  This results in visible banding in gradients on
    the LCD.  So I invented a new curve that I call sRGB8.  It uses linear stepping for the
    lowest 36 values, then switches to gamma 2.2 above that.

*/
float calcPreGammaValue_sRGB8(float normalizedIn, float outMax) {
    float preOut;
    if (normalizedIn < .140762) preOut = normalizedIn/4;
    else preOut = pow((normalizedIn+.278)/1.278, 3);
    preOut *= outMax;
    return preOut;

}
float calcPostGammaValue_sRGB8(float normalizedIn, float outMax) {
    float postOut;
    if (normalizedIn < .036168) postOut = normalizedIn * 4;
    else postOut = pow(1.278*normalizedIn, 1/3)-.278;
    postOut *= outMax;
    return postOut;
}


GLuint g_preLutTex;
GLuint g_postLutTex;
void computeLutAndLoadToTex(float gammaExponent) {
    /* Precision are in bits */
    int inputPrecision = 10;
    int outputPrecision = 16;
    float inMax = (1 << inputPrecision) - 1;
    float outMax = (1 << outputPrecision) - 1;
    std::cout << "inMax: " << inMax << std::endl;
    std::cout << "outMax: " << outMax << std::endl;

    int numOfElements = 1 << inputPrecision;
    int numOfComponents = 3; // Components, channels, etc. Same thing. Meaning R G B, etc.

    int textureWidth = numOfElements;

    /* Create LUT texture */
    std::vector<float> preLutData(numOfElements * numOfComponents);
    std::vector<float> postLutData(numOfElements * numOfComponents);
    for (int i=0; i<numOfElements; i++) {
        // float in = i / 255.0f; /* LUT output values should be based on normalized inputs */

        // /* Example curve */
        // float preOut = pow(in, gammaExponent);
        // float postOut = pow(in, 1.0f/gammaExponent);

        // float in = i / 255.0f; /* LUT output values should be based on normalized inputs */
        // float in = i / 1023.0f;
        float in = i;

        /* Calc "before" matrix */
        float normalizedIn = i / inMax;



        /* Code to match values calculated in "gamma curve generator - temp.xlsx" */
        // float preOut = pow(in, gammaExponent);
        float preOut;
        // preOut = calcPreGammaValue_sRGB(normalizedIn, outMax);
        preOut = calcPreGammaValue_sRGB8(normalizedIn, outMax);

        float postOut;
        // postOut = calcPostGammaValue_sRGB(normalizedIn, outMax);
        postOut = calcPostGammaValue_sRGB8(normalizedIn, outMax);


        preLutData[i*3 + 0] = preOut;
        preLutData[i*3 + 1] = preOut;
        preLutData[i*3 + 2] = preOut;

        postLutData[i*3 + 0] = postOut;
        postLutData[i*3 + 1] = postOut;
        postLutData[i*3 + 2] = postOut;

        std::cout << "\nindex=" << i << std::endl;
        std::cout << "s=" << normalizedIn << std::endl;
        std::cout << "16-bit: " << preOut << std::endl;

    }
    /* Write data to file for debugging/understanding what values are being computed */
    writeLutDataToFile(preLutData, "preLut.txt", numOfElements, numOfComponents);
    writeLutDataToFile(postLutData, "postLut.txt", numOfElements, numOfComponents);

    glBindTexture(GL_TEXTURE_2D, g_preLutTex);
             /*  target         level  internalFormat   width            height  border    format  type      data    */
    glTexImage2D(GL_TEXTURE_2D, 0,     GL_RGB32F,       textureWidth,    1,      0,        GL_RGB, GL_FLOAT, preLutData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, g_postLutTex);
             /*  target         level  internalFormat   width            height  border    format  type      data    */
    glTexImage2D(GL_TEXTURE_2D, 0,     GL_RGB32F,       textureWidth,    1,      0,        GL_RGB, GL_FLOAT, postLutData.data());
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
    for (int i=0; i<numOfElements; i++) {
        float in = i / 255.0f; /* LUT output values should be based on normalized inputs */

        /* Example curve */
        float out = pow(in, 1.0f);

        lutData[i*3 + 0] = out;
        lutData[i*3 + 1] = out;
        lutData[i*3 + 2] = out;
    }
    /* Write to file for debugging */
    writeLutDataToFile(lutData, "defaultLut.txt", numOfElements, numOfComponents);

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


unsigned int createImageTexture(std::string filename) {
    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int texImageWidth, texImageHeight, numOfChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filename.c_str(), &texImageWidth, &texImageHeight, &numOfChannels, 4);
    GLenum texFormat = GL_RGBA;
    
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
    std::vector<std::string> refImageNames = {
        "color_test_ref.png",
        "greyramp.png",
        "greyscale_samples.png",
        "patch_lightgrey.png",
        "patch_darkgrey.png",
        "patch_red.png",
        "patch_green.png",
        "patch_blue.png",
        "patch_purple.png",
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
        "greyramp_miscal.png",
        "greyscale_samples_miscal.png",
        "patch_lightgrey_miscal.png",
        "patch_darkgrey_miscal.png",
        "patch_red_miscal.png",
        "patch_green_miscal.png",
        "patch_blue_miscal.png",
        "patch_purple_miscal.png",
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
    if (refImageNames.size() != miscalImageNames.size()) {
        std::cout << "ERROR: refImageNames and miscalImageNames do not have the same number of elements" << std::endl;
        return -1;
    }

    std::vector<TextureFileMapping> miscalTextureFileMappings;

    imageCount = refImageNames.size();

    std::vector<unsigned int> refTextures;
    std::vector<unsigned int> miscalTextures;

    std::string imageDir = "C:/Users/gregurichcoli/color_sandbox/batch";
    for (int i=0; i<refImageNames.size(); i++) {
        std::string refFullPath = imageDir + "/" + refImageNames[i];
        std::string miscalFullPath = imageDir + "/" + miscalImageNames[i];
        std::cout << "making tex for '" << refImageNames[i] << std::endl;
        GLuint refTex = createImageTexture(refFullPath);
        std::cout << "making tex for '" << miscalImageNames[i] << std::endl;
        GLuint miscalTex = createImageTexture(miscalFullPath);
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
    GLuint tex_ccOn = createImageTexture("C:/Users/gregurichcoli/graphics-learning/opengl_sandbox/color_correction_on.png");
    GLuint tex_ccOff = createImageTexture("C:/Users/gregurichcoli/graphics-learning/opengl_sandbox/color_correction_off.png");



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
    glm::mat4 ref_model = glm::mat4(1.0f);
    ref_model = glm::translate(ref_model, glm::vec3(0.0f, transY, 0.0f));
    ref_model = glm::scale(ref_model, glm::vec3(scaleFactor, scaleFactor, 1.0f));

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
    unsigned int uTex_loc = glGetUniformLocation(shader.programID, "uTex");
    unsigned int preLut_loc = glGetUniformLocation(shader.programID, "preLut");
    unsigned int postLut_loc = glGetUniformLocation(shader.programID, "postLut");
    unsigned int useLut_loc = glGetUniformLocation(shader.programID, "useLut");
    unsigned int gammaExponent_loc = glGetUniformLocation(shader.programID, "gammaExponent");
    unsigned int blackPointValue_loc = glGetUniformLocation(shader.programID, "blackPointValue");
    validateLoc(model_loc);
    validateLoc(view_loc);
    validateLoc(projection_loc);
    validateLoc(uColorMatrix_loc);
    validateLoc(uTex_loc);
    validateLoc(preLut_loc);
    validateLoc(postLut_loc);
    validateLoc(useLut_loc);
    validateLoc(gammaExponent_loc);
    validateLoc(blackPointValue_loc);

    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

    shader_ref.use();
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));



    /* Generate tex here; it gets populated once before render loop 
    and then whenever lut.txt is written to */
    glGenTextures(1, &g_preLutTex);
    glGenTextures(1, &g_postLutTex);

    GLuint defaultLUTTex = createDefaultLutTex();
    glm::mat3 defaultColorMatrix(1.0f);

    std::string colorMatrixFilePath = "colorMatrix.txt";
    /* Need these initial calls so the respective variables are populated
    with values from the .txt files */
    updateColorMatrix();
    updateGammaExponent();
    updateBlackPointValue();

    while (!glfwWindowShouldClose(window)) {
        /* These funcs check for changes in .txt files that allow me
        to tinker with how the image is being altered in live time */
        checkForColorMatrixUpdate();
        checkForGammaExponentUpdate();
        checkForBlackPointUpdate();

        for (int i=0; i<miscalTextureFileMappings.size(); i++) {
            miscalTextureFileMappings[i].checkForUpdate();
        }


        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // ! technically this can be done just once in this case, but leaving it here so it's more visible as I work on understanding texture stuff
        glUniform1i(uTex_loc, TEX_INDEX__IMAGE);
        glUniform1i(preLut_loc, TEX_INDEX__PRE_LUT);
        glUniform1i(postLut_loc, TEX_INDEX__POST_LUT);


        glBindVertexArray(vao);

        /*====================
        DRAW REFERENCE IMAGE
        =====================*/
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, refTextures[imageIndex]);
        shader_ref.use();
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(ref_model));
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

        glUniform1i(useLut_loc, g_useLut);

        /* Apply different color matrix + LUT depending on if we want to display
        the miscalibrated image with corrections or without */
        if (g_applyCorrections) {
            /* Use matrix read from colorMatrix.txt */
            glUniformMatrix3fv(uColorMatrix_loc, 1, GL_FALSE, glm::value_ptr(g_colorMatrix));

            glUniform1f(gammaExponent_loc, g_gammaExponent);

            glUniform1f(blackPointValue_loc, g_blackPointValue);

            /* Pre LUT */
            glActiveTexture(TEX_UNIT__PRE_LUT);
            glBindTexture(GL_TEXTURE_2D, g_preLutTex);

            /* Post LUT */
            glActiveTexture(TEX_UNIT__POST_LUT);
            glBindTexture(GL_TEXTURE_2D, g_postLutTex);
        }
        else {
            /* Use identity matrix to cause no 3x3 correction effect */
            glUniformMatrix3fv(uColorMatrix_loc, 1, GL_FALSE, glm::value_ptr(defaultColorMatrix));

            glUniform1f(gammaExponent_loc, 1);
            glUniform1f(blackPointValue_loc, 0);

            /* Pre LUT */
            glActiveTexture(TEX_UNIT__PRE_LUT);
            glBindTexture(GL_TEXTURE_2D, defaultLUTTex);

            /* Post LUT */
            glActiveTexture(TEX_UNIT__POST_LUT);
            glBindTexture(GL_TEXTURE_2D, defaultLUTTex);
        }
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(altered_model));
        glDrawArrays(GL_TRIANGLES, 0, 6);


        
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
