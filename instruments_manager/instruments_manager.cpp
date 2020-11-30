#include "instruments_manager.hpp"

#include <random>

const int MAX_THICKNESS = 40;
const int SPRAY_DENSITY = 20;

void ToolbarListener::handle_event(Event* event) {
  if (event->get_type() == BUTTON_PRESSED) {
    auto button_event = dynamic_cast<ButtonPressEvent*>(event);
    InstrumentManager::set_instrument(button_event->value);
  }

  if (event->get_type() == COLOR_CHANGED) {
    auto color_event = dynamic_cast<ColorChangedEvent*>(event);
    InstrumentManager::set_color(color_event->color);
  }

  if (event->get_type() == SLIDER_MOVE) {
    auto slider_event = dynamic_cast<SliderMoveEvent*>(event);
    InstrumentManager::set_thickness(slider_event->position * MAX_THICKNESS);
  }
}

void ToolbarListener::render() {}

AbstractInstrument::~AbstractInstrument() = default;

void AbstractInstrument::init(Position pos) {}
void AbstractInstrument::deinit() {}

Pencil::Pencil() = default;

void Pencil::apply(Image& canvas, Position point, Position last_point,
                   Color color, uint8_t thickness) {
  int16_t Position::*primary_axis = nullptr;
  int16_t Position::*secondary_axis = nullptr;

  int16_t x_diff = abs(point.x - last_point.x);
  int16_t y_diff = abs(point.y - last_point.y);

  if (x_diff > y_diff) {
    primary_axis = &Position::x;
    secondary_axis = &Position::y;
  } else {
    primary_axis = &Position::y;
    secondary_axis = &Position::x;
  }

  // TODO splines
  // if there is not enough points for approximation (usually 4) you can
  // duplicate points
  float k =
      static_cast<float>(point.*secondary_axis - last_point.*secondary_axis) /
      (point.*primary_axis - last_point.*primary_axis);
  float b = point.*secondary_axis - k * point.*primary_axis;

  for (int x = std::min(point.*primary_axis, last_point.*primary_axis);
       x <= std::max(point.*primary_axis, last_point.*primary_axis); x += 1) {
    for (int i = -thickness; i <= thickness; ++i) {
      for (int j = -thickness; j <= thickness; ++j) {
        if (i * i + j * j < thickness * thickness) {
          if (x_diff > y_diff) {
            canvas.setPixel(std::max(x + i, 0), std::max(k * x + b + j, 0.f),
                            color);
          } else {
            canvas.setPixel(std::max(k * x + b + i, 0.f), std::max(x + j, 0),
                            color);
          }
        }
      }
    }
  }
}

Eraser::Eraser() = default;

void Brush::apply(Image& canvas, Position point, Position last_point,
                  Color color, uint8_t thickness) {
  int16_t Position::*primary_axis = nullptr;
  int16_t Position::*secondary_axis = nullptr;

  int16_t x_diff = abs(point.x - last_point.x);
  int16_t y_diff = abs(point.y - last_point.y);

  if (x_diff > y_diff) {
    primary_axis = &Position::x;
    secondary_axis = &Position::y;
  } else {
    primary_axis = &Position::y;
    secondary_axis = &Position::x;
  }

  // TODO splines
  // if there is not enough points for approximation (usually 4) you can
  // duplicate points
  float k =
      static_cast<float>(point.*secondary_axis - last_point.*secondary_axis) /
      (point.*primary_axis - last_point.*primary_axis);
  float b = point.*secondary_axis - k * point.*primary_axis;

  for (int x = std::min(point.*primary_axis, last_point.*primary_axis);
       x <= std::max(point.*primary_axis, last_point.*primary_axis); x += 1) {
    for (int i = -thickness; i <= thickness; ++i) {
      if (x_diff > y_diff) {
        canvas.setPixel(x, k * x + b + i, color);
      } else {
        canvas.setPixel(k * x + b, x + i, color);
      }
    }
  }
}

