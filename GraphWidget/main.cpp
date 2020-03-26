#include "widget.h"

#include <QApplication>

#include "graph.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;

    Graph graph;

    graph.show();
    w.show();
    return a.exec();
}
