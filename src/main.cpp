#include <bn_core.h>
#include <bn_display.h>
#include <bn_log.h>
#include <bn_keypad.h>
#include <bn_random.h>
#include <bn_rect.h>
#include <bn_sprite_ptr.h>
#include <bn_sprite_text_generator.h>
#include <bn_size.h>
#include <bn_string.h>
#include <bn_backdrop.h>

#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_square.h"
#include "common_fixed_8x16_font.h"

//Starting position for player and dot
static constexpr int START_X = 5;
static constexpr int START_Y = 10;

// Width and height of the the player and treasure bounding boxes
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size TREASURE_SIZE = {8, 8};

// Full bounds of the screen
static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

// Number of characters required to show the longest numer possible in an int (-2147483647)
static constexpr int MAX_SCORE_CHARS = 11;

// Score location
static constexpr int SCORE_X = 70;
static constexpr int SCORE_Y = -70;

int main()
{
    bn::core::init();
    
    bn::backdrop::set_color(bn::color(15,5,15));

    bn::random rng = bn::random();

    // Will hold the sprites for the score
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites = {};
    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

    int score = 0;
    int boosts = 3;
    // Pixels / Frame player moves at
    int speed = 1;

    static constexpr int P_START_X = 20;
    static constexpr int P_START_Y = -50;

    static constexpr int DOT_START_X = 50;
    static constexpr int DOT_START_Y = 50;

    bn::sprite_ptr player = bn::sprite_items::square.create_sprite(P_START_X, P_START_Y);
    bn::sprite_ptr treasure = bn::sprite_items::dot.create_sprite(DOT_START_X, DOT_START_Y);


    while (true)
    {
        //Player Looping
        bn::fixed x = player.x();
        bn::fixed y =  player.y();

        if (x > MAX_X) {
            x = MIN_X;
        }
        else if (x < MIN_X) {
            x = MAX_X;
        }
        if (y > MAX_Y) {
            y = MIN_Y;
        }
        else if (y < MIN_Y) {
            y = MAX_Y;
        }

        player.set_x(x);
        player.set_y(y);

        // Move player with d-pad
        if (bn::keypad::left_held())
        {
            player.set_x(player.x() - speed);
        }
        if (bn::keypad::right_held())
        {
            player.set_x(player.x() + speed);
        }
        if (bn::keypad::up_held())
        {
            player.set_y(player.y() - speed);
        }
        if (bn::keypad::down_held())
        {
            player.set_y(player.y() + speed);
        }

        //reset position and score
        if (bn::keypad::start_pressed()) {
            
            score = 0;
            player.set_position(P_START_X, P_START_Y);
            treasure.set_position(DOT_START_X, DOT_START_Y);
        }

        if (bn::keypad::a_pressed()) {
            if(boosts >= 1) {
                speed = 5;
                boosts = boosts - 1; 
            }
        }

        // The bounding boxes of the player and treasure, snapped to integer pixels
        bn::rect player_rect = bn::rect(player.x().round_integer(),
                                        player.y().round_integer(),
                                        PLAYER_SIZE.width(),
                                        PLAYER_SIZE.height());
        bn::rect treasure_rect = bn::rect(treasure.x().round_integer(),
                                          treasure.y().round_integer(),
                                          TREASURE_SIZE.width(),
                                          TREASURE_SIZE.height());

        // If the bounding boxes overlap, set the treasure to a new location an increase score
        if (player_rect.intersects(treasure_rect))
        {
            // Jump to any random point in the screen
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            treasure.set_position(new_x, new_y);

            score++;
        }

        // Update score display
        bn::string<MAX_SCORE_CHARS> score_string = bn::to_string<MAX_SCORE_CHARS>(score);
        score_sprites.clear();
        text_generator.generate(SCORE_X, SCORE_Y,
                                score_string,
                                score_sprites);

        // Update RNG seed every frame so we don't get the same sequence of positions every time
        rng.update();

        bn::core::update();
    }
}