void Dropper::apply(Image& canvas, Position point, Position last_point,
                    Color color, uint8_t thickness) {
  Color pixel = canvas.getPixel(point.x, point.y);
  SEND(nullptr, new DropperEvent(pixel));
  // TODO add fictive sender to subscription_manager instead of nullptr
}

void Spray::apply(Image& canvas, Position point, Position last_point,
                  Color color, uint8_t thickness) {
  int16_t Position::*primary_axis = nullptr;
  int16_t Position::*secondary_axis = nullptr;

  int16_t x_diff = abs(point.x - last_point.x);
  int16_t y_diff = abs(point.y - last_point.y);

  if (x_diff > y_diff) {
    primary_axis = &Position::x;
    secondary_axis = &Position::y;
  } else {
    primary_axis = &Position::y;
    secondary_axis = &Position::x;
  }

  std::minstd_rand randomizer;

  // TODO splines
  // if there is not enough points for approximation (usually 4) you can
  // duplicate points
  float k =
      static_cast<float>(point.*secondary_axis - last_point.*secondary_axis) /
      (point.*primary_axis - last_point.*primary_axis);
  float b = point.*secondary_axis - k * point.*primary_axis;

  for (int x = std::min(point.*primary_axis, last_point.*primary_axis);
       x <= std::max(point.*primary_axis, last_point.*primary_axis); x += 1) {
    for (int i = -thickness; i <= thickness; ++i) {
      for (int j = -thickness; j <= thickness; ++j) {
        bool not_colored = randomizer() % SPRAY_DENSITY;
        if (i * i + j * j < thickness * thickness && !not_colored) {
          if (x_diff > y_diff) {
            canvas.setPixel(std::max(x + i, 0), std::max(k * x + b + j, 0.f),
                            color);
          } else {
            canvas.setPixel(std::max(k * x + b + i, 0.f), std::max(x + j, 0),
                            color);
          }
        }
      }
    }
  }
}

void Clear::apply(Image& canvas, Position point, Position last_point,
                  Color color, uint8_t thickness) {
  for (int x = 0; x < canvas.get_size().width; ++x) {
    for (int y = 0; y < canvas.get_size().height; ++y) {
      canvas.setPixel(x, y, color);
    }
  }
}

void Rect::apply(Image& canvas, Position point, Position last_point,
                 Color color, uint8_t thickness) {
  rect_data.size.width = point.x - rect_data.pos.x;
  rect_data.size.height = point.y - rect_data.pos.y;
  rect_data.color = color;
  Renderer::add_delayed(rect_data);
}

void Rect::init(Position pos) {
    rect_data.pos = pos;
    rect_data.type = RECT;
}

void Ellipse::apply(Image& canvas, Position point, Position last_point,
                    Color color, uint8_t thickness) {
  ellipse_data.size.width = point.x - ellipse_data.pos.x;
  ellipse_data.size.height = point.y - ellipse_data.pos.y;
  ellipse_data.color = color;
  Renderer::add_delayed(ellipse_data);
}

void Ellipse::init(Position pos) {
    ellipse_data.pos = pos;
    ellipse_data.type = ELLIPSE;
}

bool InstrumentManager::application_started = false;
Position InstrumentManager::last_point = Position(-1, -1);
std::vector<std::unique_ptr<AbstractInstrument>> InstrumentManager::instruments(
    COUNT);
int InstrumentManager::current_instrument = PENCIL;
uint8_t InstrumentManager::thickness = 1;
Color InstrumentManager::color = Color(0, 0, 0);

void InstrumentManager::init() {
  instruments[ERASER] =
      std::move(std::unique_ptr<AbstractInstrument>(new Eraser()));
  instruments[PENCIL] =
      std::move(std::unique_ptr<AbstractInstrument>(new Pencil()));
  instruments[BRUSH] =
      std::move(std::unique_ptr<AbstractInstrument>(new Brush()));
  instruments[DROPPER] =
      std::move(std::unique_ptr<AbstractInstrument>(new Dropper()));
  instruments[SPRAY] =
      std::move(std::unique_ptr<AbstractInstrument>(new Spray()));
  instruments[CLEAR] =
      std::move(std::unique_ptr<AbstractInstrument>(new Clear()));
  instruments[RECT_INSTRUMENT] =
      std::move(std::unique_ptr<AbstractInstrument>(new Rect()));
  instruments[ELLIPSE_INSTRUMENT] =
      std::move(std::unique_ptr<AbstractInstrument>(new Ellipse()));
}

