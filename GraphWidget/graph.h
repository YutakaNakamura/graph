#ifndef GRAPH_H
#define GRAPH_H


#include <QMap>
#include <QPixmap>
#include <QVector>
#include <QWidget>
#include <QObject>
#include <QAbstractItemModel>

class Plot;
class Axis;
class Axes;

class Graph : public QWidget
{
    Q_OBJECT

public:
    Graph(QWidget *parent = 0);

    void setPlot(Plot* plot, Axis* yAxis);
    void setPlot(int column, Axis* yAxis);

    QMap<Axis*, Plot*>& plots();
    Plot* plot(int column) const { return m_plots.value(column, nullptr); }
    Axis* xAxis();

    // Model
    void setModel(QAbstractItemModel *model);
    void append(const QModelIndex &topLeft, const QModelIndex &bottomRight);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private slots:
    void onRefresh();
    void onAutoScaleUpdate();
    void onDataUpdate();
    void onYAxesVisbleChange(bool visble);
    void onXAxesVisbleChange(bool visible);

    // Model
    void onDataChange(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                      const QVector<int> &roles = QVector<int>());
    void onRowsInsert(const QModelIndex &parent, int first, int last);
    void onRowsAboutToBeRemove(const QModelIndex &parent, int first, int last);
    void onRowsRemove(const QModelIndex &parent, int first, int last);
    void onResetModel();

private:
    void refreshPixmap();
    void drawGrid(QPainter *painter);
    void drawCurves(QPainter *painter);

    enum { Margin = 10,
           TickMarksWidth = 5,
         };

    QMultiMap<Axis*, Plot*> m_plotMap;
    Axes* m_axes;
    QPixmap m_pixmap;
    int m_visbleYAxesCount;
    QRect m_rect;
    QAbstractItemModel *m_model;
    QMap<int, Plot*> m_plots;
};

class Plot : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Plot)

public:
    explicit Plot(int section, QAbstractItemModel *model = 0, QObject *parent = 0);
    virtual ~Plot(){}

    QPointF minData() const { return m_minData; }
    QPointF maxData() const { return m_maxData; }

    bool visble() const;
    void setVisble(bool visble);

    QColor lineColor() const;
    void setLineColor(const QColor &lineColor);

    qreal lineWidth() const;
    void setLineWidth(qreal lineWidth);

    int count() const { return m_model->rowCount(); }
    void clear();
    double yData(int index) const { return m_model->index(index, m_section).data().toDouble(); }
    double xData(int index) const { return m_model->index(index, 0).data().toDouble(); }

    inline int plottedPoint() const;
    inline void setPlottedPoint(int plottedCount);
    void clearPlottedPoint() {m_plottedCount = 0;}

    bool calculateMinMaxData(const QModelIndex &yIndex, const QModelIndex &xIndex = QModelIndex());
    void checkMinMaxDeleteData(const QModelIndex &yIndex, const QModelIndex &xIndex = QModelIndex());
    bool recalculateMinMaxAllData(bool isForce = false);

signals:
    void refreshed();
    void autoScaleUpdated();
    void dataUpdated();

private:
    int m_section;
    QAbstractItemModel *m_model;
    bool m_visble;
    QColor m_lineColor;
    qreal m_lineWidth;
    QPointF m_minData;
    QPointF m_maxData;
    int m_plottedCount;
    bool m_isRecalculateMinMax;
};

class Axis : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Axis)

public:
    explicit Axis(QObject* parent = 0);
    virtual ~Axis() {}

    bool visble() const;
    void setVisble(bool visble);

    qreal min() const;
    void setMin(qreal value);
    qreal max() const;
    void setMax(qreal value);
    bool autoScale() const;
    void setAutoScale(bool autoScale);

    QColor lineColor() const;
    void setLineColor(const QColor &lineColor);

    enum UpdateAdjust { AutoScale, Forced };

    void adjust(qreal min, qreal max,
                UpdateAdjust updateAdjust = AutoScale) {
        adjustAxis(min, max, updateAdjust);
    }
    bool autoScaleAdjustX(QList<Plot*> axes);
    bool autoScaleAdjustY(QList<Plot*> axes);

    int numTicks() const { return m_adjustSettings.numTicks; }
    qreal span() const { return m_adjustSettings.max - m_adjustSettings.min; }

    void clearMaxTickLabelWidth() { m_maxTickLabelWidth = 0; }
    void setMaxTickLabelWidth(int width) {
        if (m_maxTickLabelWidth < width)
            m_maxTickLabelWidth = width;
    }
    int maxTickLabelWidth() const { return m_maxTickLabelWidth; }

signals:
    void visbleChanged(bool visble);
    void autoScaleChanged(bool autoScale);
    void minMaxChanged();

private:
    bool adjustAxis(qreal min, qreal max, UpdateAdjust updateAdjust);

    struct AdjustSettings {
        qreal min;
        qreal max;
        int numTicks;
    } m_adjustSettings;

    bool m_visble;
    bool m_autoScale;
    QString m_caption;
    QColor m_lineColor;
    int m_maxTickLabelWidth;
};

class Axes
{

public:
    explicit Axes(QObject* parent = 0);
    explicit Axes(int numAxes, QObject* parent = 0);
    ~Axes();

    Axis* xAxis() { return  m_xAxis; }
    QVector<Axis* >& yAxes() { return m_yAxes; }
    Axis* yAxes(int key) { return m_yAxes[key]; }

private:
    Axis* m_xAxis;
    QVector<Axis* > m_yAxes;
};

#endif
