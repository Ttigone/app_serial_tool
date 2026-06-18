#pragma once

#include "DataType.h"

class TestCasePoint {
 public:
  TestCasePoint();

  void JudgeStatusParam(int test_index, EscRealtimeDataParam* realtime_data,
                        QString& error_info, quint16& error_flag);

  int time;
  int ppm_acc_time;
  int send_ppm;

  int recv_ppm_min;
  int recv_ppm_max;

  int speed_min;
  int speed_max;

  int current_min;
  int current_max;

  int voltage_min;
  int voltage_max;

  int temp_mos_min;
  int temp_mos_max;

  int temp_cap_min;
  int temp_cap_max;

  int temp_mcu_min;
  int temp_mcu_max;
};
