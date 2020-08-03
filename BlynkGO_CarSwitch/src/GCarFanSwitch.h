#ifndef __GCAR_FAN_SWITCH__
#define __GCAR_FAN_SWITCH__

#include <BlynkGO.h>

typedef struct car_fan_sw_ext{
  lv_img_ext_t img;
  GImage *  knob;
  GObject*  indic;
  int16_t   angle_offset;   // นับจาก 90 ตามเข็มเป็น + 
  int8_t    max_level;
  int8_t    level;
  float     indic_angle;
  uint16_t  indic_arm_length;
} gcar_fan_sw_ext_t;

class GCarFanSwitch : public GObject {
  public:
    GCarFanSwitch(GWidget& parent=GScreen);
    GCarFanSwitch(GWidget* parent);
    virtual ~GCarFanSwitch();
    virtual void create();
            void create(GWidget& parent);
            void create(GWidget* parent);
    void    selected(uint8_t selected_id);  // 0..6
    uint8_t selected_id();
  private:
    
};

GCarFanSwitch* new_GCarFanSwitch(GWidget& parent=GScreen);
GCarFanSwitch* new_GCarFanSwitch(GWidget *parent);

#endif //__GCAR_FAN_SWITCH__
