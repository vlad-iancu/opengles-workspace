#include <glad/gl.h>
#include "SnakeGameState.hpp"
#include <iostream>
#include <chrono>
#include "stb_image.h"

#include <glm/gtx/string_cast.hpp>
#include "context.hpp"

using namespace opengles_workspace;

SnakeGameState::SnakeGameState(
    int boardWidth, 
    int boardHeight,
    std::shared_ptr<Shader> pShader,
    std::shared_ptr<Context> context
) : 
    mBoardWidth(boardWidth), 
    mBoardHeight(boardHeight),
    pShader(pShader),
    mGameStarted(true),
    mGameOver(false),
    pContext(context)
{
}

void SnakeGameState::init()
{
    mSnakeX = 0;
    mSnakeY = 0;
    mAppleX = mBoardWidth / 2;
    mAppleY = mBoardHeight / 2;
    mScore = 0;
    mDirectionX = 1;
    mDirectionY = 0;
    mGameOver = false;
    mGameStarted = false;
    for(int i = 0; i < mBoardHeight; i++)
    {
        for(int j = 0; j < mBoardWidth; j++)
        {
            mBoard[i][j] = 0;
        }
    }
    mBoard[mSnakeX][mSnakeY] = LEFT_MASK;
    mBoard[mAppleX][mAppleY] = 0x1111;

    float horizontalScaleFactor = 2.0f / mBoardWidth;
    float verticalScaleFactor = 2.0f / mBoardHeight;
    mTranslations = std::vector<glm::mat4>();
    mTranslations.reserve(mBoardWidth * mBoardHeight);

    for(int i = 0; i < mBoardHeight; ++i)
    {
        for(int j = 0; j < mBoardWidth; ++j)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-1.0f + horizontalScaleFactor / 2 + j * horizontalScaleFactor, 1.0f - verticalScaleFactor / 2 - i * verticalScaleFactor, 0.0f));
            model = glm::scale(model, glm::vec3(horizontalScaleFactor, verticalScaleFactor, 1.0f));

            mTranslations[i * mBoardWidth + j] = model;
        }
    }
    
    //Create Snake and Apple textures
    //Snake texture
    mTextures = std::vector<unsigned int>(2);
    glGenTextures(2, &mTextures[0]);
    glBindTexture(GL_TEXTURE_2D,  mTextures[0]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    //Flip on load
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("Snake.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    //Apple texture
    glBindTexture(GL_TEXTURE_2D,  mTextures[1]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load image, create texture and generate mipmaps
   
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    //Flip on load
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("Apple.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    //Create VAOs
    mVAOs = std::vector<unsigned int>(2);
    glGenVertexArrays(2, &mVAOs[0]);
    //Snake texture
    glBindVertexArray(mVAOs[0]);
    //Create VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    //Create EBO
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    //Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //Bind EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //Create vertices
    float vertices[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,    1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // bottom right
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,    0.0f, 0.0f, // bottom left
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,    0.0f, 1.0f  // top left 
    };
    //Create indices
    unsigned int indices[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };
    //Copy vertices to VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //Copy indices to EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    //Set vertex attribute pointers
    //Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    //Apple texture
    glBindVertexArray(mVAOs[1]);
    //Create VBO
    glGenBuffers(1, &VBO);
    //Create EBO
    glGenBuffers(1, &EBO);
    //Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //Bind EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //Same vertices as Snake texture
    //Copy vertices to VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //Copy indices to EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    //Set vertex attribute pointers
    //Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
}

void SnakeGameState::generateApple()
{
    int x = rand() % mBoardWidth;
    int y = rand() % mBoardHeight;
    while(mBoard[x][y] != (int)TableElement::Empty)
    {
        x = rand() % mBoardWidth;
        y = rand() % mBoardHeight;
    }
    mAppleX = x;
    mAppleY = y;
    mBoard[mAppleX][mAppleY] = 0x1111;
}

void SnakeGameState::update()
{
    if(mGameOver)
    {
        return;
    }
    if(!mGameStarted)
    {
        mGameStarted = true;
        return;
    }
    int nextX = mSnakeX + mDirectionX;
    int nextY = mSnakeY + mDirectionY;
    
    if(nextX < 0 || nextX >= mBoardWidth || nextY < 0 || nextY >= mBoardHeight)
    {
        mGameOver = true;
        std::cout << "Game over" << std::endl;
        return;
    }
    if(mBoard[nextX][nextY] != (int)TableElement::Empty && mBoard[nextX][nextY] != (int)TableElement::Apple)
    {
        std::cout << "Game over" << std::endl;
        mGameOver = true;
        return;
    }
    bool apple = false;
    
    if(mBoard[nextX][nextY] == (int)TableElement::Apple)
    {
        mScore++;
        apple = true;
    }
    if(mDirectionX == 1)
    {
        mBoard[nextX][nextY] = TOP_MASK;
    }
    else if(mDirectionX == -1)
    {
        mBoard[nextX][nextY] = BOTTOM_MASK;
    }
    else if(mDirectionY == 1)
    {
        mBoard[nextX][nextY] = LEFT_MASK;
    }
    else if(mDirectionY == -1)
    {
        mBoard[nextX][nextY] = RIGHT_MASK;
    }
    if(apple)
    {
        //now the snake has grown with one unit so we don't want to cut the tail
        generateApple();
    }
    else
    {
        //Move snake cuts the tail so that the snake moves
        moveSnake();
    }
    mSnakeX = nextX;
    mSnakeY = nextY;
    //mBoard[mSnakeX][mSnakeY] = mDirectionX | mDirectionY;
}

void SnakeGameState::moveSnake()
{
    //Look around the tail, find the square that leads to the tail
    //and then update the tail
    if(mTailX >= 1 && mBoard[mTailX - 1][mTailY] == BOTTOM_MASK)
    {
        mBoard[mTailX][mTailY] = 0;
        mTailX--;   
    }
    else if(mTailX < mBoardWidth - 1 && mBoard[mTailX + 1][mTailY] == TOP_MASK)
    {
        mBoard[mTailX][mTailY] = 0;
        mTailX++;
    }
    else if(mTailY >= 1 && mBoard[mTailX][mTailY - 1] == RIGHT_MASK)
    {
        mBoard[mTailX][mTailY] = 0;
        mTailY--;
    }
    else if(mTailY < mBoardHeight - 1 && mBoard[mTailX][mTailY + 1] == LEFT_MASK)
    {
        mBoard[mTailX][mTailY] = 0;
        mTailY++;
    }
    else
    {
        std::cout << "Error: Tail not found" << std::endl;
        std::cout << "TailX: " << mTailX << std::endl;
        std::cout << "TailY: " << mTailY << std::endl;
        std::cout << "SnakeX: " << mSnakeX << std::endl;
        std::cout << "SnakeY: " << mSnakeY << std::endl;
        std::cout << "DirectionX: " << mDirectionX << std::endl;
        std::cout << "DirectionY: " << mDirectionY << std::endl;
        std::cout << "Board: " << std::endl;
        print();
        getchar();
        getchar();
    }
    return;


}

void SnakeGameState::print()
{
    for(int i = 0; i < mBoardHeight; i++)
    {
        for(int j = 0; j < mBoardWidth; j++)
        {
            if(mBoard[i][j] == (int)TableElement::Apple)
            {
                std::cout << "A";
            }
            else if(mBoard[i][j] == (int)TableElement::Empty)
            {
                std::cout << "0";
            }
            else if(mBoard[i][j] == LEFT_MASK)
            {
                std::cout << "S";
            }
            else if(mBoard[i][j] == BOTTOM_MASK)
            {
                std::cout << "S";
            }
            else if(mBoard[i][j] == RIGHT_MASK)
            {
                std::cout << "S";
            }
            else if(mBoard[i][j] == TOP_MASK)
            {
                std::cout << "S";
            }
            else
            {
                std::cout << "X";
            }
            std::cout << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

bool SnakeGameState::poll()
{
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - mTime).count();
    if(millis >= 500)
    {
        mTime = t1;
        update();
    }
    draw();
    return true;
}

void SnakeGameState::draw()
{
    // VAOs[0] = Snake texture
    // VAOs[1] = Apple texture
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    for(int i = 0; i < BOARD_HEIGHT; ++i)
    {
        for(int j = 0; j < BOARD_WIDTH; ++j)
        {
            //std::cout << "Drawing square (" << i << ", " << j << ")" << std::endl;
            pShader->use();
            glUniformMatrix4fv(glGetUniformLocation(pShader->ID, "uModel"), 1, GL_FALSE, glm::value_ptr(mTranslations[i * mBoardWidth + j]));
            if(mBoard[i][j] == (int)TableElement::Apple)
            {
                //std::cout << "Drawing apple" << std::endl;
                glBindVertexArray(mVAOs[1]);
                glBindTexture(GL_TEXTURE_2D, mTextures[1]);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
            else if(mBoard[i][j] != (int)TableElement::Empty)
            {
                //std::cout << "Drawing snake" << std::endl;
                glBindVertexArray(mVAOs[0]);
                glBindTexture(GL_TEXTURE_2D, mTextures[0]);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
            
            //std::cout << "Drew square (" << i << ", " << j << ")" << std::endl;
        }
    }
    glfwSwapBuffers(window());
}

