#pragma once

#include <QObject>

struct EscRealtimeDataParam {
  Q_GADGET
  Q_PROPERTY(quint16 recv_pwm MEMBER recv_pwm)
  Q_PROPERTY(quint16 comm_pwm MEMBER comm_pwm)

  Q_PROPERTY(qint32 speed MEMBER speed)

  Q_PROPERTY(qint16 current MEMBER current)
  Q_PROPERTY(qint16 bus_current MEMBER bus_current)

  Q_PROPERTY(quint16 voltage MEMBER voltage)
  Q_PROPERTY(quint16 v_modulation MEMBER v_modulation)

  Q_PROPERTY(qint16 mos_temp MEMBER mos_temp)
  Q_PROPERTY(qint16 cap_temp MEMBER cap_temp)
  Q_PROPERTY(qint16 mcu_temp MEMBER mcu_temp)
  Q_PROPERTY(qint16 motor_temp MEMBER motor_temp)

  Q_PROPERTY(quint16 running_error MEMBER running_error)
  Q_PROPERTY(quint16 selfcheck_error MEMBER selfcheck_error)

  Q_PROPERTY(quint16 time_10ms MEMBER time_10ms)

  Q_PROPERTY(qint16 speed_km_h MEMBER speed_km_h)

 public:
  EscRealtimeDataParam() {
    recv_pwm = 0;
    comm_pwm = 0;

    speed = 0;

    current = 0;
    bus_current = 0;

    voltage = 0;
    v_modulation = 0;

    mos_temp = 0;
    cap_temp = 0;
    mcu_temp = 0;

    running_error = 0;
    selfcheck_error = 0;

    time_10ms = 0;
    motor_temp = 0;

    speed_km_h = 0;
  }

  // 电调状态
  quint16 recv_pwm;
  quint16 comm_pwm;

  qint32 speed;

  qint16 current;
  qint16 bus_current;

  quint16 voltage;
  quint16 v_modulation;

  qint16 mos_temp;
  qint16 cap_temp;
  qint16 mcu_temp;
  qint16 motor_temp;

  quint16 running_error;
  quint16 selfcheck_error;
  quint16 time_10ms;

  qint16 speed_km_h;
};
Q_DECLARE_METATYPE(EscRealtimeDataParam)

struct EscInfoParam {
  Q_GADGET

  Q_PROPERTY(QString hardware_id MEMBER hardware_id)
  Q_PROPERTY(QString bootloader_version MEMBER bootloader_version)
  Q_PROPERTY(QString app_version MEMBER app_version)
  Q_PROPERTY(QString motor_id MEMBER motor_id)
  Q_PROPERTY(QString param_version MEMBER param_version)

  Q_PROPERTY(QString sn_code MEMBER sn_code)
  Q_PROPERTY(QString history_error_code MEMBER history_error_code)

  Q_PROPERTY(QString firmware_name MEMBER firmware_name)

 public:
  EscInfoParam() {
    hardware_id = "";
    bootloader_version = "";
    app_version = "";
    motor_id = "";
    param_version = "";

    sn_code = "";
    history_error_code = "";

    firmware_name = "";
  }
  QString hardware_id;
  QString bootloader_version;
  QString app_version;
  QString motor_id;
  QString param_version;

  QString sn_code;
  QString history_error_code;

  QString firmware_name;
};
Q_DECLARE_METATYPE(EscInfoParam)

struct ConfigInfoParam {
  Q_GADGET

  Q_PROPERTY(int set_config_id MEMBER set_config_id)
  Q_PROPERTY(int set_config_result MEMBER set_config_result)
  Q_PROPERTY(int get_config_id MEMBER get_config_id)
  Q_PROPERTY(int get_config_result MEMBER get_config_result)
  Q_PROPERTY(int save_config_result MEMBER save_config_result)

  Q_PROPERTY(int private_cmd_result MEMBER private_cmd_result)

