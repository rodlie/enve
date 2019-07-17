#include "colorpickingwidget.h"
#include <QMouseEvent>
#include <QPainter>
#include "GUI/mainwindow.h"
#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>
#include "colorsettingswidget.h"
#include <QDebug>
#include "global.h"
#include <QWindow>

ColorPickingWidget::ColorPickingWidget(QWidget * const parent)
    : QWidget(parent) {
    QScreen * screen = nullptr;
    if(const auto window = windowHandle())
        screen = window->screen();
    else screen = QGuiApplication::primaryScreen();
    if(!screen) return;

    mScreenshot = screen->grabWindow(0).toImage();

    QPixmap picker(":/cursors/cursor_color_picker.png");
    QApplication::setOverrideCursor(QCursor(picker, 2, 20) );
    grabKeyboard();
    grabMouse();

    setMouseTracking(true);

    setWindowFlag(Qt::SplashScreen);
    showFullScreen();
    updateBox(QCursor::pos());
}

void ColorPickingWidget::mouseReleaseEvent(QMouseEvent *e) {
    if(e->button() == Qt::RightButton) endThis();
    emit colorSelected(mCurrentColor);
    MainWindow::getInstance()->queTasksAndUpdate();
    endThis();
}

void ColorPickingWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.drawImage(0, 0, mScreenshot);
    p.fillRect(mCursorX + 2*MIN_WIDGET_DIM - 4,
               mCursorY - MIN_WIDGET_DIM - 4,
               MIN_WIDGET_DIM + 8,
               MIN_WIDGET_DIM + 8, Qt::black);
    p.fillRect(mCursorX + 2*MIN_WIDGET_DIM - 2,
               mCursorY - MIN_WIDGET_DIM - 2,
               MIN_WIDGET_DIM + 4,
               MIN_WIDGET_DIM + 4, Qt::white);
    p.fillRect(mCursorX + 2*MIN_WIDGET_DIM,
               mCursorY - MIN_WIDGET_DIM,
               MIN_WIDGET_DIM, MIN_WIDGET_DIM,
               mCurrentColor);
    p.end();
}

void ColorPickingWidget::keyPressEvent(QKeyEvent *e) {
    if(e->isAutoRepeat()) return;
    endThis();
}

void ColorPickingWidget::mouseMoveEvent(QMouseEvent *e) {
    updateBox(e->globalPos());
}

QColor ColorPickingWidget::colorFromPoint(const int x, const int y) {
    const QRgb rgb = mScreenshot.pixel(x, y);
    return QColor::fromRgb(rgb);
}

void ColorPickingWidget::endThis() {
    QApplication::restoreOverrideCursor();
    releaseMouse();
    releaseKeyboard();
    close();
}

void ColorPickingWidget::updateBox(const QPoint& globalPos) {
    mCursorX = globalPos.x();
    mCursorY = globalPos.y();
    mCurrentColor = colorFromPoint(globalPos.x(), globalPos.y());
    update();
}

