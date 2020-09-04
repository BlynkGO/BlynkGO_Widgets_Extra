#include "GCarFanSwitch.h"

#define LV_OBJX_NAME  "GCarFanSwitch"

IMG_DECLARE(pic_car_switch);
IMG_DECLARE(pic_car_switch_knob);

static lv_signal_cb_t ancestor_signal_cb;
static lv_signal_cb_t ancestor_indic_signal_cb;
static lv_res_t GSignal_cb(lv_obj_t *obj, lv_signal_t sign, void* param);
static lv_res_t fan_indic_signal_cb(lv_obj_t *obj, lv_signal_t sign, void* param);

static lv_point_t point_from_angle(uint16_t r, float angle_deg, lv_point_t pivot );
static float angle_from_point(lv_point_t point, lv_point_t pivot) ;
static void fan_switch_refresh(GCarFanSwitch *pCarFanSW);

static lv_res_t GSignal_cb(lv_obj_t *obj, lv_signal_t sign, void* param)
{
  lv_res_t res;
  res = ancestor_signal_cb( obj, sign, param);
  if(res != LV_RES_OK ) return res;
  if(sign == LV_SIGNAL_GET_TYPE) 
    return lv_obj_handle_get_type_signal((lv_obj_type_t*)param, LV_OBJX_NAME);

  /* หลัง del() หรือ deconstrutor ถูกเรียก และภายในได้ลบ child ไปแล้ว */
  if(sign == LV_SIGNAL_CLEANUP) {
    Serial.println("[GCarFanSwitch] signal cleanup");
    GCarFanSwitch* pCarFanSW = (GCarFanSwitch*) lv_obj_get_user_data(obj);

    gcar_fan_sw_ext_t *ext = (gcar_fan_sw_ext_t *) pCarFanSW->ext_attr();
    free_widget(ext->knob);
    free_widget(ext->indic);
    
    if(pCarFanSW->hasLocalStyle()) {pCarFanSW->freeLocalStyle(); } //pLed->_has_localstyle = false; } // 
    pCarFanSW->_created = false;
  }
  return LV_RES_OK;
}


GCarFanSwitch::GCarFanSwitch(GWidget& parent) : GObject(&parent) {}
GCarFanSwitch::GCarFanSwitch(GWidget* parent) : GObject(parent) {}
GCarFanSwitch::~GCarFanSwitch() { this->del(); }

void GCarFanSwitch::create(){
  if(this->_par != NULL && !this->_par->isCreated()) this->_par->create();
  if(this->_par->obj == NULL) return;

  if(!this->isCreated()) {
    this->obj = lv_img_create(this->_par->obj, NULL);
    if(this->obj != NULL) {
      this->_created = true;
      lv_obj_set_user_data(this->obj, this);
      lv_obj_set_event_cb (this->obj, main_event_cb);
      
      lv_img_set_src(this->obj, (void*) &pic_car_switch);
      this->clickable(false);
   
      gcar_fan_sw_ext_t *ext = (gcar_fan_sw_ext_t*) this->alloc_ext_attr(sizeof(gcar_fan_sw_ext_t));
      ext->angle_offset  = -96;
      ext->max_level     = 6;
      ext->level         = 0;
      
      ext->knob          = new_GImage(this);
      ext->indic         = new_GObject(this);
      
      ext->knob->setImage(pic_car_switch_knob);
      ext->knob->clickable(false);
      ext->knob->align(ALIGN_CENTER);

      ext->indic->size(60, 60);
      ext->indic->opa(0);
      ext->indic->dragable(true);
      ext->indic->protect(PROTECT_PRESS_LOST);

      ext->indic_arm_length = (ext->knob->width() - ext->indic->width())/2;

      if(ancestor_indic_signal_cb == NULL) ancestor_indic_signal_cb =  ext->indic->signal_cb();
      ext->indic->signal_cb(fan_indic_signal_cb);
      
      this->align(ALIGN_CENTER,0,0);

      if(ancestor_signal_cb == NULL) ancestor_signal_cb = this->signal_cb();
      this->signal_cb(GSignal_cb);
    }
  }
}