 public:
  ConfigInfoParam() {
    set_config_id = -1;
    set_config_result = -1;

    get_config_id = -1;
    get_config_result = -1;

    save_config_result = -1;

    private_cmd_result = -1;
  }

  int set_config_id;
  int set_config_result;  //-1,通信故障, 0,成功, 1, 不支持该参数, 2,数据格式出错

  int get_config_id;
  int get_config_result;  //-1,通信故障, 0,成功, 1, 不支持该参数, 2,数据格式出错

  int save_config_result;

  int private_cmd_result;  //-1,通信故障, 0,成功, 1,密码错误
};
Q_DECLARE_METATYPE(ConfigInfoParam)

struct BleInfoParam {
  Q_GADGET
  Q_PROPERTY(QString sn_code MEMBER sn_code)
  Q_PROPERTY(QString soft_version MEMBER soft_version)
  Q_PROPERTY(QString hardware_version MEMBER hardware_version)
  Q_PROPERTY(QString bootloader_version MEMBER bootloader_version)

 public:
  BleInfoParam() {
    sn_code = "";
    soft_version = "";
    hardware_version = "";
    bootloader_version = "";
  }
  QString sn_code;
  QString soft_version;
  QString hardware_version;
  QString bootloader_version;
};
Q_DECLARE_METATYPE(BleInfoParam)

struct EncoderInfoParam {
  Q_GADGET

  Q_PROPERTY(int cmd_result MEMBER cmd_result)
  Q_PROPERTY(int cmd_resv MEMBER cmd_resv)
  Q_PROPERTY(int enable_mech_angle_flag MEMBER enable_mech_angle_flag)
  Q_PROPERTY(int set_mech_angle_flag MEMBER set_mech_angle_flag)
  Q_PROPERTY(int set_mech_angle_upload_flag MEMBER set_mech_angle_upload_flag)

  Q_PROPERTY(qint16 mech_angle MEMBER mech_angle)

  Q_PROPERTY(int cali_state MEMBER mech_angle)
  Q_PROPERTY(qint32 open_loop_angle MEMBER open_loop_angle)
  Q_PROPERTY(qint32 elec_angle MEMBER elec_angle)

 public:
  EncoderInfoParam() {
    cmd_result = 0;
    cmd_resv = 0;
    enable_mech_angle_flag = 0;
    set_mech_angle_flag = 0;
    set_mech_angle_upload_flag = 0;

    mech_angle = 0;

    // 霍尔校准
    cali_state = 0;
    open_loop_angle = 0;
    elec_angle = 0;
    hall_cw_offset = 0;
    hall_ccw_offset = 0;
  }

  int cmd_result;  // 0,成功, 1,失败
  int cmd_resv;

  int enable_mech_angle_flag;      // 0,成功, 1,失败
  int set_mech_angle_flag;         // 0,成功, 1,失败
  int set_mech_angle_upload_flag;  // 0,成功, 1,失败

  qint16 mech_angle;

  int cali_state;
  qint32 open_loop_angle;
  qint32 elec_angle;
  qint32 hall_cw_offset;
  qint32 hall_ccw_offset;
};
Q_DECLARE_METATYPE(EncoderInfoParam)

struct LogFileInfoParam {
  Q_GADGET

  Q_PROPERTY(int file_error MEMBER file_error)
  Q_PROPERTY(int entry_type MEMBER entry_type)
  Q_PROPERTY(int file_size MEMBER file_size)
  Q_PROPERTY(QString file_path MEMBER file_path)

  Q_PROPERTY(int read_data_size MEMBER read_data_size)

 public:
  LogFileInfoParam() {
    file_error = 0;
    entry_type = 0;
    file_size = 0;
    file_path = "";
  }
  quint8 file_error;
  quint8 entry_type;
  quint32 file_size;
  QString file_path;
  quint16 read_data_size;
  quint8 file_data[256];
};
Q_DECLARE_METATYPE(LogFileInfoParam)

