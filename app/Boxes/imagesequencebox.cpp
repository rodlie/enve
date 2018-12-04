#include "imagesequencebox.h"
#include "filesourcescache.h"
#include "GUI/mainwindow.h"
#include <QFileDialog>

ImageSequenceBox::ImageSequenceBox() :
    AnimationBox() {
    setName("Image Sequence");
}

void ImageSequenceBox::setListOfFrames(const QStringList &listOfFrames) {
    mListOfFrames = listOfFrames;
    if(mAnimationCacheHandler != nullptr) {
        mAnimationCacheHandler->removeDependentBox(this);
    }
    mAnimationCacheHandler =
            ImageSequenceCacheHandler::createNewHandler(mListOfFrames);
    mAnimationCacheHandler->addDependentBox(this);

    reloadCacheHandler();
}

void ImageSequenceBox::changeSourceFile() {
    MainWindow::getInstance()->disableEventFilter();
    QStringList importPaths = QFileDialog::getOpenFileNames(
                MainWindow::getInstance(), "Import Image Sequence",
                "", "Images (*.png *.jpg)");
    MainWindow::getInstance()->enableEventFilter();
    if(!importPaths.isEmpty()) {
        setListOfFrames(importPaths);
    }
}