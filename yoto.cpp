#include "yoto.hpp"
//#include <time.h> 
//#include <list>

using namespace blit;

struct Player
{
    int x;
    int y;
    int score;
    int sscore;
    bool button;
    bool clouds;
};

struct Game
{
    short state;
    int balls;
    float ball_radius;
};

struct Ball
{
    bool is_ball;
    float x;
    float y;
    float dx;
    float dy;
};

struct Cloud
{
    bool is_cloud;
    int x;
    int y;
    float r;
    float dr;
};

Player p;
Game game;
Ball ball[100];
Cloud cloud[100];

Timer end_timer;

//start settings
void start()
{
    game.ball_radius = rand() %5 +1;
    game.balls = rand() %95 + 5;

    int width = 318 - (2 * game.ball_radius);
    int height = 238 - (2 * game.ball_radius);  

    for (int i=0; i<game.balls; i++)
    {
        ball[i].is_ball = true;
        ball[i].x = rand() %width + game.ball_radius + 1;
        ball[i].y = rand() %height + game.ball_radius + 1;
        ball[i].dx = (rand() %50 *.1f) -2;
        ball[i].dy = (rand() %50 *.1f) -2;
    }

    p.x = 159;
    p.y = 119;
    p.score = 0;
    p.sscore = int(game.balls * .9f);
    if (p.sscore == game.balls)
    {
        p.sscore = game.balls - 1;
    } 
    p.button = true;
    p.clouds = false;
}

void end_of_game(Timer &t)
{
    start();
}

void new_cloud(int x, int y)
{
    for (int i=0; i<100; i++)
    {
        if (cloud[i].is_cloud == false)
        {
            cloud[i].is_cloud = true;
            cloud[i].x = x;
            cloud[i].y = y;
            cloud[i].r = game.ball_radius;
            cloud[i].dr = .2f;
            break;
        }
    }
}

void update_clouds()
{
    p.clouds = false;
    for (int i=0; i<100; i++)
    {
        if (cloud[i].is_cloud)
        {
            p.clouds = true;
            cloud[i].r += cloud[i].dr;
            if (cloud[i].r >= 20)
            {
                cloud[i].dr = -cloud[i].dr;
            }
            else if (cloud[i].r <= 0)
            {
                cloud[i].is_cloud = false;
            }

            for (int ii=0; ii<game.balls; ii++)
            {
                if (ball[ii].is_ball)
                {
                    float dx = ball[ii].x - cloud[i].x;
                    float dy = ball[ii].y - cloud[i].y;
                    float res = sqrt((dx * dx) + (dy * dy));  
                    if (res < (game.ball_radius + cloud[i].r))
                    {
                        channels[0].trigger_attack();
                        ball[ii].is_ball = false;
                        new_cloud(ball[ii].x, ball[ii].y);
                        p.score++;
                    }
                }
            }        
        }
    }
    if (!p.clouds && !p.button && game.ball_radius > 0)
    {
        game.ball_radius -= .05f;
        if (game.ball_radius < 0)
        {
            for (int i=0; i<game.balls; i++)
            {
                ball[i].is_ball = false;
            }
            end_timer.start();
        }
    }
}


void update_balls()
{
    for (int i=0; i<game.balls; i++)
    {
        if (ball[i].is_ball)
        {
            ball[i].x += ball[i].dx;
            if (ball[i].x - game.ball_radius <= 0 || ball[i].x + game.ball_radius >= 319)
            {
                ball[i].dx = -ball[i].dx;
            }
            ball[i].y += ball[i].dy;
            if (ball[i].y - game.ball_radius <= 0 || ball[i].y + game.ball_radius >= 239)
            {
                ball[i].dy = -ball[i].dy;
            }
        }
    }
}

void player_control()
{
    if (buttons & Button::DPAD_RIGHT && p.x < 315)
    {
        p.x += 3;
    }
    else if (buttons & Button::DPAD_LEFT && p.x > 4)
    {
        p.x -= 3;
    }
    else if (buttons & Button::DPAD_DOWN && p.y < 235)
    {
        p.y += 3;
    }
    else if (buttons & Button::DPAD_UP && p.y > 4)
    {
        p.y -= 3;
    }
    if (buttons.pressed & Button::A)
    {
        p.button = false;
        new_cloud(p.x, p.y);
    }
}

// init()
void init() 
{
    //srand (time(NULL));

    set_screen_mode(ScreenMode::hires);

    end_timer.init(end_of_game, 750, 1);

    channels[0].waveforms = Waveform::TRIANGLE;
    channels[0].frequency = 400;
    channels[0].attack_ms = 5;
    channels[0].decay_ms = 100;
    channels[0].sustain = 0;
    channels[0].release_ms = 5;

    start();
}

// render(time)
void render(uint32_t time) 
{

    // clear the screen -- screen is a reference to the frame buffer and can be used to draw all things with the 32blit
    if (p.score >= p.sscore)
    {
        screen.pen = Pen(0, 240, 0);
    }
    else if (p.button || p.clouds)
    {
        screen.pen = Pen(75, 240, 240);
    }    
    else
    {
        screen.pen = Pen(255, 0, 0);
    }
    
    screen.clear();

    // draw some text at the top of the screen
    screen.alpha = 255;
    screen.mask = nullptr;
    screen.pen = Pen(0, 0, 0);
    screen.text(std::to_string(p.sscore) + "/" + std::to_string(p.score), minimal_font, Point(4, 4));

    screen.pen = Pen(150, 150, 150);
    for (int i=0; i<game.balls; i++)
    {
        if (ball[i].is_ball)
        {
            screen.circle(Point(ball[i].x + 3, ball[i].y + 4), game.ball_radius);
        }
    }

    screen.pen = Pen(255, 255, 255);
    for (int i=0; i<game.balls; i++)
    {
        if (ball[i].is_ball)
        {
            screen.circle(Point(ball[i].x, ball[i].y), game.ball_radius);
        }
    }

    screen.pen = Pen(150, 150, 150);
    for (int i=0; i<100; i++)
    {
        if (cloud[i].is_cloud)
        {
            screen.circle(Point(cloud[i].x + 3, cloud[i].y + 4), cloud[i].r);
        }
    }

    screen.pen = Pen(255, 255, 255);
    for (int i=0; i<100; i++)
    {
        if (cloud[i].is_cloud)
        {
            screen.circle(Point(cloud[i].x, cloud[i].y), cloud[i].r);
        }
    }

    if (p.button)
    {
        screen.pen = Pen(0, 0, 0);
        screen.line(Point(p.x - 2, p.y), Point(p.x + 2, p.y));
        screen.line(Point(p.x, p.y - 2), Point(p.x, p.y + 2));
    }
}

// update(time)
void update(uint32_t time) 
{
    update_balls();
    if (p.button)
    {
        player_control();
    }
    update_clouds();
}
