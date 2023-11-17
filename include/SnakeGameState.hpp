//Header guards
#ifndef SNAKEGAMESTATE_HPP_INCLUDED
#define SNAKEGAMESTATE_HPP_INCLUDED

#define BOARD_WIDTH 20
#define BOARD_HEIGHT 20

#define TOP_MASK 0x1000
#define LEFT_MASK 0x0100
#define BOTTOM_MASK 0x0010
#define RIGHT_MASK 0x0001

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#include <memory>
#include <vector>
#include "polled_object.hpp"
#include "shader.hpp"


#include <GLFW/glfw3.h>
#include "context.hpp"

namespace opengles_workspace
{
    enum class TableElement
    {
        Empty = 0,
        Apple = 0x1111
    };

    class SnakeGameState: public PolledObject
    {  
    //Make private before pushing
    private:
        int mBoardWidth;
        int mBoardHeight;
        int mSnakeX;
        int mSnakeY;
        int mTailX;
        int mTailY;
        int mAppleX;
        int mAppleY;
        int mScore;
        int mDirectionX;
        int mDirectionY;
        bool mGameOver;
        bool mGameStarted;
        //The board representing the game state
        //The board will contain
        // 0x0000 for empty
        // 0x1111 for apple
        // and 0x1000 if the previous snake square is to the top
        // 0x0100 if the previous snake square is to the left
        // 0x0010 if the previous snake square is to the bottom
        // 0x0001 if the previous snake square is to the right
        int mBoard[BOARD_WIDTH][BOARD_HEIGHT];

        std::vector<unsigned int> mVAOs;
        std::vector<unsigned int> mTextures;
        //Row major
        // tranform(i, j) = mTranslations[i * mBoardWidth + j]
        std::vector<glm::mat4> mTranslations;
        std::shared_ptr<Shader> pShader;   
        std::shared_ptr<Context> pContext; 

    public:
        SnakeGameState(
            int boardWidth = BOARD_WIDTH, 
            int boardHeight = BOARD_HEIGHT,
            std::shared_ptr<Shader> pShader = nullptr,
            std::shared_ptr<Context> context = nullptr
        );
        void init();
        void update();
        void draw();
        void print();
        void moveSnake();
        GLFWwindow* window() const { return static_cast<GLFWwindow*>(pContext->window()); }
        bool poll() override;
        std::chrono::high_resolution_clock::time_point mTime;

        public:
            // Getter and setter functions for snake direction
            int getSnakeDirectionX() const { return mDirectionX; }
            int getSnakeDirectionY() const { return mDirectionY; }
            void setSnakeDirection(int x, int y) { mDirectionX = x; mDirectionY = y; }

            // Getter and setter functions for snake position
            int getSnakeX() const { return mSnakeX; }
            int getSnakeY() const { return mSnakeY; }
            void setSnakePosition(int x, int y) { mSnakeX = x; mSnakeY = y; }

            // Getter and setter functions for snake tail
            int getSnakeTailX() const { return mTailX; }
            int getSnakeTailY() const { return mTailY; }
            void setSnakeTail(int x, int y) { mTailX = x; mTailY = y; }

            int getBoard(int x, int y) const { return mBoard[x][y]; }
            void setBoard(int x, int y, int value) { mBoard[x][y] = value; }

        private:
            void generateApple();

        };
    };
    #endif // SNAKEGAMESTATE_HPP_INCLUDED
