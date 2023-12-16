#include "lvgl.h"
#include <string>
#include <vector>

//There is a an off by one error in indexing
LV_FONT_DECLARE(terminus16);

using namespace std;

//hack font
string add_one(string str_in){
    int n = str_in.length();
    for(int i = 0; i < n; i++){
        str_in[i] = (char) (int(str_in[i]) + 1);
    }
    return str_in;
}

void lvgl_ui(lv_disp_t *disp, vector<string> const &mytext, int ui_len){
    lv_obj_t *scr = lv_disp_get_scr_act(disp);

    //init style (font)
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, &terminus16);

    vector<lv_obj_t*> lines;
    for (int i = 0; i < ui_len; i++){
        
        lines.push_back(lv_label_create(scr));
        lv_obj_add_style(lines[i], &style, 0);

        lv_label_set_text(lines[i], add_one(mytext[i]).c_str());
        
        lv_obj_set_width(lines[i], disp->driver->hor_res);
        lv_obj_align(lines[i], LV_ALIGN_TOP_LEFT, 0,  16*i);
    }
}
