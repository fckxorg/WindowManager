#ifndef INSTRUMENT_MANAGER
#define INSTRUMENT_MANAGER

#include <cstdarg>
#include <memory>
#include <random>
#include <vector>

#include "../data_classes/data_classes.hpp"
#include "../event/event.hpp"
#include "../subscription_manager/subscription_manager.hpp"
#include "../window_base/window_base.hpp"
#include "../sfml_engine/sfml_engine.hpp"

class ToolbarListener : public Window {
 public:
  void handle_event(Event* event) override;
  void render() override;
};

enum INSTRUMENTS { ERASER, PENCIL, BRUSH, DROPPER, SPRAY, CLEAR, RECT_INSTRUMENT, ELLIPSE_INSTRUMENT, COUNT };

class AbstractInstrument {
 public:
  virtual void init(Position pos);
  virtual void deinit(Image& canvas, Color color);
  virtual void apply(Image& canvas, Position point, Position last_point,
                     Color color, uint8_t thickness) = 0;

  virtual ~AbstractInstrument();
};

class Pencil : public AbstractInstrument {
 public:
  Pencil();
  virtual void apply(Image& canvas, Position point, Position last_point,
                     Color color, uint8_t thickness) override;
};

class Eraser : public Pencil {
 public:
  Eraser();
};

class Brush : public AbstractInstrument {
 public:
  virtual void apply(Image& canvas, Position point, Position last_point,
                     Color color, uint8_t thickness) override;
};

class Spray : public AbstractInstrument {
 public:
  virtual void apply(Image& canvas, Position point, Position last_point,
                     Color color, uint8_t thickness) override;
};

class Dropper : public AbstractInstrument {
 public:
  virtual void apply(Image& canvas, Position point, Position last_point,
                     Color color, uint8_t thickness) override;
};

class Clear : public AbstractInstrument {
 public:
  virtual void apply(Image& canvas, Position point, Position last_point,
                     Color color, uint8_t thickness) override;
};

class Rect : public AbstractInstrument {
 private:
  DelayedRenderData rect_data;

 public:
  virtual void init(Position pos) override;
  virtual void deinit(Image& canvas, Color color) override;
  virtual void apply(Image& canvas, Position point, Position last_point,
                     Color color, uint8_t thickness) override;
  friend class InstrumentManager;
};

class Ellipse : public AbstractInstrument {
 private:
  DelayedRenderData ellipse_data;

 public:
  virtual void init(Position pos) override;
  virtual void deinit(Image& canvas, Color color) override;
  virtual void apply(Image& canvas, Position point, Position last_point,
                     Color color, uint8_t thickness) override;
  friend class InstrumentManager;
};


class InstrumentManager {
 private:
  static bool application_started;
  static Position last_point;
  static std::vector<std::unique_ptr<AbstractInstrument>> instruments;
  static int current_instrument;

  static uint8_t thickness;
  static Color color;

 public:
  static void init();

  static void start_applying(Position pos);

  static void stop_applying(Image& canvas);

  static void apply(Image& canvas, Position pos);

  static bool is_applying();

  static void set_instrument(uint8_t instrument);
  static void set_color(Color color);
  static void set_thickness(uint8_t thickness);

  static Color get_color();
};

#endif
