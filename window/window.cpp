#include "window.hpp"

/*--------------- WINDOW ---------------------*/

Window::Window() = default;
bool Window::is_opened() const {
    return opened;
}

void Window::set_event_mask(uint32_t mask) {
    event_mask = mask;
}

void Window::add_child_window(std::unique_ptr<Window>& child) {
    subwindows.push_back(std::move(child));
}

Window::~Window() = default;

void Window::handle_event(Event& event) {
    for(auto& subwindow : subwindows) {
        subwindow->handle_event(event);
    }
} 


/*----------- RENDER WINDOW ----------------*/

RenderWindow::RenderWindow() = default;
RenderWindow::~RenderWindow() = default;
RenderWindow::RenderWindow(Size size, Position pos) : size(size), pos(pos) {}

void RenderWindow::open() { opened = true; }
void RenderWindow::close() { opened = false; }

void RenderWindow::refresh() {
    
}

void RenderWindow::set_pos(Position pos) { this->pos = pos; }
Position RenderWindow::get_position() const { return pos; }

void RenderWindow::set_rotation(const float rotation) { this->rotation = rotation; } 
float RenderWindow::get_rotation() const { return rotation; }


/*---------- RECT WINDOW -------------------*/

RectWindow::RectWindow() = default;
RectWindow::~RectWindow() = default;
RectWindow::RectWindow(Size size, Position pos, Color color) : RenderWindow(size, pos), color(color) {}

void RectWindow::render() {
    if (!this->opened) {
        return;
    }
    
    Renderer::draw_rectangle(size, pos, color);

    for(auto& subwindow : subwindows) {
        subwindow->render();
    }
    
}

void RectWindow::set_color(Color color) {
    this->color = color;
}

Color RectWindow::get_color() {
    return color;
}

/* --------- RECT BUTTON -------------------*/
RectButton::RectButton() = default;
RectButton::~RectButton() = default;

void RectButton::render() {
    if(!this->opened) {
        return;
    }

    RectWindow::render();
    
    for(auto& subwindow : subwindows) {
        subwindow->render();
    }

}

bool RectButton::check_boundaries(Position click_pos) {
    if(click_pos.x < pos.x || click_pos.x > pos.x + size.width) return false;
    if(click_pos.y < pos.y || click_pos.y > pos.y + size.height) return false;

    return true;
}

void RectButton::onMousePress(Event& event) {
    Position click_position = event.mouse_button_event.pos;
    if(!check_boundaries(click_position)) return;

    Color curr_color = RectWindow::get_color();
    
    uint8_t color_r_pressed = std::min(curr_color.r - 50, 0);
    uint8_t color_g_pressed = std::min(curr_color.g - 50, 0);
    uint8_t color_b_pressed = std::min(curr_color.b - 50, 0);

    Color pressed_color = Color(color_r_pressed, color_g_pressed, color_b_pressed);
    this->color = pressed_color;
}

void RectButton::onMouseRelease(Event& event) {
    this->color = default_color;
}

void RectButton::handle_event(Event& event) {
    switch(event.type) {
        case MOUSE_BUTTON_PRESSED:
            RectButton::onMousePress(event);
            break;
        case MOUSE_BUTTON_RELEASED:
            RectButton::onMouseRelease(event);
            break;
    }

    for(auto& subwindow : subwindows) {
        subwindow->handle_event(event);
    }
}

/*--------------- TEXT WINDOW-----------------------------*/

TextWindow::TextWindow() = default;
TextWindow::TextWindow(Text text, Position pos, Color bgcolor) : text(text), bgcolor(bgcolor) {
    set_pos(pos);
}

Text TextWindow::get_text() const {
    return text;
}

void TextWindow::set_text(Text text) {
    this->text = text;
}

Color TextWindow::get_bgcolor() const {
    return bgcolor;
}

void TextWindow::set_bgcolor(Color bgcolor) {
    this->bgcolor = bgcolor;
}

void TextWindow::render() {
    Renderer::draw_text(text, pos, bgcolor);

    for(auto& subwindow : subwindows) {
        subwindow->render();
    }

}

