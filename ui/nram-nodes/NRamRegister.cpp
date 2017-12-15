#include "NRamRegister.h"

Register::Register(RegisterInfo info) : m_info(info)
{
}

QString Register::caption() const { return m_info.m_title; }

bool Register::captionVisible() const { return true; }

QString Register::name() const { return QString("R")+QString::number( m_info.m_id ); }

std::unique_ptr<QtNodes::NodeDataModel> Register::clone() const { return std::make_unique<Register>(m_info); }

unsigned int Register::nPorts(QtNodes::PortType portType) const
{
  switch (portType)
  {
    default:
    case QtNodes::PortType::In: return m_info.m_in;
    case QtNodes::PortType::Out:return !m_info.m_in;
  }
}

QtNodes::NodeDataType Register::dataType(QtNodes::PortType, QtNodes::PortIndex) const
{
   return {"integer", QString::number(m_value)};
}

std::shared_ptr<QtNodes::NodeData> Register::outData(QtNodes::PortIndex)
{
  std::shared_ptr<QtNodes::NodeData> ptr;
  return ptr;
}

void Register::setInData(std::shared_ptr<QtNodes::NodeData> nodeData,  QtNodes::PortIndex port)
{
    //todo
}

QWidget* Register::embeddedWidget()
{
    return nullptr;
}

QtNodes::NodeDataType Register::type() const
{
  return QtNodes::NodeDataType {"integer", "Integer"};
}

 QtNodes::NodeDataModel::ConnectionPolicy Register::portOutConnectionPolicy(QtNodes::PortIndex) const
 {
   return ConnectionPolicy::Many;
 }

 void Register::set_value(int value)
 {
     m_value = value;
 }