void InstrumentManager::start_applying(Position pos) {
  application_started = true;
  last_point.x = pos.x + 1;
  last_point.y = pos.y;
    
  instruments[current_instrument]->init(pos);
}

void InstrumentManager::stop_applying(Image& canvas) {
  application_started = false;
  if (current_instrument == RECT_INSTRUMENT) {
    DelayedRenderData* rect_data =
        &(dynamic_cast<Rect*>(instruments[RECT_INSTRUMENT].get())->rect_data);
    if (rect_data->size.width < 0) {
      rect_data->pos.x += rect_data->size.width;
      rect_data->size.width = -rect_data->size.width;
    }

    if (rect_data->size.height < 0) {
      rect_data->pos.y += rect_data->size.height;
      rect_data->size.height = -rect_data->size.height;
    }

    for (int x = 0; x < rect_data->size.width; ++x) {
      for (int y = 0; y < rect_data->size.height; ++y) {
        canvas.setPixel(x + rect_data->pos.x, y + rect_data->pos.y, color);
      }
    }

    rect_data->pos = Position(-1, -1);
    rect_data->size = Size(0, 0);
    Renderer::remove_delayed();
  }

  if (current_instrument == ELLIPSE_INSTRUMENT) {
    DelayedRenderData* ellipse_data =
        &(dynamic_cast<Ellipse*>(instruments[ELLIPSE_INSTRUMENT].get())
              ->ellipse_data);
    if (ellipse_data->size.width < 0) {
      ellipse_data->pos.x += ellipse_data->size.width;
      ellipse_data->size.width = -ellipse_data->size.width;
    }

    if (ellipse_data->size.height < 0) {
      ellipse_data->pos.y += ellipse_data->size.height;
      ellipse_data->size.height = -ellipse_data->size.height;
    }

    float radius_hor = static_cast<float>(ellipse_data->size.width) / 2;
    float radius_vert = static_cast<float>(ellipse_data->size.height) / 2;

    for (int x = 0; x < ellipse_data->size.width; ++x) {
      for (int y = 0; y < ellipse_data->size.height; ++y) {
        float x_eq_part =
            (x - radius_hor) * (x - radius_hor) / (radius_hor * radius_hor);
        float y_eq_part =
            (y - radius_vert) * (y - radius_vert) / (radius_vert * radius_vert);
        if (x_eq_part + y_eq_part <= 1) {
          canvas.setPixel(x + ellipse_data->pos.x, y + ellipse_data->pos.y,
                          color);
        }
      }
    }

    ellipse_data->pos = Position(-1, -1);
    ellipse_data->size = Size(0, 0);
    Renderer::remove_delayed();
  }
}

void InstrumentManager::apply(Image& canvas, Position pos) {
  switch (current_instrument) {
    case ERASER: {
      instruments[current_instrument]->apply(canvas, pos, last_point,
                                             Color(255, 255, 255), thickness);
      break;
    }

    case CLEAR: {
      instruments[current_instrument]->apply(canvas, pos, last_point,
                                             Color(255, 255, 255), thickness);
      break;
    }

    default: {
      instruments[current_instrument]->apply(canvas, pos, last_point, color,
                                             thickness);
      break;
    }
  }

  last_point = pos;
}

bool InstrumentManager::is_applying() { return application_started; }

void InstrumentManager::set_instrument(uint8_t instrument) {
  current_instrument = instrument;
}

void InstrumentManager::set_color(Color color) {
  InstrumentManager::color = color;
}

Color InstrumentManager::get_color() { return InstrumentManager::color; }

void InstrumentManager::set_thickness(uint8_t thickness) {
  InstrumentManager::thickness = thickness;
}