struct AgingTestCmdParam {
  Q_GADGET

  Q_PROPERTY(int test_ctrl_result MEMBER test_ctrl_result)
  Q_PROPERTY(int test_point_restult MEMBER test_point_restult)

 public:
  AgingTestCmdParam() {
    test_ctrl_result = 0;
    test_point_restult = 0;
  }

  int test_ctrl_result;  // 0,成功, 1,执行中, 2. 失败
  int test_ctrl_flag;    // 标志位

  int test_point_restult;
};
Q_DECLARE_METATYPE(AgingTestCmdParam)

struct EscAgingTestDataParam {
  Q_GADGET

  Q_PROPERTY(int adc1 MEMBER adc1)
  Q_PROPERTY(int adc2 MEMBER adc2)

  Q_PROPERTY(float recv_pwm MEMBER recv_pwm)
  Q_PROPERTY(float comm_pwm MEMBER comm_pwm)

  Q_PROPERTY(float phase_voltage MEMBER phase_voltage)

  Q_PROPERTY(float current_res MEMBER current_res)
  Q_PROPERTY(float current_acs MEMBER current_acs)
  Q_PROPERTY(float bus_current MEMBER bus_current)

  Q_PROPERTY(float voltage MEMBER voltage)
  Q_PROPERTY(float v_modulation MEMBER v_modulation)

  Q_PROPERTY(float mos_temp MEMBER mos_temp)
  Q_PROPERTY(float mos2_temp MEMBER mos2_temp)

  Q_PROPERTY(float cap_temp MEMBER cap_temp)
  Q_PROPERTY(float mcu_temp MEMBER mcu_temp)

  Q_PROPERTY(int running_error MEMBER running_error)
  Q_PROPERTY(int selfcheck_error MEMBER selfcheck_error)
  Q_PROPERTY(int selfcheck_value MEMBER selfcheck_value)
  Q_PROPERTY(int selfcheck_phase MEMBER selfcheck_phase)

  Q_PROPERTY(float motor_temp MEMBER motor_temp)
  Q_PROPERTY(int time_10ms MEMBER time_10ms)

  Q_PROPERTY(int encoder_angle MEMBER encoder_angle)
  Q_PROPERTY(float encoder_temp MEMBER encoder_temp)

  Q_PROPERTY(int v5_V MEMBER v5_V)
  Q_PROPERTY(int v15_V MEMBER v15_V)

 public:
  EscAgingTestDataParam() {
    adc1 = 0;
    adc2 = 0;
    recv_pwm = 0;
    comm_pwm = 0;

    phase_voltage = 0;

    current_res = 0;
    current_acs = 0;
    bus_current = 0;

    voltage = 0;
    v_modulation = 0;

    mos_temp = 0;
    mos2_temp = 0;
    cap_temp = 0;
    mcu_temp = 0;

    running_error = 0;
    selfcheck_error = 0;
    selfcheck_value = 0;
    selfcheck_phase = 0;

    motor_temp = 0;

    time_10ms = 0;

    encoder_angle = 0;
    encoder_temp = 0;

    v5_V = 0;
    v15_V = 0;
  }

  // 电调状态
  quint16 adc1;
  quint16 adc2;

  float recv_pwm;
  float comm_pwm;

  float phase_voltage;

  float current_res;
  float current_acs;
  float bus_current;

  float voltage;
  float v_modulation;

  float mos_temp;
  float mos2_temp;

  float cap_temp;
  float mcu_temp;

  quint16 running_error;
  quint16 selfcheck_error;
  qint16 selfcheck_value;
  qint16 selfcheck_phase;

  float motor_temp;

  quint16 time_10ms;

  quint16 encoder_angle;
  float encoder_temp;

  float v5_V;
  float v15_V;
};
Q_DECLARE_METATYPE(EscAgingTestDataParam)
