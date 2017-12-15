#include "NRamNodes.h"
#include "NRamGate.h"
#include "NRamRegister.h"
#include "NRamMemory.h"
#include "ui_NRamNodes.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QFileDialog>

#include <nodes/Node.hpp>
#include <nodes/NodeDataModel.hpp>
#include <nodes/DataModelRegistry.hpp>
#include <vector>

static std::shared_ptr<QtNodes::DataModelRegistry> node_model_factory()
{
  auto ret = std::make_shared<QtNodes::DataModelRegistry>();
  return ret;
}

static QPoint add_register(std::vector< QtNodes::Node* >& nodes,std::unique_ptr<QtNodes::FlowScene>& graph, int id, bool in = false, const QPoint& pos = QPoint())
{
    //node
    auto& node =  graph->createNode(std::make_unique<Register>(Register::RegisterInfo{ id, QString("R")+QString::number( id ), in }));
    //position
    graph->setNodePosition(node, pos);
    //add
    nodes.push_back(&node);
    //get size
    QSizeF size_f = graph->getNodeSize(node);
    QPoint size(size_f.width(), size_f.height());
    return size;
}

static QPoint add_gate(std::vector< QtNodes::Node* >& nodes,std::unique_ptr<QtNodes::FlowScene>& graph, const QString& name, const QPoint& pos = QPoint())
{
    //graph
    auto  gate = std::move(Gate::create_gate_by_name(name));
    //test
    if(!gate.get())
    {
        qDebug() << "Wrong: gate \'" << name << "\' not supported";
        return QPoint();
    }
    //new node
    auto& node = graph->createNode(std::move(gate));
    //position
    graph->setNodePosition(node, pos);
    //add
    nodes.push_back(&node);
    //get size
    QSizeF size_f = graph->getNodeSize(node);
    QPoint size(size_f.width(), size_f.height());
    return size;
}

static QByteArray text_file(const QString& path)
{
    QFile tfile(path);
    //open
    if (!tfile.open(QFile::ReadOnly | QFile::Text)) return "";
    //read all
    return tfile.readAll();
}

//from json
void NRamNodes::build_layout_json(const  QByteArray& json_source)
{
    //json
    QJsonParseError jerrors;
    //doc
    m_jdocument = std::move(QJsonDocument::fromJson(json_source, &jerrors));
    //test
    if (!m_jdocument.isObject())
    {
        //Error
        qDebug() << "Document is not an object, " << jerrors.errorString();
        return;
    }
    //free nodes
    free_nodes();
    //
    QJsonObject jdocument = m_jdocument.object();
    QJsonObject jlayout = jdocument["layout"].toObject();
    m_layout.m_max_int = jlayout["max_int"].toInt();
    m_layout.m_n_regs = jlayout["n_registers"].toInt();
    m_layout.m_time_steps = jlayout["time_steps"].toInt();
    //get values
    m_tests = m_jdocument["tests"].toArray().size();
    //gates
    QJsonArray jgates = jlayout["gates"].toArray();
    //add
    for(auto jgate : jgates) m_layout.m_gates.push_back(jgate.toString());
    //Gpos
    int pos_r_y = 0;
    QPoint pos(0,0);
    QPoint delta(60,30);
    //build reg nodes
    for(long i = 0; i < m_layout.m_n_regs-1; ++i)
    {
        pos += add_register(m_nodes, m_scene, i, false, pos) + delta; pos.setX(0);
    }
    if(m_layout.m_n_regs)
    {
        pos += add_register(m_nodes, m_scene, m_layout.m_n_regs-1, false, pos) + delta;
        pos_r_y = pos.y();
        pos.setY(0);
    }
    //build gate nodes
    for(long i = 0; i < m_layout.m_gates.size(); ++i)
    {
        pos+= add_gate(m_nodes, m_scene, m_layout.m_gates[i], pos) + delta;
    }
    //build reg out
    //last x
    int last_x = pos.x();
    int last_y = pos.y();
    //y to 0
    pos.setY(0);
    //draw
    for(long i = 0; i < m_layout.m_n_regs-1; ++i)
    {
        pos += add_register(m_nodes, m_scene, i, true, pos) + delta; pos.setX(last_x);
    }
    if(m_layout.m_n_regs)
    {
        pos += add_register(m_nodes, m_scene, m_layout.m_n_regs-1, true, pos) + delta;
    }
    //scene size
    m_scene_size = QRectF
                (
                  -delta.x()
                , -delta.y()
                , std::max(pos.x(),last_x)+delta.x()
                , std::max(pos.y(),last_y)+delta.y()
                );
    //alloc mem node
    auto mem = std::move(std::make_unique<NodeMemory>());
    //Add mem node
    m_mem_node = &m_scene->createNode(std::move(mem));
    //mem node pos
    QPointF mem_pos(0, pos_r_y + ((last_y - pos_r_y) / 4.0));
    m_scene->setNodePosition(*m_mem_node, mem_pos);
}

