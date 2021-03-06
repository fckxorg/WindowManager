#include "data_classes.hpp"

#include <bits/stdint-uintn.h>

#include <cstdio>

/*---------------- SIZE CLASS -------------------------------*/

Size::Size() = default;
Size::Size(int16_t width, int16_t height) : width(width), height(height) {}

#ifdef SFML_ENGINE
Size::Size(const sf::Vector2f& sfsize)
    : width(static_cast<int16_t>(sfsize.x)),
      height(static_cast<int16_t>(sfsize.y)) {}

Size::operator sf::Vector2f() const { return sf::Vector2f(width, height); }
#endif

/*---------------- COLOR CLASS --------------------------------*/

Color::Color() = default;
Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : r(r), g(g), b(b), a(a){};

Color Color::operator+(int delta) {
  uint8_t color_r = std::max(static_cast<int>(r) + delta, 0);
  uint8_t color_g = std::max(static_cast<int>(g) + delta, 0);
  uint8_t color_b = std::max(static_cast<int>(b) + delta, 0);

  return Color(color_r, color_g, color_b, a);
}

Color Color::operator-(int delta) {
  delta = -delta;
  return *this + delta;
}

Color::operator int32_t() {
  int32_t result = 0;
  uint8_t* components = reinterpret_cast<uint8_t*>(&result);

  components[0] = r;
  components[1] = g;
  components[2] = b;
  components[3] = a;

  return result;
}

#ifdef SFML_ENGINE
Color::operator sf::Color() const { return sf::Color(r, g, b, a); }
#endif

/*--------------- POSITION ----------------------------------*/

Position::Position() = default;
Position::Position(int16_t x, int16_t y) : x(x), y(y) {}

#ifdef SFML_ENGINE
Position::Position(const sf::Vector2f& sfpos)
    : x(static_cast<uint16_t>(sfpos.x)), y(static_cast<uint16_t>(sfpos.y)) {}

Position::operator sf::Vector2f() const { return sf::Vector2f(x, y); }
#endif

Position& Position::operator+=(const Position& other) {
  this->x += other.x;
  this->y += other.y;
  return *this;
}

Position::operator PluginAPI::Position() const {
  PluginAPI::Position plugin_adapter_pos = {x, y};
  return plugin_adapter_pos;
}

/*--------------- TEXT -------------------------------------*/

Text::Text() = default;
Text::Text(const char* text, uint16_t character_size, const char* font_path,
           Color color, Color bg_color, float line_spacing)
    : text(text),
      character_size(character_size),
      font_path(font_path),
      color(color),
      bg_color(bg_color),

      line_spacing(line_spacing) {}

/*--------------- VIEWPORT ---------------------------------*/
Viewport::Viewport() = default;
Viewport::Viewport(Size size, Position pos) : size(size), pos(pos) {}

/*-------------------- IMAGE -----------------------------*/
Image::Image(Size size, Color color) : size(size) {
  pixels.assign(size.width * size.height * sizeof(Color), 0);
  printf("Image size: %ld\n", pixels.size());
  fflush(stdout);
  for (int i = 0; i < pixels.size(); i += sizeof(Color)) {
    pixels[i] = color.r;
    pixels[i + 1] = color.g;
    pixels[i + 2] = color.b;
    pixels[i + 3] = color.a;
  }
}

void Image::setPixel(int x, int y, Color color) {
  int pos = (y * size.width + x) * sizeof(Color);
  pixels[pos] = color.r;
  pixels[pos + 1] = color.g;
  pixels[pos + 2] = color.b;
  pixels[pos + 3] = color.a;
}

Color Image::getPixel(int x, int y) {
  return *reinterpret_cast<Color*>(
      (pixels.data() + (y * size.width + x) * sizeof(Color)));
}

uint8_t* Image::get_pixel_array() { return pixels.data(); }

Size Image::get_size() { return size; }

Image::operator PluginAPI::Canvas() {
  PluginAPI::Canvas plugin_adapter_canvas = {};

  plugin_adapter_canvas.height = size.height;
  plugin_adapter_canvas.width = size.width;
  plugin_adapter_canvas.pixels = pixels.data();

  return plugin_adapter_canvas;
}

/*------------------------- TEXTURE -------------------------------*/
Texture::Texture() = default;
Texture::Texture(const char* path, Size size) : path(path), size(size) {}
