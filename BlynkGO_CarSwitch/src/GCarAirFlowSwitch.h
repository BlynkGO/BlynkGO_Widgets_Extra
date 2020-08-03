#ifndef __GCAR_AIRFLOW_SWITCH__
#define __GCAR_AIRFLOW_SWITCH__

#include <BlynkGO.h>

typedef struct car_airflow_sw_ext{
  lv_img_ext_t img;
  GImage *  knob;
  GObject*  indic;
  int16_t   angle_offset;   // นับจาก 90 ตามเข็มเป็น + 
  int8_t    max_level;
  int8_t    level;
  float     indic_angle;
  uint16_t  indic_arm_length;
} gcar_airflow_sw_ext_t;

class GCarAirFlowSwitch : public GObject {
  public:
    GCarAirFlowSwitch(GWidget& parent=GScreen);
    GCarAirFlowSwitch(GWidget* parent);
    virtual ~GCarAirFlowSwitch();
    virtual void create();
            void create(GWidget& parent);
            void create(GWidget* parent);
    void    selected(uint8_t selected_id);  // 0..4
    uint8_t selected_id();
  private:
    
};

GCarAirFlowSwitch* new_GCarAirFlowSwitch(GWidget& parent=GScreen);
GCarAirFlowSwitch* new_GCarAirFlowSwitch(GWidget *parent);

#endif //__GCAR_AIRFLOW_SWITCH__