void GCarFanSwitch::create(GWidget& parent){
  this->_par = &parent;
  create();
}

void GCarFanSwitch::create(GWidget* parent) {
  this->_par = parent;
  create();
}

void GCarFanSwitch::selected(uint8_t selected_id){
  if(!this->isCreated()) this->create();
  gcar_fan_sw_ext_t *ext = (gcar_fan_sw_ext_t *) this->ext_attr();
  selected_id = constrain(selected_id, 0, ext->max_level);
  
  int knob_angle = (selected_id)* abs(ext->angle_offset)/3 + ext->angle_offset;

  ext->indic_angle = ANGLE360(90 - knob_angle);
  fan_switch_refresh(this);
}

uint8_t GCarFanSwitch::selected_id(){
  if(!this->isCreated()) this->create();
  gcar_fan_sw_ext_t *ext = (gcar_fan_sw_ext_t *) this->ext_attr();
  return ext->level;
}


static lv_res_t fan_indic_signal_cb(lv_obj_t* indic_obj, lv_signal_t sign, void* param)
{
  lv_res_t res;

  // เรียก singal_cb ของ ancestor เสียก่อน
  res = ancestor_indic_signal_cb(indic_obj, sign, param);
  if(res != LV_RES_OK) return res;

  GObject* indic = (GObject*) lv_obj_get_user_data(indic_obj);
  GCarFanSwitch* pCarFanSW = (GCarFanSwitch*) indic->_par;
  gcar_fan_sw_ext_t *ext = (gcar_fan_sw_ext_t *) pCarFanSW->ext_attr();
    
  // ค่อยดัก SIGNAL ต่างๆ ที่ต้องการ
  if( sign == LV_SIGNAL_CORD_CHG) { 
    
    lv_coord_t x = indic->posX();
    lv_coord_t y = indic->posY();
    lv_coord_t w = indic->width();
    lv_coord_t h = indic->height();

    lv_point_t pivot                  = { pCarFanSW->width()/2, pCarFanSW->height()/2 };
    lv_point_t indic_center_p         = {x+ w/2,y + h/2};

    // มุมวัดจากแกน x,y หมุนทวนเข็ม ตามระบบ math ปกติ
    ext->indic_angle = angle_from_point( indic_center_p, pivot);

    // กำหนดมุม ระหว่าง angle_start องศา ถึง angle_end องศา (กวาดมุมแบบนับตามเข็ม) แต่มุมนับทวนเข็มตามระบบ xy
    int16_t angle_start = ANGLE360(90 - ext->angle_offset); if(angle_start==0) angle_start = 360;
    int16_t angle_end   = ANGLE360(90 + ext->angle_offset); if(angle_end==0) angle_end = 360;
    if(ext->indic_angle> angle_start && ext->indic_angle<=270)    ext->indic_angle = angle_start;
    else if(ext->indic_angle> 270 && ext->indic_angle<angle_end ) ext->indic_angle = angle_end;
    fan_switch_refresh(pCarFanSW);    
  }else
  if( sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST ) {
    int16_t angle_from_90 = ext->knob->angle();
    
    uint16_t angle_from_offset     = ANGLE360(angle_from_90 - ext->angle_offset);
    uint16_t angle_max_from_offset = ANGLE360(2* abs(ext->angle_offset));
    uint8_t level = round( map_f ( angle_from_offset, 0, angle_max_from_offset, 0, ext->max_level));
    pCarFanSW->selected(level);
  }
  
  // ไม่ลืมคืนค่าด้วย
  return LV_RES_OK;
}

/**
 * Calculate a point from angle rotate around a pivot point;  // คำนวณหาจุด ด้วยมุมนับทวนเข็มตามแนวแกน x,y รอบจุดหมุน
 * @param r : radius
 * @param angle_deg : a degree to rotate r around a pivot point
 * @param pivot : a pivot point
 * @return the point
 */
