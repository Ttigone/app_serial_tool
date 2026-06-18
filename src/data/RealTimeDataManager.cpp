#include "RealTimeDataManager.h"
#include <QDateTime>
#include "ParamManager.h"
#include "Data/AgingTestDataManager.h"
QObject* RealTimeDataManager::qmlSingleton(QQmlEngine *engine, QJSEngine *scriptEngine){
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    static RealTimeDataManager* _singleton=nullptr;
    if(!_singleton)
        _singleton=new RealTimeDataManager;

    return _singleton;
}

RealTimeDataManager* RealTimeDataManager::singleton(){
    return qobject_cast<RealTimeDataManager*>(qmlSingleton(nullptr,nullptr));
}


RealTimeDataManager::RealTimeDataManager()
{
    initRealTimeWave();
    initAgingTestWave();
}

RealTimeDataManager::~RealTimeDataManager()
{
}


void RealTimeDataManager::initRealTimeWave()
{
    m_TimeStart = QDateTime::currentMSecsSinceEpoch();

    addWave("time_10ms");

    addWave("recv_pwm");
    addWave("comm_pwm");

    addWave("speed");

    addWave("current");
    addWave("bus_current");

    addWave("voltage");
    addWave("v_modulation");

    addWave("mos_temp");
    addWave("cap_temp");
    addWave("mcu_temp");
    addWave("motor_temp");

    addWave("running_error");
    addWave("selfcheck_error");

    addWave("speed_km_h");

    addWave("mech_angle");
    addWave("open_loop_angle");
    addWave("elec_angle");
}

void RealTimeDataManager::resetWave()
{
    QMap<QString, WaveData*>::iterator iter;
    for (iter = m_DataMap.begin(); iter != m_DataMap.end(); iter++)
    {
        WaveData* data = iter.value();
        data->clear();
    }

    m_TimeStart = QDateTime::currentMSecsSinceEpoch();
}


void RealTimeDataManager::addWave(QString line_name)
{
    if (!m_DataMap.contains(line_name))
    {
        WaveData* data = new WaveData();
        m_DataMap[line_name] = data;
    }
}

void RealTimeDataManager:: clearData()
{
    QMap<QString, WaveData*>::iterator iter;
    for (iter = m_DataMap.begin(); iter != m_DataMap.end(); iter++)
    {
        WaveData* data = iter.value();
        data->clear();
    }
}


QSharedPointer<QCPGraphDataContainer>  RealTimeDataManager::getData(QString wave_id)
{
    QSharedPointer<QCPGraphDataContainer> data_point = NULL;

    WaveData* wave_data = NULL;
    if (m_DataMap.contains(wave_id))
    {
        wave_data = m_DataMap[wave_id];
        data_point = wave_data->m_DataBufferPointer;
    }
    return data_point;
}


void RealTimeDataManager::initAgingTestWave()
{
    m_TimeStart = QDateTime::currentMSecsSinceEpoch();

    for(int i = 0; i < 4; i++)
    {
        addAgingTestWave(QString("ESC%1#ADC1").arg(i));
        addAgingTestWave(QString("ESC%1#ADC2").arg(i));

        addAgingTestWave(QString("ESC%1#recv_pwm").arg(i));
        addAgingTestWave(QString("ESC%1#comm_pwm").arg(i));

        addAgingTestWave(QString("ESC%1#phase_voltage").arg(i)); //相电压

        addAgingTestWave(QString("ESC%1#current_res").arg(i));   //电阻传感器
        addAgingTestWave(QString("ESC%1#current_acs").arg(i));   //电流传感器
        addAgingTestWave(QString("ESC%1#bus_current").arg(i));   //母线电流

        addAgingTestWave(QString("ESC%1#voltage").arg(i));   //
        addAgingTestWave(QString("ESC%1#v_modulation").arg(i));   //

        addAgingTestWave(QString("ESC%1#mos_temp").arg(i));   //
        addAgingTestWave(QString("ESC%1#mos2_temp").arg(i));   //

        addAgingTestWave(QString("ESC%1#cap_temp").arg(i));   //
        addAgingTestWave(QString("ESC%1#mcu_temp").arg(i));   //

        addAgingTestWave(QString("ESC%1#running_error").arg(i));   //

        addAgingTestWave(QString("ESC%1#selfcheck_error").arg(i));   //
        addAgingTestWave(QString("ESC%1#selfcheck_value").arg(i));   //
        addAgingTestWave(QString("ESC%1#selfcheck_phase").arg(i));   //

        addAgingTestWave(QString("ESC%1#motor_temp").arg(i));   //

        addAgingTestWave(QString("ESC%1#time_10ms").arg(i));   //
        addAgingTestWave(QString("ESC%1#encoder_angle").arg(i));   //
        addAgingTestWave(QString("ESC%1#encoder_temp").arg(i));   //

        addAgingTestWave(QString("ESC%1#v5_V").arg(i));   //
        addAgingTestWave(QString("ESC%1#v15_V").arg(i));   //
    }
}

