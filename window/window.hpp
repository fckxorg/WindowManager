#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <bits/stdint-uintn.h>
#include <stdio.h>

#include <cstdint>
#include <list>
#include <memory>

#include "../data_classes/data_classes.hpp"
#include "../event/event.hpp"
#include "../subscription_manager/subscription_manager.hpp"
#include "../window_base/window_base.hpp"

#ifdef NCURSES_ENGINE
#include "../ncurses_engine/ncurses_engine.hpp"
#endif

#ifdef SFML_ENGINE
#include "../sfml_engine/sfml_engine.hpp"
#endif

enum DIRECTION { UP, DOWN };

class InterfaceClickable {
   public:
    void handle_mouse_button_event(Event* event);
    virtual void onMousePress(MouseButtonEvent* event) = 0;
    virtual void onMouseRelease(MouseButtonEvent* event) = 0;
    virtual ~InterfaceClickable();
};

class RootWindow : public Window {
   public:
    virtual void render();
    virtual void handle_event(Event* event);
};

class RenderWindow : public Window {
   protected:
    Position pos;
    Size size;
    float rotation;

   public:
    RenderWindow();

    virtual ~RenderWindow();

    RenderWindow(const RenderWindow& other) = delete;

    RenderWindow(Size size, Position pos);

    virtual void render();

    void set_pos(Position pos);

    virtual Position get_position() const;

    void set_rotation(const float rotation);

    float get_rotation() const;
};

class RectWindow : public RenderWindow {
   protected:
    Color color;
    virtual bool is_point_inside(Position point);

   public:
    RectWindow();
    ~RectWindow();
    RectWindow(Size size, Position pos, Color color);

    void set_color(Color color);

    Color get_color();

    virtual void render() override;
};

class TextWindow : public RenderWindow {
   private:
    Color bgcolor;
    Text text;

   public:
    TextWindow();
    TextWindow(Text text, Position pos, Color bgcolor);

    Text get_text() const;
    void set_text(Text text);

    void set_bgcolor(Color bgcolor);
    Color get_bgcolor() const;

    virtual void render() override;
};

class RectButton : public RectWindow, public InterfaceClickable {
   private:
    Color default_color;
    uint32_t value;

   public:
    RectButton();
    ~RectButton();

    virtual void render() override;
    RectButton(Size size, Position pos, Color color, uint32_t value = 0);

    virtual void handle_event(Event* event) override;

    bool check_boundaries(Position click_pos);

    virtual void onMousePress(MouseButtonEvent* event) override;
    virtual void onMouseRelease(MouseButtonEvent* event) override;
};

class Slider : public RectWindow, public InterfaceClickable {
   private:
    Color default_color;
    Position last_mouse_pos;
    bool pressed;
    uint16_t step;

    bool horizontal;
    uint16_t lower_bound;
    uint16_t upper_bound;

    bool check_boundaries(Position click_pos);


   public:
    Slider();
    ~Slider();
    Slider(Size size, Position pos, Color color, uint16_t lower_bound,
           uint16_t upper_bound, uint16_t step, bool horizontal = false);

    virtual void onMousePress(MouseButtonEvent* event);
    virtual void onMouseRelease(MouseButtonEvent* event);
    virtual void onMouseMove(MouseMoveEvent* event);

    void onButtonUp();
    void onButtonDown();

    virtual void handle_event(Event* event);
};

class Scrollbar : public RectWindow {
   private:
    bool horizontal;    
    float scroll_ratio;

   public:
    Window* slider_ptr;
    Scrollbar();
    ~Scrollbar();

    void handle_event(Event* event) override;
    Scrollbar(Size size, Position pos, Color color, uint16_t viewport_size, uint16_t scroll_block_size, uint16_t step, bool horizontal = false);
};


class ScrollableText : public RectWindow {
    private:
        Text text;  
        uint16_t whole_block_height;
        int16_t offset; 

        uint16_t get_nlines();

    public:
        
        ScrollableText(Size viewport_size, Position pos, Color bg_color, Text text);
        virtual void render() override;

        void handle_event(Event* event) override;

        void onButtonUp();
        void onButtonDown();
};

#endif
