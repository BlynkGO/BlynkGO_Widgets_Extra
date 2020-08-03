#include <BlynkGO.h>
#include <GCarFanSwitch.h>          // Note : หากไม่ใช้ วิตเจ็ตเสริมไหน ไม่ต้องเพิ่ม
#include <GCarAirFlowSwitch.h>
#include <GCarTempSwitch.h>
#include <GCarLightSwitch.h>

GMultiScreen      screens;          // วิตเจ็ต GMultiScreen สำหรับเพิ่มหลายหน้า Screen
GCarFanSwitch     FanSwitch;        // วิตเจ็ต GCarFanSwitch สำหรับ ปุ่มหมุนของรถยนต์ในการปรับความแรงพัดลมแอร์
GCarAirFlowSwitch AirflowSwitch;    // วิตเจ็ต GCarAirFlowSwitch สำหรับ ปุ่มหมุนของรถยนต์ในการปรับทิศทางลมแอร์
GCarTempSwitch    TempSwitch;       // วิตเจ็ต GCarTempSwitch สำหรับ ปุ่มหมุนของรถยนต์ในการปรับความเย็นความอุ่นของแอร์
GCarLightSwitch   LightSwitch;      // วิตเจ็ต GCarLightSwitch สำหรับ ปุ่มหมุนของรถยนต์ในการปรับไฟหน้ารถยนต์
GLabel            label[4];         // วิตเจ็ต GLabel สำหรับสร้างข้อความ

void setup() {
  Serial.begin(115200); Serial.println();
  BlynkGO.begin();
  BlynkGO.fillScreen(TFT_COLOR(28,28,28));

  /*-------------------------------------------------*/
  // เพิ่มหลายหน้า screens พร้อมกำหนด วิตเจ็ตต่างๆให้อยู่ screens ไหน
  screens.addScreen(4);
    FanSwitch     .parent( screens[0] );  label[0].parent( screens[0] );
    AirflowSwitch .parent( screens[1] );  label[1].parent( screens[1] );
    TempSwitch    .parent( screens[2] );  label[2].parent( screens[2] );
    LightSwitch   .parent( screens[3] );  label[3].parent( screens[3] );

  /*-------------------------------------------------*/
  FanSwitch.selected(4);        // id : 0..6
  FanSwitch.onValueChanged([](GWidget*w){
    label[0] = "Fan : " + String(FanSwitch.selected_id());
  });
  label[0] = "Fan : " + String(FanSwitch.selected_id());
  label[0].align(FanSwitch, ALIGN_TOP);
  
  /*-------------------------------------------------*/
  AirflowSwitch.selected(2);     // id : 0..4
  AirflowSwitch.onValueChanged([](GWidget*w){
    label[1] = "Air Flow : " + String(AirflowSwitch.selected_id());
  });
  label[1] = "Air Flow : " + String(AirflowSwitch.selected_id());
  label[1].align(AirflowSwitch, ALIGN_TOP);

  /*-------------------------------------------------*/
  TempSwitch.selected(5);       // id : 0..10  (เย็นมาก 0,1,2,3,4 -> กลางๆ 5 -> 6,7,8,9,10 อุ่นมาก)
  TempSwitch.onValueChanged([](GWidget*w){
    label[2] = "เย็น-อุ่น : " + String(TempSwitch.selected_id());
  });
  label[2] = "เย็น-อุ่น : " + String(TempSwitch.selected_id());
  label[2].align(TempSwitch, ALIGN_TOP);

  /*-------------------------------------------------*/
  LightSwitch.selected(4);     // id : 0..5
  LightSwitch.onValueChanged([](GWidget*w){
    label[3] = "Light : " + String(LightSwitch.selected_id());
  });
  label[3] = "Light : " + String(LightSwitch.selected_id());
  label[3].align(LightSwitch, ALIGN_TOP);

}

void loop() {
  BlynkGO.update();
}

