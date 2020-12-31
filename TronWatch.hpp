// LiliGo TTGO Watch 2020 V1 Wrapper

LV_IMG_DECLARE(arrow_left_png);
LV_IMG_DECLARE(arrow_right_png);
LV_FONT_DECLARE(fn1_32);
LV_FONT_DECLARE(robot_ightItalic_16);
LV_FONT_DECLARE(robot_light_16);
LV_FONT_DECLARE(liquidCrystal_nor_64);
LV_FONT_DECLARE(liquidCrystal_nor_32);
LV_FONT_DECLARE(liquidCrystal_nor_24);
LV_FONT_DECLARE(hansans_cn_24);
LV_FONT_DECLARE(digital_play_st_48);
LV_FONT_DECLARE(quostige_16);
LV_FONT_DECLARE(digital_play_st_24);
LV_FONT_DECLARE(gracetians_32);
LV_FONT_DECLARE(exninja_22);




void GetMonthAbbr(char* month_str, uint8_t month)
{
  //char month_str[4] = {'\0'};
  switch (month)
  {
    case 1:
      snprintf(month_str, sizeof(month_str), "Jan");
      break;
    case 2:
      snprintf(month_str, sizeof(month_str), "Feb");
      break;
    case 3:
      snprintf(month_str, sizeof(month_str), "Mar");
      break;
    case 4:
      snprintf(month_str, sizeof(month_str), "Apr");
      break;
    case 5:
      snprintf(month_str, sizeof(month_str), "May");
      break;
    case 6:
      snprintf(month_str, sizeof(month_str), "Jun");
      break;
    case 7:
      snprintf(month_str, sizeof(month_str), "Jul");
      break;
    case 8:
      snprintf(month_str, sizeof(month_str), "Aug");
      break;
    case 9:
      snprintf(month_str, sizeof(month_str), "Sep");
      break;
    case 10:
      snprintf(month_str, sizeof(month_str), "Oct");
      break;
    case 11:
      snprintf(month_str, sizeof(month_str), "Nov");
      break;
    case 12:
      snprintf(month_str, sizeof(month_str), "Dec");
      break;
    default:
      snprintf(month_str, sizeof(month_str), "UNK");
      break;
  }
  //return month_str;
}

namespace Retro {
char DOW_NAMES[7][4] = { 
  {'S','u','n','\0'}, 
  {'M','o','n','\0'},
  {'T','u','e','\0'},
  {'W','e','d','\0'},
  {'T','h','u','\0'},
  {'F','r','i','\0'},
  {'S','a','t','\0'},
};

class Watch
{
  private:
    TTGOClass* ttgo;
  public:
    Watch() 
      : m_Delay(5) 
    {}

    static Watch* Create() { return new Watch(); }
    
    void Init()
    {
      Serial.begin(115200);
      Serial.printf("Watch Init.\n");
      ttgo = TTGOClass::getWatch();
      Serial.printf("Watch Init..\n");
      ttgo->begin();
      Serial.printf("Watch Init...\n");
      // this doesn't work for enabling sound
      // think it's from another chip combo
      //ttgo->enableLDO3();      
      ttgo->lvgl_begin();
      ttgo->openBL();
      //SPIFFS.begin(true, "/fs");
      //! begin motor attach to 33 pin , In TWatch-2020 it is IO4
      //ttgo->motor_begin();
      //attach touch screen interrupt pin
      //pinMode(TP_INT, INPUT);
      // if UpdateAll() not called, the GUI looks all screwy??
      UpdateAll();
      SetupGUI();
      Serial.printf("Setup Complete");
    }
    // clock
    char* GetHour() { return hh_str; }
    char* GetMinute() { return mm_str; }
    char* GetSecond() { return ss_str; }
    char* GetYear() { return year_str; }
    char* GetMonth() { return month_str; }
    char* GetDay() { return day_str; }
    char* GetDOW() { return DOW_NAMES[dow]; } //dow_str; }
    // file system
    size_t GetTotalBytes() { return m_TotalBytes; }
    size_t GetUsedBytes() { return m_UsedBytes; }
    size_t GetFreeBytes() { return m_FreeBytes; }
    char* GetFreeBytesStr() { return m_FreeBytesStr; }
    // power
    float GetBusVolt() { return m_BusVolt; }
    char* GetBusVoltStr() { return m_BusVoltStr; }
    char* GetBusCurrStr() { return m_BusCurrStr; }
    float GetBattVolt() { return m_BattVolt; }
    char* GetBattVoltStr() { return m_BattVoltStr; }
    char* GetBattPercentStr() { return m_BattPercentStr; }
    // memory
    uint32_t GetFreeHeap() { return m_FreeHeap; }
    char* GetFreeHeapStr() { return m_FreeHeapStr; }
    // esp_get_minimum_free_heap_size()

