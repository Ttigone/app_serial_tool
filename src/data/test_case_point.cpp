#include "TestCasePoint.h"

TestCasePoint::TestCasePoint() {
  time = 200;
  ppm_acc_time = 1000;

  send_ppm = 9600;

  recv_ppm_min = 6000;
  recv_ppm_max = 25000;

  speed_min = -32768;
  speed_max = 32767;

  current_min = -32768;
  current_max = 32767;

  voltage_min = -32768;
  voltage_max = 32767;

  temp_mos_min = -550;
  temp_mos_max = 1500;

  temp_cap_min = -550;
  temp_cap_max = 1500;

  temp_mcu_min = -550;
  temp_mcu_max = 1500;
}

void TestCasePoint::JudgeStatusParam(int test_index,
                                     EscRealtimeDataParam* realtime_data,
                                     QString& error_info, quint16& error_flag) {
  if (realtime_data->recv_pwm < recv_ppm_min) {
    if (!(error_flag & 0x0001))
      error_info +=
          QString(QObject::tr("Test Point %1 : recv_pwm %2 is less than %3\n"))
              .arg(test_index)
              .arg(realtime_data->recv_pwm)
              .arg(recv_ppm_min);

    error_flag |= 0x0001;
  } else if (realtime_data->recv_pwm > recv_ppm_max) {
    if (!(error_flag & 0x0002))
      error_info +=
          QString(
              QObject::tr("Test Point %1 : recv_pwm %2 is greater than %3\n"))
              .arg(test_index)
              .arg(realtime_data->recv_pwm)
              .arg(recv_ppm_max);

    error_flag |= 0x0002;
  }

  if (realtime_data->speed < speed_min) {
    if (!(error_flag & 0x0004))
      error_info +=
          QString(QObject::tr("Test Point %1 : speed %2 is less than %3\n"))
              .arg(test_index)
              .arg(realtime_data->speed)
              .arg(speed_min);

    error_flag |= 0x0004;
  } else if (realtime_data->speed > speed_max) {
    if (!(error_flag & 0x0008))
      error_info +=
          QString(QObject::tr("Test Point %1 : speed %2 is greater than %3\n"))
              .arg(test_index)
              .arg(realtime_data->speed)
              .arg(speed_max);

    error_flag |= 0x0008;
  }

  if (realtime_data->current < current_min) {
    if (!(error_flag & 0x0010))
      error_info +=
          QString(QObject::tr("Test Point %1 : current %2 is less than %3\n"))
              .arg(test_index)
              .arg(realtime_data->current)
              .arg(current_min);

    error_flag |= 0x0010;
  } else if (realtime_data->current > current_max) {
    if (!(error_flag & 0x0020))
      error_info +=
          QString(
              QObject::tr("Test Point %1 : current %2 is greater than %3\n"))
              .arg(test_index)
              .arg(realtime_data->current)
              .arg(current_max);

    error_flag |= 0x0020;
  }

  if (realtime_data->voltage < voltage_min) {
    if (!(error_flag & 0x0040))
      error_info +=
          QString(QObject::tr("Test Point %1 : voltage %2 is less than %3\n"))
              .arg(test_index)
              .arg(realtime_data->voltage)
              .arg(voltage_min);

    error_flag |= 0x0040;
  } else if (realtime_data->voltage > voltage_max) {
    if (!(error_flag & 0x0080))
      error_info +=
          QString(
              QObject::tr("Test Point %1 : voltage %2 is greater than %3\n"))
              .arg(test_index)
              .arg(realtime_data->voltage)
              .arg(voltage_max);

    error_flag |= 0x080;
  }

  if (realtime_data->mos_temp < temp_mos_min) {
    if (!(error_flag & 0x0100))
      error_info +=
          QString(QObject::tr("Test Point %1 : mos_temp %2 is less than %3\n"))
              .arg(test_index)
              .arg(realtime_data->mos_temp)
              .arg(temp_mos_min);

    error_flag |= 0x0100;
  } else if (realtime_data->mos_temp > temp_mos_max) {
    if (!(error_flag & 0x0200))
      error_info +=
          QString(
              QObject::tr("Test Point %1 : mos_temp %2 is  greater than %3\n"))
              .arg(test_index)
              .arg(realtime_data->mos_temp)
              .arg(temp_mos_max);

    error_flag |= 0x0200;
  }

  if (realtime_data->cap_temp < temp_cap_min) {
    if (!(error_flag & 0x0400))
      error_info +=
          QString(QObject::tr("Test Point %1 : cap_temp %2 is less than %3\n"))
              .arg(test_index)
              .arg(realtime_data->cap_temp)
              .arg(temp_cap_min);

    error_flag |= 0x0400;
  } else if (realtime_data->cap_temp > temp_cap_max) {
    if (!(error_flag & 0x0800))
      error_info +=
          QString(QObject::tr("Test Point %1 : cap_temp %2 greater than %3\n"))
              .arg(test_index)
              .arg(realtime_data->cap_temp)
              .arg(temp_cap_max);

    error_flag |= 0x0800;
  }

  if (realtime_data->mcu_temp < temp_mcu_min) {
    if (!(error_flag & 0x1000))
      error_info +=
          QString(QObject::tr("Test Point %1 : mcu_temp %2 is less than %3\n"))
              .arg(test_index)
              .arg(realtime_data->mcu_temp)
              .arg(temp_mcu_min);

    error_flag |= 0x1000;
  } else if (realtime_data->mcu_temp > temp_mcu_max) {
    if (!(error_flag & 0x2000))
      error_info +=
          QString(
              QObject::tr("Test Point %1 : mcu_temp %2 is greater than %3\n"))
              .arg(test_index)
              .arg(realtime_data->mcu_temp)
              .arg(temp_mcu_max);

    error_flag |= 0x2000;
  }
}
