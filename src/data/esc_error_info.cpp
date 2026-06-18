#include "EscErrorInfo.h"

EscErrorInfo::EscErrorInfo() { initErrorInfo(); }

EscErrorInfo::~EscErrorInfo() {}

void EscErrorInfo::initErrorInfo() {
  m_SelfCheckErrorInfo.clear();
  m_RunningErrorInfo.clear();

  ErrorInfo info;

  // 自检错误
  info.flag_bit = 0x0001;
  info.error_message = tr("The output current of the motor  is abnormal");
  info.error_analysis =
      tr("Hardware failure: chip problem / short circuit welding / virtual "
         "soldering");
  m_SelfCheckErrorInfo.append(info);

  info.flag_bit = 0x0002;
  info.error_message = tr("Motor line current output is abnormal");
  info.error_analysis =
      tr("Hardware failure: resistance problem / short circuit welding / "
         "virtual soldering");
  m_SelfCheckErrorInfo.append(info);

  info.flag_bit = 0x0004;
  info.error_message = tr("Motor line voltage is abnormal");
  info.error_analysis =
      tr("Hardware failure: resistance problem / short circuit welding / "
         "virtual soldering");
  m_SelfCheckErrorInfo.append(info);

  info.flag_bit = 0x0008;
  info.error_message = tr("Electrolytic capacitor NTC output is abnormal");
  info.error_analysis =
      tr("Hardware failure: resistance problem / short circuit welding / "
         "virtual soldering");
  m_SelfCheckErrorInfo.append(info);

  info.flag_bit = 0x0010;
  info.error_message = tr("MOS/NTC abnormal output");
  info.error_analysis =
      tr("Hardware failure: resistance problem / short circuit welding / "
         "virtual soldering");
  m_SelfCheckErrorInfo.append(info);

  info.flag_bit = 0x0020;
  info.error_message = tr("Overvoltage or undervoltage of bus voltage");
  info.error_analysis = tr("The supply voltage is too high or too low.");
  m_SelfCheckErrorInfo.append(info);

  info.flag_bit = 0x0040;
  info.error_message = tr("Abnormal bus voltage sampling");
  info.error_analysis =
      tr("Hardware failure: resistance problem / short circuit welding / "
         "virtual soldering");
  m_SelfCheckErrorInfo.append(info);

  info.flag_bit = 0x0080;
  info.error_message = tr("Motor circuit impedance is too small");
  info.error_analysis =
      tr("The motor line loop is short-circuited or the motor coil is "
         "short-circuited.");
  m_SelfCheckErrorInfo.append(info);

  info.flag_bit = 0x0100;
  info.error_message = tr("Motor circuit impedance is too large");
  info.error_analysis =
      tr("The motor line is not in good contact or the motor coil has an open "
         "circuit.");
  m_SelfCheckErrorInfo.append(info);

  info.flag_bit = 0x0200;
  info.error_message = tr("Motor line voltage detection circuit is abnormal");
  info.error_analysis =
      tr("Hardware failure: resistance problem / short circuit welding / "
         "virtual soldering");
  m_SelfCheckErrorInfo.append(info);

  info.flag_bit = 0x0400;
  info.error_message = tr("Motor line voltage detection circuit is abnormal");
  info.error_analysis =
      tr("Hardware failure: resistance problem / short circuit welding / "
         "virtual soldering");
  m_SelfCheckErrorInfo.append(info);

  info.flag_bit = 0x0800;
  info.error_message = tr("Motor line current output is abnormal");
  info.error_analysis =
      tr("Hardware failure: resistance problem / short circuit welding / "
         "virtual soldering");
  m_SelfCheckErrorInfo.append(info);

  info.flag_bit = 0x1000;
  info.error_message = tr("ADC 5V error");
  info.error_analysis = tr("ADC 5V offset is out of range.");
  m_SelfCheckErrorInfo.append(info);

  info.flag_bit = 0x2000;
  info.error_message =
      tr("The software and hardware versions are incompatible");
  info.error_analysis = tr("The firmware is not burned correctly.");
  m_SelfCheckErrorInfo.append(info);

  info.flag_bit = 0x4000;
  info.error_message = tr("Bootloader nonsupport");
  info.error_analysis = tr("The firmware is not burned correctly.");
  m_SelfCheckErrorInfo.append(info);

  info.flag_bit = 0x8000;
  info.error_message = tr("ADC 15V error");
  info.error_analysis = tr("ADC 15V offset is out of range.");
  m_SelfCheckErrorInfo.append(info);

  // running 错误
  info.flag_bit = 0x0001;
  info.error_message = tr("Overcurrent / short circuit protection");
  info.error_analysis = tr("The motor wire is short-circuited or blocked");
  m_RunningErrorInfo.append(info);

  info.flag_bit = 0x0002;
  info.error_message = tr("Stall protection");
  info.error_analysis = tr("The motor is locked");
  m_RunningErrorInfo.append(info);

  info.flag_bit = 0x0004;
  info.error_message = tr("Over Temp");
  info.error_analysis = tr("The ESC temp is too High, need stop working");
  m_RunningErrorInfo.append(info);

  info.flag_bit = 0x0008;
  info.error_message = tr("PWM Throttle missing pulse");
  info.error_analysis =
      tr("The PWM throttle cable is in poor contact, or the throttle cable is "
         "disturbed.");
  m_RunningErrorInfo.append(info);

  info.flag_bit = 0x0010;
  info.error_message = tr("No load");
  info.error_analysis = tr("The motor is not connected to the load");
  m_RunningErrorInfo.append(info);

  info.flag_bit = 0x0020;
  info.error_message = tr("Throttle saturation");
  info.error_analysis =
      tr("Throttle saturation appears, indicating that the voltage is low or "
         "the motor KV is not enough.");
  m_RunningErrorInfo.append(info);

  info.flag_bit = 0x0040;
  info.error_message = tr("Over Voltage");
  info.error_analysis =
      tr("Motor deceleration causes an increase in bus voltage");
  m_RunningErrorInfo.append(info);

  info.flag_bit = 0x0080;
  info.error_message = tr("Voltage fall");
  info.error_analysis = tr(
      "The ESC power plug is loose, causing the voltage to drop abnormally.");
  m_RunningErrorInfo.append(info);

  info.flag_bit = 0x0100;
  info.error_message = tr("Comm throttle lost");
  info.error_analysis = tr("Comm throttle transmission frequency is unstable");
  m_RunningErrorInfo.append(info);
}

