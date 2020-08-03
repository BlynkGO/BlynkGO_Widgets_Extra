#ifndef __GCAR_LIGHT_SWITCH__
#define __GCAR_LIGHT_SWITCH__

#include <BlynkGO.h>

typedef struct car_light_sw_ext{
  lv_img_ext_t img;
  GImage *  knob;
  GObject*  indic;
  int16_t   angle_offset;   // นับจาก 90 ตามเข็มเป็น + 
  int8_t    max_level;
  int8_t    level;
  float     indic_angle;
  uint16_t  indic_arm_length;
} gcar_light_sw_ext_t;

class GCarLightSwitch : public GObject {
  public:
    GCarLightSwitch(GWidget& parent=GScreen);
    GCarLightSwitch(GWidget* parent);
    virtual ~GCarLightSwitch();
    virtual void create();
            void create(GWidget& parent);
            void create(GWidget* parent);
    void    selected(uint8_t selected_id);  // 0..6
    uint8_t selected_id();
  private:
    
};

GCarLightSwitch* new_GCarLightSwitch(GWidget& parent=GScreen);
GCarLightSwitch* new_GCarLightSwitch(GWidget *parent);

#endif //__GCAR_LIGHT_SWITCH__
