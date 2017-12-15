#pragma once

#include <vector>
#include <QMainWindow>
#include <QJsonDocument>
#include <QGridLayout>
#include <nodes/FlowScene.hpp>
#include <nodes/FlowView.hpp>

namespace Ui
{
    class NRamNodes;
}

struct NRamLayout
{
    int m_max_int;
    int m_time_steps;
    int m_n_regs;
    std::vector < QString > m_gates;
};

class Gate;
class Register;
class NodeMemory;

class NRamNodes : public QMainWindow
{
    Q_OBJECT

public:

    explicit NRamNodes(QWidget *parent = 0);

    ~NRamNodes();

    //from json
    void build_layout_json(const QByteArray &json_source);
    void build_connections(int test, int step);
    size_t get_tests() const
    {
        return m_tests;
    }
    size_t get_steps() const
    {
        return m_layout.m_time_steps;
    }

protected:

    virtual void resizeEvent(QResizeEvent * event);
    virtual bool event(QEvent * event);
    virtual void resizeAction();

private slots:
    void on_actionOpen_triggered();

    void on_testCB_currentIndexChanged(int test);

    void on_stepSB_valueChanged(int step);

    QtNodes::Node& node(int i);

    Gate& nram_gate(int i);

    Register& nram_register(int i);

private:
    //UI
    Ui::NRamNodes *ui;
    //Scene
    std::unique_ptr<QtNodes::FlowScene> m_scene;
    std::unique_ptr<QtNodes::FlowView>  m_view;
    std::unique_ptr<QGridLayout>        m_view_layout;
    //nram
    NRamLayout  m_layout;
    QJsonDocument m_jdocument;
    size_t m_tests;
    QRectF m_scene_size;
    //node by seq
    QtNodes::Node* m_mem_node;
    std::vector< QtNodes::Node* > m_nodes;

    //Free connections
    void free_connections();
    //Free nodes
    void free_nodes();
    //update ui
    void update_ui();
};