void NRamNodes::build_connections(int test, int step)
{
    //free all
    free_connections();
    //get values
    QJsonArray jtests = m_jdocument["tests"].toArray();
    //select test
    QJsonObject jtest = jtests[test].toObject();
    //execution
    QJsonArray jsteps = jtest["execution"].toArray();
    //a step
    QJsonArray jstep = jsteps[step].toArray();
    //step last mem
    QJsonArray jmem[]
    {
        jtest["memory"].toArray()[0].toArray() //in
       ,QJsonArray()                            //out
       ,jtest["expected"].toArray()[0].toArray() //expected
    };
    //current
    size_t c_node = 0;
    //get reg values before
    if(step <= 0)
    {
       for(size_t i = 0; i!=m_layout.m_n_regs; ++i)
       {
           nram_register(i).set_value(0);
       }
       c_node = m_layout.m_n_regs;
    }
    else
    {
        //gate pre step
        auto jpre_step = jsteps[step-1].toArray();
        //for all
        for(auto j_value_node : jpre_step)
        {
            QJsonObject jnode = j_value_node.toObject();
            QString type = jnode["type"].toString();
            if(type == "update")
            {
                QJsonArray values = jnode["values"].toArray();
                int in_value = values[1].toArray()[0].toArray()[0].toInt();
                nram_register(c_node).set_value(in_value);
                //inc
                ++c_node;
            }
        }
    }
    //for all
    for(auto j_value_node : jstep)
    {
        QJsonObject jnode = j_value_node.toObject();
        QString type = jnode["type"].toString();
        if(type == "gate")
        {
            QString arity = jnode["arity"].toString();
            QJsonArray values = jnode["values"].toArray();
            //cases
            if(arity == "CONST")
            {
                //mem is at 0
                jmem[1] = values[0].toArray()[0].toArray();
                //out is at 1
                int out_value = values[1].toArray()[0].toArray()[0].toInt();
                nram_gate(c_node).set_out_value(out_value);
            }
            //cases
            else if(arity == "UNARY")
            {
                int from_id  = values[0].toArray()[0].toArray()[0].toInt();
                int in_value = values[1].toArray()[0].toArray()[0].toInt();
                m_scene->createConnection(node(c_node), 0, node(from_id), 0);
                //mem is at 2
                jmem[1] = values[2].toArray()[0].toArray();
                //out is at 3
                int out_value = values[3].toArray()[0].toArray()[0].toInt();
                //set value
                nram_gate(c_node).set_in_values({in_value});
                nram_gate(c_node).set_out_value(out_value);
            }
            //cases
            else if(arity == "BINARY")
            {
                int from_id_1 = values[0].toArray()[0].toArray()[0].toInt();
                int in_value_1 = values[1].toArray()[0].toArray()[0].toInt();
                m_scene->createConnection( node(c_node), 0, node(from_id_1), 0);
                int from_id_2 = values[2].toArray()[0].toArray()[0].toInt();
                int in_value_2 = values[3].toArray()[0].toArray()[0].toInt();
                m_scene->createConnection( node(c_node), 1, node(from_id_2), 0);
                //mem is at 4
                jmem[1] = values[4].toArray()[0].toArray();
                //out is at 5
                int out_value = values[5].toArray()[0].toArray()[0].toInt();
                //set values
                nram_gate(c_node).set_in_values({in_value_1, in_value_2});
                nram_gate(c_node).set_out_value(out_value);
            }
        }
        else if(type == "update")
        {
            QJsonArray values = jnode["values"].toArray();
            int from_id = values[0].toArray()[0].toArray()[0].toInt();
            int in_value = values[1].toArray()[0].toArray()[0].toInt();
            m_scene->createConnection( node(c_node), 0, node(from_id), 0);
            nram_register(c_node).set_value(in_value);
        }
        //inc
        ++c_node;
    }
    //for all mem
    for(unsigned short m = 0; m != 3 ; ++m)
    {
        //print mem
        QStringList n_values;
        for(size_t i = 0; i != jmem[m].size(); ++i) n_values << QString::number(jmem[m][i].toInt());
        //model date
        ((NodeMemory*)m_mem_node->nodeDataModel())->update_mem(m, n_values.join(" "));
    }
    //re-draw
    m_view->update();
    m_view->repaint();
}

