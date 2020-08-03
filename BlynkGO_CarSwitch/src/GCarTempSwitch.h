#ifndef __GCAR_TEMP_SWITCH__
#define __GCAR_TEMP_SWITCH__

#include <BlynkGO.h>

typedef struct car_temp_sw_ext{
  lv_img_ext_t img;
  GImage *  knob;
  GObject*  indic;
  int16_t   angle_offset;   // นับจาก 90 ตามเข็มเป็น + 
  int8_t    max_level;
  int8_t    level;          // 0..10
  float     indic_angle;
  uint16_t  indic_arm_length;
} gcar_temp_sw_ext_t;

class GCarTempSwitch : public GObject {
  public:
    GCarTempSwitch(GWidget& parent=GScreen);
    GCarTempSwitch(GWidget* parent);
    virtual ~GCarTempSwitch();
    virtual void create();
            void create(GWidget& parent);
            void create(GWidget* parent);
    void    selected(uint8_t selected_id);  // 0..10  ; Coolest 0-4 --> 5 Normal --> 6-10 Warmest
    uint8_t selected_id();
  private:
};

GCarTempSwitch* new_GCarTempSwitch(GWidget& parent=GScreen);
GCarTempSwitch* new_GCarTempSwitch(GWidget *parent);

#endif //__GCAR_TEMP_SWITCH__