void RealTimeDataManager::resetAgingTestWave()
{
    QMap<QString, WaveData*>::iterator iter;
    for (iter = m_AgingDataMap.begin(); iter != m_AgingDataMap.end(); iter++)
    {
        WaveData* data = iter.value();
        data->clear();
    }

    m_AgingTestTimeStart = QDateTime::currentMSecsSinceEpoch();
}


void RealTimeDataManager::addAgingTestWave(QString line_name)
{
    if (!m_AgingDataMap.contains(line_name))
    {
        WaveData* data = new WaveData();
        m_AgingDataMap[line_name] = data;
    }
}

void RealTimeDataManager:: clearAgingTestData()
{
    QMap<QString, WaveData*>::iterator iter;
    for (iter = m_AgingDataMap.begin(); iter != m_AgingDataMap.end(); iter++)
    {
        WaveData* data = iter.value();
        data->clear();
    }
}

QSharedPointer<QCPGraphDataContainer>  RealTimeDataManager::getAgingTestData(QString wave_id)
{
    QSharedPointer<QCPGraphDataContainer> data_point = NULL;

    WaveData* wave_data = NULL;
    if (m_AgingDataMap.contains(wave_id))
    {
        wave_data = m_AgingDataMap[wave_id];
        data_point = wave_data->m_DataBufferPointer;
    }
    return data_point;
}

void RealTimeDataManager::addDataPoint()
{
    EscRealtimeDataParam* realtime_data = ParamManager::singleton()->getEscRealtimeDataParam();

    qint64 time_now = QDateTime::currentMSecsSinceEpoch();
    double time_x = double((time_now - m_TimeStart)) / 1000.0;


    //recv_pwm(time_10ms)
    QCPGraphData time_10ms_point(time_x, realtime_data->time_10ms);
    WaveData* time_10ms_data = m_DataMap["time_10ms"];
    time_10ms_data->appendPoint(time_10ms_point);

    //recv_pwm(0.1us)
    QCPGraphData recv_pwm_point(time_x, realtime_data->recv_pwm/10.0f);
    WaveData* pwm_recv_data = m_DataMap["recv_pwm"];
    pwm_recv_data->appendPoint(recv_pwm_point);

    //comm_pwm(0.1us)
    QCPGraphData comm_pwm_point(time_x, realtime_data->comm_pwm/10.0f);
    WaveData* comm_pwm_data = m_DataMap["comm_pwm"];
    comm_pwm_data->appendPoint(comm_pwm_point);

    //speed(0.1rpm)
    QCPGraphData speed_point(time_x, realtime_data->speed);
    WaveData* speed_data = m_DataMap["speed"];
    speed_data->appendPoint(speed_point);

    //current(0.1A)
    QCPGraphData current_point(time_x, realtime_data->current/10.0f);
    WaveData* current_data = m_DataMap["current"];
    current_data->appendPoint(current_point);

    //bus_current(0.1A)
    QCPGraphData bus_current_point(time_x, realtime_data->bus_current/10.0f);
    WaveData* bus_current_data = m_DataMap["bus_current"];
    bus_current_data->appendPoint(bus_current_point);

    //voltage(0.1V)
    QCPGraphData voltage_point(time_x, realtime_data->voltage/10.0f);
    WaveData* voltage_data = m_DataMap["voltage"];
    voltage_data->appendPoint(voltage_point);

    //v_modulation(0.1)
    QCPGraphData v_modulation_point(time_x, realtime_data->v_modulation/10.0f);
    WaveData* v_modulation_data = m_DataMap["v_modulation"];
    v_modulation_data->appendPoint(v_modulation_point);

    //mos_temp(0.1)
    QCPGraphData mos_temp_point(time_x, realtime_data->mos_temp/10.0f);
    WaveData* mos_temp_data = m_DataMap["mos_temp"];
    mos_temp_data->appendPoint(mos_temp_point);

    //cap_temp(0.1)
    QCPGraphData cap_temp_point(time_x, realtime_data->cap_temp/10.0f);
    WaveData* cap_temp_data = m_DataMap["cap_temp"];
    cap_temp_data->appendPoint(cap_temp_point);

    //mcu_temp(0.1)
    QCPGraphData mcu_temp_point(time_x, realtime_data->mcu_temp/10.0f);
    WaveData* mcu_temp_data = m_DataMap["mcu_temp"];
    mcu_temp_data->appendPoint(mcu_temp_point);

    //motor_temp(0.1)
    QCPGraphData motor_temp_point(time_x, realtime_data->motor_temp/10.0f);
    WaveData* motor_temp_data = m_DataMap["motor_temp"];
    motor_temp_data->appendPoint(motor_temp_point);

    //running_error
    QCPGraphData running_error_point(time_x, realtime_data->running_error);
    WaveData* running_error_data = m_DataMap["running_error"];
    running_error_data->appendPoint(running_error_point);

    //selfcheck_error
    QCPGraphData selfcheck_error_point(time_x, realtime_data->selfcheck_error);
    WaveData* selfcheck_error_data = m_DataMap["selfcheck_error"];
    selfcheck_error_data->appendPoint(selfcheck_error_point);

    //selfcheck_error
    QCPGraphData speed_km_h_point(time_x, realtime_data->speed_km_h/10.0f);
    WaveData* speed_km_h_data = m_DataMap["speed_km_h"];
    speed_km_h_data->appendPoint(speed_km_h_point);
}

