#ifndef __SNAKE_H__
#define __SNAKE_H__

#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

#include "winsys.h"
#include "cpoint.h"
#include "screen.h"

class CSnake : public CFramedWindow
{
private:
    vector <CPoint> snake;
    CPoint apple;
    int dir = KEY_RIGHT;
    unsigned int score = 0;
    bool has_died = false;
    bool pause = true;
    bool display_help = true;

    void reset()
    {
        dir = KEY_RIGHT;
        score = 0;
        has_died = false;
        pause = false;
        display_help = false;

        snake.clear();
        CPoint head = CPoint(rand() % (geom.size.x - 5) + 3, rand() % (geom.size.y - 3) + 1);
        snake.push_back(head);
        snake.push_back(CPoint(head.x - 1, head.y));
        snake.push_back(CPoint(head.x - 2, head.y));

        dropApple();
        paint();
    }

    void dropApple()
    {
        while(1)
        {
            bool reroll = false;
            apple = CPoint(rand() % (geom.size.x - 2) + 1, rand() % (geom.size.y - 2) + 1);
            for(unsigned int i = 0; i < snake.size(); ++i)
            {
                if(snake[i].y == apple.y && snake[i].x == apple.x)
                {
                    reroll = true;
                    break;
                }
            }
            if(reroll)
                continue;
            break;
        }
    }

    bool next_move()
    {
        if(pause)
            return true;

        CPoint tail = snake[snake.size() - 1];
        for(unsigned long i = snake.size() - 1; i > 0; --i)
            snake[i] = snake[i - 1];

        if(dir == KEY_DOWN)
            snake[0] += CPoint(0, 1);
        else if(dir == KEY_UP)
            snake[0] += CPoint(0, -1);
        else if(dir == KEY_RIGHT)
            snake[0] += CPoint(1, 0);
        else if(dir == KEY_LEFT)
            snake[0] += CPoint(-1, 0);

        for(unsigned int i = 1; i < snake.size(); ++i)
        {
            if(snake[0].x == snake[i].x && snake[0].y == snake[i].y)
                return false;
        }
        if(snake[0].x == 0)
            snake[0].x = geom.size.x - 2;
        if(snake[0].x == geom.size.x - 1)
            snake[0].x = 1;
        if(snake[0].y == 0)
            snake[0].y = geom.size.y - 2;
        if(snake[0].y == geom.size.y - 1)
            snake[0].y = 1;

        if(snake[0].y == apple.y && snake[0].x == apple.x)
        {
            dropApple();
            snake.push_back(tail);
            ++score;
        }

        return true;
    }

    void draw()
    {
        if(!pause && !next_move())
        {
            has_died = true;
            pause = true;
        }
        gotoyx(apple.y + geom.topleft.y, apple.x + geom.topleft.x);
        printc('O');
        for (unsigned int i = 1; i < snake.size(); i++)
        {
            gotoyx(snake[i].y + geom.topleft.y, snake[i].x + geom.topleft.x);
            printc('+');
        }
        gotoyx(snake[0].y + geom.topleft.y, snake[0].x + geom.topleft.x);
        printc('*');
    }

    void drawDead()
    {
        int x = geom.topleft.x, y = geom.topleft.y;
        gotoyx(y + 4, x + 5);
        printl("You bit yourself! Achieved score: %d", score);
    }

    void drawPause()
    {
        int x = geom.topleft.x, y = geom.topleft.y;
        gotoyx(y + 2, x + 3);
        printl("H/h - Open help");
        gotoyx(y + 3, x + 3);
        printl("P/p - Pause the game");
        gotoyx(y + 4, x + 3);
        printl("R/r - Restart the game");
        gotoyx(y + 5, x + 3);
        printl("Q/q - Quit");
        gotoyx(y + 6, x + 3);
        printl("Tab - Change window");
        gotoyx(y + 7, x + 3);
        printl("Arrows - Move snake (in play mode)");
        gotoyx(y + 8, x + 12);
        printl("Move window (in pause mode)");
    }

    void drawHelp()
    {
        int x = geom.topleft.x, y = geom.topleft.y;
        gotoyx(y + 4, x + 2);
        printl("- Move around and eat apples to grow");
        gotoyx(y + 5, x + 2);
        printl("- The more apples you eat, ");
        gotoyx(y + 6, x + 2);
        printl("    the faster the snake will become!");
        gotoyx(y + 7, x + 2);
        printl("- Be careful not to bit yourself!");
        gotoyx(y + 8, x + 2);
        printl("- Press 'P'/'p' or 'R'/'r' to begin,");
        gotoyx(y + 9, x + 2);
        printl("    or 'H'/'h' for help");
    }

public:
    CSnake(CRect r, char _c = ' ') : CFramedWindow(r, _c)
    {
        srand(time(NULL));
        reset();
        display_help = true;
        pause = true;
        draw();
    }

    bool handleEvent(int key)
    {
        if(!pause && key == ERR)
            key = dir;

        if(!has_died && tolower(key) == 'p')
        {
            pause = !pause;
            if(!pause)
                display_help = false;

            return true;
        }
        if(tolower(key) == 'h' && pause)
        {
            display_help = !display_help;
            return true;
        }
        if(pause && tolower(key) == 'q')
        {
            exit(0);
        }
        if(tolower(key) == 'r')
        {
            reset();
            return true;
        }
        if(!has_died && !pause && (key == KEY_UP || key == KEY_DOWN || key == KEY_LEFT || key == KEY_RIGHT))
        {
            if((key == KEY_UP && dir != KEY_DOWN) ||
                (key == KEY_DOWN && dir != KEY_UP) ||
                (key == KEY_LEFT && dir != KEY_RIGHT) ||
                (key == KEY_RIGHT && dir != KEY_LEFT))
                dir = key;
            return true;
        }
        if(key == '\t')
            pause = true;

        return CFramedWindow::handleEvent(key);
    }

    void paint()
    {
        CFramedWindow::paint();
        draw();
        if(!has_died)
        {
            gotoyx(geom.topleft.y - 1, geom.topleft.x);
            printl("| LEVEL: %d |", score);
            if(pause)
            {
                if(display_help)
                    drawHelp();
                else
                    drawPause();
            }
        }
        else
            drawDead();
        if(!pause)
            usleep((score < 14) ? ((400 - (score * 30)) * 1000) : (10 * 1000)); // speed up (30ms interval increments, up to 10ms)
    }
};

#endif
