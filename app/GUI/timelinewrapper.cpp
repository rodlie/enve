#include "timelinewrapper.h"
#include "boxeslistkeysviewwidget.h"
#include "boxeslistanimationdockwidget.h"

TimelineWrapper::TimelineWrapper(Document * const document,
                                 TWidgetStackLayoutItem* const layItem,
                                 QWidget * const parent) :
    StackWidgetWrapper(
        layItem,
        []() {
            const auto rPtr = new TWidgetStackLayoutItem;
            return std::unique_ptr<WidgetStackLayoutItem>(rPtr);
        },
        [document](WidgetStackLayoutItem* const layItem,
                   QWidget * const parent) {
            const auto tLayItem = static_cast<TWidgetStackLayoutItem*>(layItem);
            const auto derived = new TimelineWrapper(document, tLayItem, parent);
            return static_cast<StackWidgetWrapper*>(derived);
        },
        [document](StackWidgetWrapper * toSetup) {
            const auto newWidget = new TimelineWidget(*document,
                                                      toSetup->getCornerMenu(),
                                                      toSetup);
            toSetup->setCentralWidget(newWidget);
}, parent) {}

void TimelineWrapper::setScene(Canvas * const scene) {
    getTimelineWidget()->setCurrentScene(scene);
}

Canvas* TimelineWrapper::getScene() const {
    return getTimelineWidget()->getCurrrentScene();
}

void TimelineWrapper::saveDataToLayout() const {
    const auto lItem = static_cast<TWidgetStackLayoutItem*>(getLayoutItem());
    if(!lItem) return;
    const auto sceneWidget = getTimelineWidget();
    lItem->setScene(sceneWidget->getCurrrentScene());
    const QSizeF sizeF = size();
    const QSizeF parentSize = parentWidget() ? parentWidget()->size() : sizeF;
    lItem->setSizeFrac({sizeF.width()/parentSize.width(),
                        sizeF.height()/parentSize.height()});
}

TimelineWidget* TimelineWrapper::getTimelineWidget() const {
    return static_cast<TimelineWidget*>(getCentralWidget());
}

void TWidgetStackLayoutItem::apply(StackWidgetWrapper * const stack) const {
    SceneWidgetStackLayoutItem::apply(stack);
    const auto tWrapper = static_cast<TimelineWrapper*>(stack);
    tWrapper->setScene(mScene);
    //const auto tWid = tWrapper->getTimelineWidget();
}