void RealTimeDataManager::addEncoderPoint()
{
    EncoderInfoParam* encoder_param = ParamManager::singleton()->getEncoderInfoParamPtr();

    qint64 time_now = QDateTime::currentMSecsSinceEpoch();
    double time_x = double((time_now - m_TimeStart)) / 1000.0;


    //(mech_angle)
    QCPGraphData mech_angle_point(time_x, encoder_param->mech_angle);
    WaveData* mech_angle_data = m_DataMap["mech_angle"];
    mech_angle_data->appendPoint(mech_angle_point);

//    //(open_loop_angle)
//    QCPGraphData open_loop_angle_point(time_x, encoder_param->open_loop_angle);
//    WaveData* open_loop_angle_data = m_DataMap["open_loop_angle"];
//    open_loop_angle_data->appendPoint(mech_angle_point);

//    //(elec_angle)
//    QCPGraphData elec_angle_point(time_x, encoder_param->elec_angle);
//    WaveData* elec_angle_data = m_DataMap["elec_angle"];
//    elec_angle_data->appendPoint(mech_angle_point);
}

void RealTimeDataManager::addAgingTestPoint(quint8 esc_id, quint16 test_count, quint8 test_point_index, quint32 test_point_time_ms, EscAgingTestDataParam& test_data)
{
    quint8 esc_index = esc_id - 32;

    qint64 time_now = QDateTime::currentMSecsSinceEpoch();
    double time_x = double((time_now - m_AgingTestTimeStart)) / 1000.0;

    //ADC1
    QCPGraphData adc1_point(time_x, test_data.adc1);
    WaveData* adc1_data = m_AgingDataMap[QString("ESC%1#ADC1").arg(esc_index)];
    adc1_data->appendPoint(adc1_point);
    //ADC2
    QCPGraphData adc2_point(time_x, test_data.adc2);
    WaveData* adc2_data = m_AgingDataMap[QString("ESC%1#ADC2").arg(esc_index)];
    adc2_data->appendPoint(adc2_point);

    //recv_pwm
    QCPGraphData recv_pwm_point(time_x, test_data.recv_pwm);
    WaveData* recv_pwm_data = m_AgingDataMap[QString("ESC%1#recv_pwm").arg(esc_index)];
    recv_pwm_data->appendPoint(recv_pwm_point);

    //comm_pwm
    QCPGraphData comm_pwm_point(time_x, test_data.comm_pwm);
    WaveData* comm_pwm_data = m_AgingDataMap[QString("ESC%1#comm_pwm").arg(esc_index)];
    comm_pwm_data->appendPoint(comm_pwm_point);

    //phase_voltage
    QCPGraphData phase_voltage_point(time_x, test_data.phase_voltage);
    WaveData* phase_voltage_data = m_AgingDataMap[QString("ESC%1#phase_voltage").arg(esc_index)];
    phase_voltage_data->appendPoint(phase_voltage_point);

    //current_res
    QCPGraphData current_res_point(time_x, test_data.current_res);
    WaveData* current_res_data = m_AgingDataMap[QString("ESC%1#current_res").arg(esc_index)];
    current_res_data->appendPoint(current_res_point);

    //current_acs
    QCPGraphData current_acs_point(time_x, test_data.current_acs);
    WaveData* current_acs_data = m_AgingDataMap[QString("ESC%1#current_acs").arg(esc_index)];
    current_acs_data->appendPoint(current_acs_point);

    //bus_current
    QCPGraphData bus_current_point(time_x, test_data.bus_current);
    WaveData* bus_current_data = m_AgingDataMap[QString("ESC%1#bus_current").arg(esc_index)];
    bus_current_data->appendPoint(bus_current_point);

    //voltage
    QCPGraphData voltage_point(time_x, test_data.voltage);
    WaveData* voltage_data = m_AgingDataMap[QString("ESC%1#voltage").arg(esc_index)];
    voltage_data->appendPoint(voltage_point);

    //v_modulation
    QCPGraphData v_modulation_point(time_x, test_data.v_modulation);
    WaveData* v_modulation_data = m_AgingDataMap[QString("ESC%1#v_modulation").arg(esc_index)];
    v_modulation_data->appendPoint(v_modulation_point);

    //mos_temp
    QCPGraphData mos_temp_point(time_x, test_data.mos_temp);
    WaveData* mos_temp_data = m_AgingDataMap[QString("ESC%1#mos_temp").arg(esc_index)];
    mos_temp_data->appendPoint(mos_temp_point);

    //mos2_temp
    QCPGraphData mos2_temp_point(time_x, test_data.mos2_temp);
    WaveData* mos2_temp_data = m_AgingDataMap[QString("ESC%1#mos2_temp").arg(esc_index)];
    mos2_temp_data->appendPoint(mos2_temp_point);

    //cap_temp
    QCPGraphData cap_temp_point(time_x, test_data.cap_temp);
    WaveData* cap_temp_data = m_AgingDataMap[QString("ESC%1#cap_temp").arg(esc_index)];
    cap_temp_data->appendPoint(cap_temp_point);

    //mcu_temp
    QCPGraphData mcu_temp_point(time_x, test_data.mcu_temp);
    WaveData* mcu_temp_data = m_AgingDataMap[QString("ESC%1#mcu_temp").arg(esc_index)];
    mcu_temp_data->appendPoint(mcu_temp_point);

    //
    QCPGraphData running_error_point(time_x, test_data.running_error);
    WaveData* running_error_data = m_AgingDataMap[QString("ESC%1#running_error").arg(esc_index)];
    running_error_data->appendPoint(running_error_point);

    //selfcheck_error
    QCPGraphData selfcheck_error_point(time_x, test_data.selfcheck_error);
    WaveData* selfcheck_error_data = m_AgingDataMap[QString("ESC%1#selfcheck_error").arg(esc_index)];
    selfcheck_error_data->appendPoint(selfcheck_error_point);

    //selfcheck_value
    QCPGraphData selfcheck_value_point(time_x, test_data.selfcheck_value);
    WaveData* selfcheck_value_data = m_AgingDataMap[QString("ESC%1#selfcheck_value").arg(esc_index)];
    selfcheck_value_data->appendPoint(selfcheck_value_point);

    //selfcheck_phase
    QCPGraphData selfcheck_phase_point(time_x, test_data.selfcheck_phase);
    WaveData* selfcheck_phase_data = m_AgingDataMap[QString("ESC%1#selfcheck_phase").arg(esc_index)];
    selfcheck_phase_data->appendPoint(selfcheck_phase_point);

    //motor_temp
    QCPGraphData motor_temp_point(time_x, test_data.motor_temp);
    WaveData* motor_temp_data = m_AgingDataMap[QString("ESC%1#motor_temp").arg(esc_index)];
    motor_temp_data->appendPoint(motor_temp_point);

    //time_10ms
    QCPGraphData time_10ms_point(time_x, test_data.time_10ms);
    WaveData* time_10ms_data = m_AgingDataMap[QString("ESC%1#time_10ms").arg(esc_index)];
    time_10ms_data->appendPoint(time_10ms_point);

    //encoder_angle
    QCPGraphData encoder_angle_point(time_x, test_data.encoder_angle);
    WaveData* encoder_angle_data = m_AgingDataMap[QString("ESC%1#encoder_angle").arg(esc_index)];
    encoder_angle_data->appendPoint(encoder_angle_point);

    //encoder_temp
    QCPGraphData encoder_temp_point(time_x, test_data.encoder_temp);
    WaveData* encoder_temp_data = m_AgingDataMap[QString("ESC%1#encoder_temp").arg(esc_index)];
    encoder_temp_data->appendPoint(encoder_temp_point);

    //selfcheck_phase
    QCPGraphData v5_V_point(time_x, test_data.v5_V);
    WaveData* v5_V_data = m_AgingDataMap[QString("ESC%1#v5_V").arg(esc_index)];
    v5_V_data->appendPoint(v5_V_point);

    //v15_V
    QCPGraphData v15_V_point(time_x, test_data.v15_V);
    WaveData* v15_V_data = m_AgingDataMap[QString("ESC%1#v15_V").arg(esc_index)];
    v15_V_data->appendPoint(v15_V_point);

    AgingTestDataManager::singleton()->judgeAgingTestData(esc_id, test_count, test_point_index, test_point_time_ms, test_data);
}