QString EscErrorInfo::getSelfCheckErrorInfo(quint16 error_code) {
  QString info_list;
  info_list = "";

  for (int i = 0; i < m_SelfCheckErrorInfo.size(); i++) {
    if (error_code & m_SelfCheckErrorInfo[i].flag_bit) {
      QString error_msg = tr("Error Info : ");
      error_msg += m_SelfCheckErrorInfo[i].error_message;

      QString error_analysis = tr("Error Analysis : ");
      error_analysis += m_SelfCheckErrorInfo[i].error_analysis;

      info_list.append(error_msg);
      info_list.append("<br/>");

      info_list.append(error_analysis);
      info_list.append("<br/>");
      info_list.append("<br/>");
    }
  }

  return info_list;
}

QString EscErrorInfo::getRunningErrorInfo(quint16 error_code) {
  QString info_list;
  info_list = "";
  for (int i = 0; i < m_RunningErrorInfo.size(); i++) {
    if (error_code & m_RunningErrorInfo[i].flag_bit) {
      QString error_msg = tr("Error Info : ");
      error_msg += m_RunningErrorInfo[i].error_message;

      QString error_analysis = tr("Error Analysis : ");
      error_analysis += m_RunningErrorInfo[i].error_analysis;

      info_list.append(error_msg);
      info_list.append("<br/>");
      info_list.append(error_analysis);
      info_list.append("<br/>");
      info_list.append("<br/>");
    }
  }

  return info_list;
}
