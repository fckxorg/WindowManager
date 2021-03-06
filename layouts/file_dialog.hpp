#include "macro.hpp"

CREATE(file_inputbox, Inputbox, inputbox_size, inputbox_pos,
       Color(255, 255, 255), 20, "fonts/Roboto-Thin.ttf", Color(0, 0, 0));
CREATE(file_inputbox_outline, RectWindow, inputbox_outline_size,
       inputbox_outline_pos, Color(80, 90, 91));
CREATE(dialog_end_button, FileDialogEndButton, Size(100, 30),
       Position(pos.x + 665, pos.y + INPUTBOX_SAVE_DIALOG_OFFSET_Y + 5),
       Color(236, 236, 236), dynamic_cast<Inputbox*>(file_inputbox.get()), CanvasFileEvent::CanvasAction::SAVE);
CREATE(dialog_end_button_outline, RectWindow, Size(110, 40),
       Position(pos.x + 660, pos.y + INPUTBOX_SAVE_DIALOG_OFFSET_Y),
       Color(80, 90, 91));
CREATE(file_list, FileList, Size(680, 480), Size(1276, 800),
       Position(pos.x + 35, pos.y + 35), Color(255, 255, 255));
CREATE(scrollbar_outline, RectWindow, Size(40, 490),
       Position(pos.x + 730, pos.y + 30), Color(80, 90, 91));
CREATE(scrollbar, Scrollbar, Size(30, 480), Position(pos.x + 735, pos.y + 35),
       Color(200, 200, 200), 480, 720, 10);

CREATE(file_list_outline, RectWindow, Size(690, 490),
       Position(pos.x + 30, pos.y + 30), Color(80, 90, 91));

SUBSCRIBE(SubscriptionManager::get_system_event_sender(), file_inputbox.get());
SUBSCRIBE(SubscriptionManager::get_system_event_sender(),
          dialog_end_button.get());
SUBSCRIBE(SubscriptionManager::get_system_event_sender(), creator);

SUBSCRIBE(SubscriptionManager::get_system_event_sender(), file_list.get());
SUBSCRIBE(SubscriptionManager::get_system_event_sender(), scrollbar.get());
SUBSCRIBE(scrollbar.get(), file_list.get());
SUBSCRIBE(file_list.get(), scrollbar.get());
SUBSCRIBE(file_list.get(), file_inputbox.get());


