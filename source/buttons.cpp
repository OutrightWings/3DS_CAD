#include "buttons.hpp"
std::vector<Button *> vertex_editor_buttons;
std::vector<Button *> pause_menu_buttons;
std::vector<Button *> vertex_options_buttons;

void initButtons(){
    vertex_editor_buttons = {
        new Button(279,196,38,26,-1,-1),
        new Button(279,169,38,26,-1,-1),
        new ViewButton(279,57,38,26,-1,1,VIEW_TOP),
        new ViewButton(279,30,38,26,-1,1,VIEW_LEFT),
        new ViewButton(279,3,38,26,-1,1,VIEW_RIGHT),
        new ViewButton(279,84,38,26,-1,1,VIEW_OPP_TOP),
        new ViewButton(279,138,38,26,-1,1,VIEW_OPP_LEFT),
        new ViewButton(279,111,38,26,-1,1,VIEW_OPP_RIGHT)
    };
    pause_menu_buttons = {
        new Button(45,37,75,38,-1,-1),
        new Button(122,37,75,38,-1,-1),
        new Button(199,37,75,38,-1,-1),  
        new Button(45,85,75,38,-1,7),
        new Button(122,85,75,38,-1,7),
        new Button(199,85,75,38,-1,7)
    };
    vertex_options_buttons = {
        new Button(45,133,18,18,9,10),
        new Button(45,155,18,18,9,10)
    };
}
