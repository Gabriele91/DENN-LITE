#pragma once
#include <QObject>
#include <nodes/NodeDataModel.hpp>
#include <vector>

class Gate : public QtNodes::NodeDataModel
{
    Q_OBJECT

public:

    struct GeteInfo
    {
        QString m_name;
        QString m_title;
        int m_in_ports;
        int m_out_ports;
        GeteInfo
        (
              QString name
            , int in_ports
            , int out_ports = 1
        )
        {
            m_name      = name;
            m_title     = name;
            m_in_ports  = in_ports;
            m_out_ports  = out_ports;
        }
        GeteInfo
        (
              QString name
            , QString title
            , int in_ports
            , int out_ports = 1
        )
        {
            m_name      = name;
            m_title     = title;
            m_in_ports  = in_ports;
            m_out_ports  = out_ports;
        }
    };

    Gate(GeteInfo info);

    QString caption() const override;

    bool captionVisible() const override;

    QString name() const override;

    std::unique_ptr<QtNodes::NodeDataModel> clone() const override;

    unsigned int nPorts(QtNodes::PortType portType) const override;

    QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override;

    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override;

    void setInData(std::shared_ptr<QtNodes::NodeData> nodeData,  QtNodes::PortIndex port)  override;

    virtual QWidget* embeddedWidget() override;

    QtNodes::NodeDataType type() const;

    static std::unique_ptr<Gate> create_gate_by_name(const QString& gate_name);

    void set_out_value(int value);

    void set_in_values(const std::vector<int> &values);

private:

    GeteInfo m_info;
    //values
    int m_out_value{0};
    std::vector<int> m_in_values;
};