static lv_point_t point_from_angle(uint16_t r, float angle_deg, lv_point_t pivot )
{
  lv_point_t p;

  float a = radians(angle_deg);
  p.x = pivot.x + r*cosf(a); 
  p.y = pivot.y - r*sinf(a);
  return p;
}

/**
 * Calculate the angle (degree) of a point from a pivot point;  // มุมนับทวนเข็มตามแนวแกน x,y 
 * @param point : a point which to know its angle
 * @param pivot : a pivot point
 * @return the angle (degree) of a point
 */
static float angle_from_point(lv_point_t point, lv_point_t pivot) {
  if(point.x== pivot.x && point.y == pivot.y) return 0;
  
  float dx = abs( point.x - pivot.x); int8_t signX = ( point.x >= pivot.x)? 1 : -1;
  float dy = abs( point.y - pivot.y); int8_t signY = ( point.y <= pivot.y)? 1 : -1;
  float r  = sqrt((pivot.x-point.x)*(pivot.x-point.x) + (pivot.y-point.y)*(pivot.y-point.y));

  float a  = asin( dy/r);
  if( signX ==  1 && signY ==  1) { return degrees(a);       } else  // จตุภาค 1
  if( signX == -1 && signY ==  1) { return degrees(PI - a);  } else  // จตุภาค 2
  if( signX == -1 && signY == -1) { return degrees(PI + a);  } else  // จตุภาค 3
  if( signX ==  1 && signY == -1) { return degrees(2*PI - a);}       // จตุภาค 4

  return 0;
}

static void fan_switch_refresh(GCarFanSwitch *pCarFanSW){
  gcar_fan_sw_ext_t *ext = (gcar_fan_sw_ext_t *) pCarFanSW->ext_attr();

  // คำนวนตำแหน่งศูนย์กล่าง indic แล้วย้ายตำแหน่ง indic
  lv_point_t c =  {0,0};
  lv_point_t pos = point_from_angle(ext->indic_arm_length, ext->indic_angle, c);
  ext->indic->align(ALIGN_CENTER, pos.x, pos.y);

  float knob_angle = 90 - ext->indic_angle;
  ext->knob->angle(knob_angle);

  // แปลงหา level จาก มุม ext->indic_angle ปัจจุบัน
  // ภ้า level ไม่ตรงกับ ext->level ให้ทำการส่ง EVENT_VALUE_CHANGED ออกไป เพื่อแจ้งบอก  
  uint16_t angle_from_offset     = ANGLE360(knob_angle - ext->angle_offset);  // แปลงมุมหมุนภาพ แต่เริ่มนับจากมุม angle_offset
  uint16_t angle_max_from_offset = ANGLE360(2* abs(ext->angle_offset));       // มุมหมุนภาพ ที่มากสุด นับจาก angle_offset
  uint8_t level = round( map_f ( angle_from_offset, 0, angle_max_from_offset, 0, ext->max_level));  // แปลง level ออกมา
  level = constrain(level, 0, ext->max_level);
  if(ext->level != level ){
    ext->level = level;
    pCarFanSW->event_send(EVENT_VALUE_CHANGED, NULL);    // อัพเดต level สู่ระบบ พร้อมส่ง EVENT_VALUE_CHANGED ออกไป
  }
}


GCarFanSwitch* new_GCarFanSwitch(GWidget& parent){
  GCarFanSwitch* w = (GCarFanSwitch*) esp32_malloc(sizeof(GCarFanSwitch));
  new (w) GCarFanSwitch(parent);
  return w;
}

GCarFanSwitch* new_GCarFanSwitch(GWidget *parent){
  GCarFanSwitch* w = (GCarFanSwitch*) esp32_malloc(sizeof(GCarFanSwitch));
  new (w) GCarFanSwitch(parent);
  return w;
}