    void TaskHandler()
    {
      lv_task_handler();
      /*
      if (digitalRead(TP_INT) == LOW) {
        ttgo->motor->onec();
        delay(200);
      }
      */
    }

    void Delay() const { delay(m_Delay); }
    
    void UpdateAll()
    {
      UpdateClock();
      UpdatePower();
      UpdateFileSystem();
      UpdateMemory();
    }

    bool IsNextSecond()
    {
      tnow = ttgo->rtc->getDateTime();
      if (tnow.second != m_SaveSecond)
      {
        m_SaveSecond = tnow.second;
        if (blink) {
          //lv_style_set_text_color(&dot_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
          lv_obj_set_style_local_text_color(dot, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
          blink = false;
        } else {
          //lv_style_set_text_color(&dot_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
          lv_obj_set_style_local_text_color(dot, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
          blink = true;
        }
        return true;
      }
      return false;
    }

     
    void UpdateClock()
    {
      dow = m_GetDOW(tnow.day, tnow.month, tnow.year);
      //snprintf(dow_str, sizeof(dow_str), "%s", DOW_NAMES[dow]);
      uint8_t hh = tnow.hour;
      if (hh > 12) hh -= 12;
      snprintf(hh_str, sizeof(hh_str), "%02d", hh);
      snprintf(mm_str, sizeof(mm_str), "%02d", tnow.minute);
      snprintf(ss_str, sizeof(ss_str), "%02d", tnow.second);
      snprintf(year_str, sizeof(year_str), "%d", tnow.year);
      GetMonthAbbr(month_str, tnow.month);
      snprintf(day_str, sizeof(day_str), "%d", tnow.day);
    }

    void UpdateFileSystem()
    {
      m_TotalBytes = SPIFFS.totalBytes();
      m_UsedBytes = SPIFFS.usedBytes();
      m_FreeBytes = m_TotalBytes - m_UsedBytes;

      snprintf(m_FreeBytesStr, sizeof(m_FreeBytesStr), "%d %d", m_TotalBytes, m_UsedBytes);     
    }
    
    void UpdatePower()
    {
       //ttgo->power->readIRQ();
      m_BusVolt = ttgo->power->getVbusVoltage();
      m_BusCurr = ttgo->power->getVbusCurrent();
      m_BattVolt = ttgo->power->getBattVoltage();
      m_BattPercent = ttgo->power->getBattPercentage();
    
      snprintf(m_BusVoltStr, sizeof(m_BusVoltStr), "%.2f", m_BusVolt);
      snprintf(m_BusCurrStr, sizeof(m_BusCurrStr), "%.2f", m_BusCurr);
      snprintf(m_BattVoltStr, sizeof(m_BattVoltStr), "%.2f", m_BattVolt);
      snprintf(m_BattPercentStr, sizeof(m_BattPercentStr), "%3d%%", m_BattPercent);
    }

    void UpdateMemory()
    {
      m_FreeHeap = esp_get_minimum_free_heap_size();
      snprintf(m_FreeHeapStr, sizeof(m_FreeHeapStr), "%d", m_FreeHeap);
    }
  private:
    bool blink = true;
    uint32_t m_Delay;
    // memory
    // file system
    size_t m_TotalBytes, m_UsedBytes, m_FreeBytes;
    char m_FreeBytesStr[32] = {'\0'};
    // clock (rtc)
    RTC_Date tnow;
    uint8_t m_SaveSecond = 0;
    uint8_t dow;
    char hh_str[3] = {'\0'}, mm_str[3] = {'\0'}, ss_str[3] = {'\0'};
    char year_str[5] = {'\0'}, month_str[4] = {'\0'}, day_str[3] = {'\0'};
    char dow_str[4] = {'\0'};
    // power
    float m_BusVolt;
    float m_BusCurr;
    float m_BattVolt;
    int8_t m_BattPercent;

    char m_BusVoltStr[12] = {'\0'};
    char m_BusCurrStr[12] = {'\0'};
    char m_BattVoltStr[12] = {'\0'};
    char m_BattPercentStr[6] = {'\0'};

    // memory

    uint32_t m_FreeHeap;
    char m_FreeHeapStr[16] = {'\0'};

    uint8_t m_GetDOW(uint8_t day, uint8_t month, uint16_t year)
    {
      uint8_t val;
      if (month < 3) {
        month = 12u + month;
        year--;
      }
      val = (day + (((month + 1u) * 26u) / 10u) + year + (year / 4u) + (6u * (year / 100u)) + (year / 400u)) % 7u;
      if (0u == val) val = 7;
      return (val - 1);
    }
  private:
        lv_obj_t* model;       
        lv_obj_t *row_down_line;
        lv_obj_t *casio;
        lv_obj_t *light_text;
        lv_obj_t *hour;
        lv_obj_t *dot;
        lv_obj_t *second;
        lv_obj_t *minute;
        lv_obj_t *month;
        lv_obj_t *year;
        lv_obj_t *state;
        lv_obj_t *week;
        lv_obj_t *day;
        lv_obj_t *bat_text;
        lv_obj_t *temp_text;
        lv_obj_t *bat;
/*
        float Round(float var) 
        { 
          float value = (int)(var * 100 + .5); 
          return (float)value / 100; 
        }
        */
  public:
        void UpdateGUI()
        {
          lv_label_set_text(model, GetFreeHeapStr());         
          lv_label_set_text(row_down_line, GetFreeBytesStr()); //"Designed by Casey");
          //lv_label_set_text(casio, "MCP");
          //lv_label_set_text(light_text, "LIGHT");
          lv_label_set_text(hour, GetHour()); //"10"); hh_str
          //lv_label_set_text(dot, ":");
          lv_label_set_text(minute, GetMinute()); // mm_str
          lv_label_set_text(second, GetSecond());
          lv_label_set_text(month, GetMonth()); // "Dec"
          //lv_label_set_text(state, "ST");
          lv_label_set_text(week, GetDOW());
          lv_label_set_text(day, GetDay()); // "3"
          lv_label_set_text(bat_text, GetBusVoltStr()); // "BAT"
          lv_label_set_text(temp_text, GetBattVoltStr()); //"24*C");
          lv_label_set_text(bat, GetBattPercentStr()); //"100%");
        }
        
        void SetupGUI()
        {
              static lv_style_t cont_style;
    lv_style_init(&cont_style);
    lv_style_init(&cont_style);
    lv_style_set_radius(&cont_style, LV_OBJ_PART_MAIN, 12);
    lv_style_set_bg_color(&cont_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    lv_style_set_bg_opa(&cont_style, LV_OBJ_PART_MAIN, LV_OPA_COVER);
    lv_style_set_border_width(&cont_style, LV_OBJ_PART_MAIN, 0);

    lv_obj_t *view = lv_cont_create(lv_scr_act(), nullptr);
    lv_obj_set_size(view, 240, 240);
    lv_obj_add_style(view, LV_OBJ_PART_MAIN, &cont_style);

    static lv_style_t onestyle;
    lv_style_init(&onestyle);
    lv_style_set_text_color(&onestyle, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&onestyle, LV_STATE_DEFAULT, &fn1_32);

    //Upper left corner logo
    casio = lv_label_create(view, nullptr);
    lv_obj_add_style(casio, LV_OBJ_PART_MAIN, &onestyle);
    lv_label_set_text(casio, "MCP");
    lv_obj_align(casio, view, LV_ALIGN_IN_TOP_LEFT, 10, 10);

    //Upper right corner model
    static lv_style_t model_style;
    lv_style_init(&model_style);
    lv_style_set_text_color(&model_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&model_style, LV_STATE_DEFAULT, &robot_ightItalic_16);

    model = lv_label_create(view, nullptr);
    lv_obj_add_style(model, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(model, GetFreeHeapStr()); //"ESP32-S2");
    lv_obj_align(model, view, LV_ALIGN_IN_TOP_RIGHT, -10, 15);


    //Line style
    static lv_style_t line_style;
    lv_style_init(&line_style);
    lv_style_set_line_color(&line_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_line_width(&line_style, LV_STATE_DEFAULT, 2);
    lv_style_set_line_rounded(&line_style, LV_STATE_DEFAULT, 1);

    //Top horizontal line
    static lv_point_t line_points[] = { {10, 0}, {225, 0} }; // 230?
    lv_obj_t *line1 = lv_line_create(view, NULL);
    lv_line_set_points(line1, line_points, 2);     /*Set the points*/
    lv_obj_add_style(line1, LV_OBJ_PART_MAIN, &line_style);
    lv_obj_align(line1, NULL, LV_ALIGN_IN_TOP_MID, 0, 45);

    //Slogan below the top horizontal line
    row_down_line = lv_label_create(view, nullptr);
    lv_obj_add_style(row_down_line, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(row_down_line, GetFreeBytesStr()); //"Designed by Casey");
    lv_obj_align(row_down_line, line1, LV_ALIGN_OUT_BOTTOM_LEFT, 10, 2);


    //Bottom horizontal line left
    static lv_point_t line_points1[] = { {0, 0}, {75, 0} };
    lv_obj_t *line2 = lv_line_create(view, NULL);
    lv_line_set_points(line2, line_points1, 2);     /*Set the points*/
    lv_obj_add_style(line2, LV_OBJ_PART_MAIN, &line_style);
    lv_obj_align(line2, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 10, -30);

    //! Bottom line right
    static lv_point_t line_points2[] = { {0, 0}, {75, 0} };
    lv_obj_t *line3 = lv_line_create(view, NULL);
    lv_line_set_points(line3, line_points2, 2);     /*Set the points*/
    lv_obj_add_style(line3, LV_OBJ_PART_MAIN, &line_style);
    lv_obj_align(line3, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -30);

    // Below the horizontal line in the upper left corner
    static lv_style_t text_style;
    lv_style_init(&text_style);
    lv_style_set_text_color(&text_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&text_style, LV_STATE_DEFAULT, &robot_ightItalic_16);

    //! light
    light_text = lv_label_create(view, nullptr);
    lv_obj_add_style(light_text, LV_OBJ_PART_MAIN, &text_style);
    lv_label_set_text(light_text, "LIGHT");
    lv_obj_align(light_text, line1, LV_ALIGN_OUT_BOTTOM_RIGHT, -30, 5);

    //! arrow -> right
    lv_obj_t *img1 = lv_img_create(view, NULL);
    lv_img_set_src(img1, &arrow_right_png);
    lv_obj_align(img1, line1, LV_ALIGN_OUT_BOTTOM_RIGHT, -10, 5);

    //! arrow down -> left
    lv_obj_t *img2 = lv_img_create(view, NULL);
    lv_img_set_src(img2, &arrow_left_png);
    lv_obj_align(img2, line2, LV_ALIGN_OUT_TOP_LEFT, 0, -5);

    //! arrow down -> right
    lv_obj_t *img3 = lv_img_create(view, NULL);
    lv_img_set_src(img3, &arrow_right_png);
    lv_obj_align(img3, line3, LV_ALIGN_OUT_TOP_RIGHT, 0, -5);

    //Intermediate clock time division font
    static lv_style_t time_style;
    lv_style_init(&time_style);
    lv_style_set_text_color(&time_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&time_style, LV_STATE_DEFAULT, &digital_play_st_48);

    
    hour = lv_label_create(view, nullptr);
    lv_obj_add_style(hour, LV_OBJ_PART_MAIN, &time_style);
    lv_label_set_text(hour, GetHour()); //"10"); hh_str
    lv_obj_align(hour, view, LV_ALIGN_CENTER, -50, 10);

    //-- semicolon ------------------------------------------
    static lv_style_t dot_style;
    lv_style_init(&dot_style);
    lv_style_set_text_color(&dot_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&dot_style, LV_STATE_DEFAULT, &liquidCrystal_nor_64);

    dot = lv_label_create(view, nullptr);
    lv_obj_add_style(dot, LV_OBJ_PART_MAIN, &dot_style);
    lv_label_set_recolor(dot, true);
    lv_label_set_text(dot, ":");
    lv_obj_align(dot, hour, LV_ALIGN_OUT_RIGHT_MID, 5, -10);

    //minute
    minute = lv_label_create(view, nullptr);
    lv_obj_add_style(minute, LV_OBJ_PART_MAIN, &time_style);
    lv_label_set_text(minute, GetMinute()); // mm_str
    lv_obj_align(minute, hour, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

    //Intermediate clock second digit
    static lv_style_t second_style;
    lv_style_init(&second_style);
    lv_style_set_text_color(&second_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&second_style, LV_STATE_DEFAULT, &liquidCrystal_nor_32);

    second = lv_label_create(view, nullptr);
    lv_obj_add_style(second, LV_OBJ_PART_MAIN, &second_style);
    lv_label_set_text(second, GetSecond()); // ss_str
    lv_obj_align(second, minute, LV_ALIGN_OUT_RIGHT_BOTTOM, 4, -10); // 10, -10

    //date
    static lv_style_t year_style;
    lv_style_init(&year_style);
    lv_style_set_text_color(&year_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&year_style, LV_STATE_DEFAULT, &liquidCrystal_nor_24);


    year = lv_label_create(view, nullptr);
    lv_obj_add_style(year, LV_OBJ_PART_MAIN, &year_style);
    lv_label_set_text(year, GetYear());
    lv_obj_align(year, view, LV_ALIGN_CENTER, 0, 55);

    
    month = lv_label_create(view, nullptr);
    lv_obj_add_style(month, LV_OBJ_PART_MAIN, &year_style); //chinese_style);
    lv_label_set_text(month, GetMonth()); // "Dec"
    lv_obj_align(month, year, LV_ALIGN_OUT_LEFT_MID, -5, 0);

    state = lv_label_create(view, nullptr);
    lv_obj_add_style(state, LV_OBJ_PART_MAIN, &year_style); //chinese_style);
    lv_label_set_text(state, "ST");
    lv_obj_align(state, year, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    week = lv_label_create(view, nullptr);
    lv_obj_add_style(week, LV_OBJ_PART_MAIN, &year_style); //&chinese_style);
    lv_label_set_text(week, GetDOW());
    lv_obj_align(week, view, LV_ALIGN_CENTER, 45, -35);
   
    day = lv_label_create(view, nullptr);
    lv_obj_add_style(day, LV_OBJ_PART_MAIN, &year_style);
    lv_label_set_text(day, GetDay()); // "3"
    lv_obj_align(day, week, LV_ALIGN_OUT_RIGHT_MID, 10, 0); // 10




    
    //! Battery string
    bat_text = lv_label_create(view, nullptr);
    lv_obj_add_style(bat_text, LV_OBJ_PART_MAIN, &text_style);
    lv_label_set_text(bat_text, GetBusVoltStr()); // "BAT"
    lv_obj_align(bat_text, view, LV_ALIGN_IN_LEFT_MID, 20, -40);

    // temperature
    static lv_style_t temp_style;
    lv_style_init(&temp_style);
    lv_style_set_text_color(&temp_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&temp_style, LV_STATE_DEFAULT, &quostige_16);


    temp_text = lv_label_create(view, nullptr);
    lv_obj_add_style(temp_text, LV_OBJ_PART_MAIN, &temp_style);
    lv_label_set_text(temp_text, GetBattVoltStr()); //"24*C");
    lv_obj_align(temp_text, bat_text, LV_ALIGN_OUT_BOTTOM_MID, 0, -5);

    //Power
    static lv_style_t bat_style;
    lv_style_init(&bat_style);
    lv_style_set_text_color(&bat_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&bat_style, LV_STATE_DEFAULT, &digital_play_st_24);



    bat = lv_label_create(view, nullptr);
    lv_obj_add_style(bat, LV_OBJ_PART_MAIN, &bat_style);
    lv_label_set_text(bat, GetBattPercentStr()); //"100%");
    lv_obj_align(bat, view, LV_ALIGN_CENTER, -10, -35); // -35, -35



/*
    static lv_point_t line_points3[] = {
        {0, 0}, {50, 0},
        {50, 30}, {50, 30},
        {50, 35}, {35, 39},
        {35, 39}, {15, 39},
        {15, 39}, {0, 35},
        {0, 35}, {0, 0},
    };
    */
    static lv_point_t line_points3[] = {
        {0, 0}, {50, 0},
        {50, 0}, {50, 30},
        {50, 30}, {0, 30},
        {0, 30}, {0, 0}
    };

    lv_obj_t *line4;
    line4 = lv_line_create(view, NULL);
    lv_line_set_points(line4, line_points3, sizeof(line_points3) / sizeof(line_points3[0]));
    lv_obj_add_style(line4, LV_OBJ_PART_MAIN, &line_style);
    lv_obj_align(line4, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -7); // 0, -2
    
    // Bottom Go
    static lv_style_t key_style;
    lv_style_init(&key_style);
    lv_style_set_text_color(&key_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&key_style, LV_STATE_DEFAULT, &gracetians_32);


    lv_obj_t *key = lv_label_create(view, nullptr);
    lv_obj_add_style(key, LV_OBJ_PART_MAIN, &key_style);
    lv_label_set_text(key, "Go");
    lv_obj_align(key, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);


    static lv_style_t bot_style;
    lv_style_init(&bot_style);
    lv_style_set_text_color(&bot_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&bot_style, LV_STATE_DEFAULT, &exninja_22);


    lv_obj_t *str1 = lv_label_create(view, nullptr);
    lv_obj_add_style(str1, LV_OBJ_PART_MAIN, &bot_style);
    lv_label_set_text(str1, "WATER");
    lv_obj_align(str1, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 10, -2);

    lv_obj_t *str2 = lv_label_create(view, nullptr);
    lv_obj_add_style(str2, LV_OBJ_PART_MAIN, &bot_style);
    lv_label_set_text(str2, "RESIST");
    lv_obj_align(str2, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -2);

    
        }
    
    
};

} // namespace Retro