//Free connections
void NRamNodes::free_connections()
{
    //vector
    std::vector< std::shared_ptr<QtNodes::Connection> > m_to_delete;
    //remove
    for(auto it : m_scene->connections()) m_to_delete.push_back(it.second);
    //delete all
    for(auto it : m_to_delete) m_scene->deleteConnection(*(it));
}

//Free nodes
void NRamNodes::free_nodes()
{
    //free connections
    free_connections();
    //free all
    m_scene->clearScene();
    m_scene->clear();
    //free nodes ref
    m_nodes.clear();
    m_layout.m_gates.clear();
    m_mem_node = nullptr;
}

//Update ui
void NRamNodes::update_ui()
{
    //remove tests
    ui->testCB->clear();
    //add tests
    for(size_t i = 0; i != get_tests(); ++i)
    {
        ui->testCB->addItem("Test: " + QString::number(i));
    }
    //set step
    ui->stepSB->setMinimum(0);
    ui->stepSB->setMaximum(get_steps() - 1);
    ui->stepDIAL->setRange(0, get_steps() - 1);
}


NRamNodes::NRamNodes(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::NRamNodes)
{
    //init ui
    ui->setupUi(this);
    //init scene
    m_scene       = std::make_unique<QtNodes::FlowScene>(node_model_factory());
    m_view_layout = std::make_unique<QGridLayout>(ui->flowGraph);
    m_view        = std::make_unique<QtNodes::FlowView>(m_scene.get());
    //layout, fill parent
    m_view_layout->addWidget(m_view.get(), 0, 0, 1, 1);
    ui->flowGraph->setContentsMargins(0, 0, 0, 0);
    //set
    connect(ui->stepSB, SIGNAL(valueChanged(int)), ui->stepDIAL, SLOT(setValue(int)));
    connect(ui->stepDIAL, SIGNAL(valueChanged(int)), ui->stepSB, SLOT(setValue(int)));
}

NRamNodes::~NRamNodes()
{
    delete ui;
}



QtNodes::Node& NRamNodes::node(int i)
{
    return *m_nodes[i];
}

Gate& NRamNodes::nram_gate(int i)
{
    return *((Gate*)(node(i).nodeDataModel()));
}

Register& NRamNodes::nram_register(int i)
{
    return *((Register*)(node(i).nodeDataModel()));
}

//Events
void NRamNodes::resizeAction()
{
    QRectF rect(m_scene_size);
    m_view->setSceneRect(rect);
    m_view->fitInView(rect, Qt::KeepAspectRatio);
    m_view->repaint();
}

void NRamNodes::resizeEvent(QResizeEvent * event)
{
    resizeAction();
    QMainWindow::resizeEvent(event);
}

bool NRamNodes::event(QEvent * event)
{
    if (event->type() == QEvent::Show)  resizeAction();
    return QMainWindow::event(event);
}

void NRamNodes::on_actionOpen_triggered()
{
    //Key of open path
    static const QString default_open_dir("default_open_dir");
    //Setting info
    QSettings nram_view_execution_settings;
    //OpenFile
    QString file_to_load = QFileDialog::getOpenFileName
    (
          this
        , tr("Open json of nram execution")
        , nram_view_execution_settings.value(default_open_dir).toString()
        , tr("Json of execution (*.json)")
    );
    //load
    if(file_to_load.size())
    {
        //save path
        nram_view_execution_settings.setValue(default_open_dir, QDir().absoluteFilePath(file_to_load));
        //build
        build_layout_json(text_file(file_to_load));
        //connection
        build_connections(0, 0);
        //to center
        resizeAction();
        //set values
        update_ui();
    }
}

void NRamNodes::on_testCB_currentIndexChanged(int test)
{
    build_connections(test, ui->stepSB->value());
}

void NRamNodes::on_stepSB_valueChanged(int step)
{
    build_connections(ui->testCB->currentIndex(), step);
}

