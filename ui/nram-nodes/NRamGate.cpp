#include "NRamGate.h"
#include <QDebug>

Gate::Gate(GeteInfo info) : m_info(info)
{
}

QString Gate::caption() const { return m_info.m_title; }

bool Gate::captionVisible() const { return true; }

QString Gate::name() const { return m_info.m_name; }

std::unique_ptr<QtNodes::NodeDataModel> Gate::clone() const { return std::move(std::make_unique<Gate>(m_info)); }

unsigned int Gate::nPorts(QtNodes::PortType portType) const
{
  switch (portType)
  {
    default:
    case QtNodes::PortType::In: return m_info.m_in_ports;
    case QtNodes::PortType::Out:return m_info.m_out_ports;
  }
}

QtNodes::NodeDataType Gate::dataType(QtNodes::PortType type, QtNodes::PortIndex index) const
{
    if(QtNodes::PortType::Out == type)
    {
        return QtNodes::NodeDataType { "integer", QString::number(m_out_value) };
    }
    else if(QtNodes::PortType::In == type && index < m_in_values.size())
    {
        return QtNodes::NodeDataType { "integer", QString::number(m_in_values[index]) };
    }
    else
    {
        return QtNodes::NodeDataType { "integer", "0" };
    }
}

void Gate::set_out_value(int value)
{
    m_out_value = value;
}

void Gate::set_in_values(const std::vector<int> &values)
{
    m_in_values = values;
}

std::shared_ptr<QtNodes::NodeData> Gate::outData(QtNodes::PortIndex)
{
  std::shared_ptr<QtNodes::NodeData> ptr;
  return ptr;
}

void Gate::setInData(std::shared_ptr<QtNodes::NodeData> nodeData,  QtNodes::PortIndex port)
{
    //todo
}

QWidget* Gate::embeddedWidget()
{
    return nullptr;
}

QtNodes::NodeDataType Gate::type() const
{
  return QtNodes::NodeDataType {"integer", "Integer"};
}


std::unique_ptr<Gate> Gate::create_gate_by_name(const QString& gate_name)
{
    QString name = gate_name.toLower();
         if(name == "add") return std::move(std::make_unique<Gate>(Gate::GeteInfo{ "Add", 2  }));
    else if(name == "dec") return std::move(std::make_unique<Gate>(Gate::GeteInfo{ "Dec", 1  }));
    else if(name == "inc") return std::move(std::make_unique<Gate>(Gate::GeteInfo{ "Inc", 1  }));
    else if(name == "equality"      || name == "eq")  return std::move(std::make_unique<Gate>(Gate::GeteInfo{ "Equality", 2  }));
    else if(name == "lessequalthan" || name == "let") return std::move(std::make_unique<Gate>(Gate::GeteInfo{ "LessEqualThan", 2  }));
    else if(name == "lessthan"      || name == "lt")  return std::move(std::make_unique<Gate>(Gate::GeteInfo{ "LessThan", 2  }));
    else if(name == "max") return std::move(std::make_unique<Gate>(Gate::GeteInfo{ "Max", 2  }));
    else if(name == "min") return std::move(std::make_unique<Gate>(Gate::GeteInfo{ "Min", 2  }));
    else if(name == "one") return std::move(std::make_unique<Gate>(Gate::GeteInfo{ "One", 0  }));
    else if(name == "read") return std::move(std::make_unique<Gate>(Gate::GeteInfo{ "Read", 1  }));
    else if(name == "sub") return std::move(std::make_unique<Gate>(Gate::GeteInfo{ "Sub", 2  }));
    else if(name == "two") return std::move(std::make_unique<Gate>(Gate::GeteInfo{ "Two", 0  }));
    else if(name == "write") return std::move(std::make_unique<Gate>(Gate::GeteInfo{ "Write", 2  }));
    else if(name == "zero") return std::move(std::make_unique<Gate>(Gate::GeteInfo{ "Zero", 0  }));
    qDebug() << "Not found gate: " << gate_name;
    return nullptr;
}
