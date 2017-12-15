#pragma once
#include <QObject>
#include <nodes/NodeDataModel.hpp>

class Register : public QtNodes::NodeDataModel
{

    Q_OBJECT

public:

    struct RegisterInfo
    {
        int m_id;
        QString m_title;
        bool m_in;
    };

    Register(RegisterInfo info);

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

    virtual ConnectionPolicy portOutConnectionPolicy(QtNodes::PortIndex) const override;

    void set_value(int value);

private:

    RegisterInfo m_info;
    int m_value{0};
};

