#pragma once
#include <QObject>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <nodes/NodeDataModel.hpp>

class NodeMemory : public QtNodes::NodeDataModel
{
    Q_OBJECT

public:

    struct NodeMemoryInfo
    {
        QString m_in_mem;
        QString m_out_mem;
        QString m_ex_mem;
    };

    NodeMemory(NodeMemoryInfo info = NodeMemoryInfo());

    QString caption() const override;

    bool captionVisible() const override;

    QString name() const override;

    std::unique_ptr<QtNodes::NodeDataModel> clone() const override;

    unsigned int nPorts(QtNodes::PortType portType) const override;

    QtNodes::NodeDataType dataType(QtNodes::PortType type, QtNodes::PortIndex id) const override;

    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex) override;

    void setInData(std::shared_ptr<QtNodes::NodeData> nodeData,  QtNodes::PortIndex port)  override;

    virtual QWidget* embeddedWidget() override;

    QtNodes::NodeDataType type() const;

    void update_in_mem(const QString& str);

    void update_out_mem(const QString& str);

    void update_ex_mem(const QString& str);

    void update_mem(size_t m, const QString& str);

private:

    //info
    NodeMemoryInfo m_info;
    //layout
    QWidget* m_layout;
    QLabel* m_in_mem { nullptr };
    QLabel* m_out_mem { nullptr };
    QLabel* m_ex_mem { nullptr };

};

