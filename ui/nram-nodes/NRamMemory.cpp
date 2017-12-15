#include "NRamMemory.h"

NodeMemory::NodeMemory(NodeMemoryInfo info): m_info(info)
{
    //alloc and init
    m_layout  = new QWidget();
    m_in_mem  = new QLabel(info.m_in_mem);
    m_out_mem = new QLabel(info.m_out_mem);
    m_ex_mem  = new QLabel(info.m_ex_mem);
    //style
    m_layout->setAttribute(Qt::WA_TranslucentBackground);
    m_in_mem->setAttribute(Qt::WA_TranslucentBackground);
    m_out_mem->setAttribute(Qt::WA_TranslucentBackground);
    m_ex_mem->setAttribute(Qt::WA_TranslucentBackground);
    //Add
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_in_mem);
    layout->addWidget(m_out_mem);
    layout->addWidget(m_ex_mem);
    //layout
    m_layout->setLayout(layout);
}

QString NodeMemory::caption() const
{
    return QString("Memory");
}

bool NodeMemory::captionVisible() const
{
    return true;
}

QString NodeMemory::name() const
{
    return QString("Memory");
}

std::unique_ptr<QtNodes::NodeDataModel> NodeMemory::clone() const
{
    return std::move(std::make_unique<NodeMemory>());
}

unsigned int NodeMemory::nPorts(QtNodes::PortType portType) const
{
    return 0;
}

QtNodes::NodeDataType NodeMemory::dataType(QtNodes::PortType type, QtNodes::PortIndex id) const
{
    return QtNodes::NodeDataType{};
}

std::shared_ptr<QtNodes::NodeData> NodeMemory::outData(QtNodes::PortIndex)
{
    return nullptr;
}

void NodeMemory::setInData(std::shared_ptr<QtNodes::NodeData> nodeData,  QtNodes::PortIndex port)
{
    //none
}

QWidget* NodeMemory::embeddedWidget()
{
    return m_layout;
}

QtNodes::NodeDataType NodeMemory::type() const
{
    return QtNodes::NodeDataType{};
}

void NodeMemory::update_in_mem(const QString& str)
{
    m_info.m_in_mem = str;
    if(m_in_mem) m_in_mem->setText("IN :\t" + str);
    m_layout->repaint();
}

void NodeMemory::update_out_mem(const QString& str)
{
    m_info.m_out_mem = str;
    if(m_out_mem) m_out_mem->setText("OUT:\t" + str);
    m_layout->repaint();
}

void NodeMemory::update_ex_mem(const QString& str)
{
    m_info.m_ex_mem = str;
    if(m_ex_mem) m_ex_mem->setText("EXP:\t" + str);
    m_layout->repaint();
}

void NodeMemory::update_mem(size_t m, const QString& str)
{
    switch (m)
    {
    case 0: update_in_mem(str); break;
    case 1: update_out_mem(str); break;
    case 2: update_ex_mem(str); break;
    default:
    break;
    }
}
