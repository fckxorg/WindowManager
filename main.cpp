#include <bits/stdint-uintn.h>
#include <unistd.h>

#include <iostream>
#include <memory>

#include "app/app.hpp"
#include "data_classes/data_classes.hpp"
#include "instruments_manager/instruments_manager.hpp"
#include "subscription_manager/subscription_manager.hpp"
#include "window/window.hpp"

int main() {
  FILE* test_file = fopen("test_text.txt", "r");

  if (test_file == nullptr) {
    perror("Can't open the file");
    return -1;
  }

  std::unique_ptr<Window> root_window(new RootWindow());
  std::unique_ptr<Window> window(
      new RectWindow(Size(1920, 1080), Position(0, 0), Color(212, 212, 212)));

  std::unique_ptr<Window> pencil_button_sprite(
      new Sprite(Texture("icons/pencil.png", Size(50, 50)), Position(15, 910)));
  std::unique_ptr<Window> eraser_button_sprite(
      new Sprite(Texture("icons/eraser.png", Size(50, 50)), Position(15, 980)));

  std::unique_ptr<Window> toolbar_listener(new ToolbarListener());

  std::unique_ptr<Window> pencil_button_outline(
      new RectWindow(Size(60, 60), Position(10, 905), Color(80, 90, 91)));
  std::unique_ptr<Window> eraser_button_outline(
      new RectWindow(Size(60, 60), Position(10, 975), Color(80, 90, 91)));
  std::unique_ptr<Window> hue_selector_outline(
      new RectWindow(Size(370, 40), Position(1540, 1010), Color(80, 90, 91)));
  std::unique_ptr<Window> sv_selector_outline(
      new RectWindow(Size(370, 370), Position(1540, 630), Color(80, 90, 91)));

  std::unique_ptr<Window> pencil_button(new RectButton(
      Size(50, 50), Position(15, 910), Color(236, 236, 236), PENCIL));
  std::unique_ptr<Window> eraser_button(new RectButton(
      Size(50, 50), Position(15, 980), Color(236, 236, 236), ERASER));

  std::unique_ptr<Window> hue_selector(
      new HUEselector(Size(360, 30), Position(1545, 1015)));
  std::unique_ptr<Window> sv_selector(
      new SVselector(Size(360, 360), Position(1545, 635)));

  std::unique_ptr<Window> hue_slider(new Slider(
      Size(3, 30), Position(1545, 1015), Color(0, 0, 0), 1545, 1905, 1, true));

  SUBSCRIBE(root_window.get(), hue_slider.get());

  pencil_button->add_child_window(pencil_button_sprite);
  eraser_button->add_child_window(eraser_button_sprite);

  SUBSCRIBE(root_window.get(), pencil_button.get());
  SUBSCRIBE(root_window.get(), eraser_button.get());

  SUBSCRIBE(pencil_button.get(), toolbar_listener.get());
  SUBSCRIBE(eraser_button.get(), toolbar_listener.get());

  SUBSCRIBE(hue_slider.get(), hue_selector.get());
  SUBSCRIBE(hue_selector.get(), sv_selector.get());

  pencil_button_outline->add_child_window(pencil_button);
  eraser_button_outline->add_child_window(eraser_button);
  hue_selector_outline->add_child_window(hue_selector);
  hue_selector_outline->add_child_window(hue_slider);
  sv_selector_outline->add_child_window(sv_selector);

  InstrumentManager::init();

  std::unique_ptr<Window> canvas(
      new Canvas(Size(1920, 890), Position(0, 0), Color(255, 255, 255)));

  SUBSCRIBE(root_window.get(), canvas.get());

  window->add_child_window(canvas);

  root_window->add_child_window(window);
  root_window->add_child_window(pencil_button_outline);
  root_window->add_child_window(eraser_button_outline);
  root_window->add_child_window(hue_selector_outline);
  root_window->add_child_window(sv_selector_outline);

  App::init(Size(1920, 1080), "Test application");
  App::set_root_window(root_window);
  App::run();
  App::deinit();

  return 0;
}

