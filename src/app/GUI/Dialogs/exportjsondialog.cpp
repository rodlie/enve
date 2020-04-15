// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "exportjsondialog.h"

#include "Private/document.h"
#include "GUI/scenechooser.h"
#include "GUI/global.h"
#include "canvas.h"
#include "jsonexporter.h"
#include "GUI/edialogs.h"
#include "GUI/twocolumnlayout.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMenuBar>
#include <QLabel>

//class SkottieLogger : public skottie::Logger {
//public:
//    void log(skottie::Logger::Level lvl, const char message[], const char json[]) override {
//        Q_UNUSED(lvl);
//        qDebug() << message << json;
//    }
//};

class JsonPreview : public QLabel {
public:
    JsonPreview(QWidget* const parent = nullptr) : QLabel(parent) {
        setSizePolicy(QSizePolicy::MinimumExpanding,
                      QSizePolicy::MinimumExpanding);
        setMinimumSize(10*MIN_WIDGET_DIM, 10*MIN_WIDGET_DIM);
        connect(&mTimer, &QTimer::timeout, this, &JsonPreview::incFrame);
        setStyleSheet("QLabel { background: white; }");
    }

    void reload() {
        load(mPath);
    }

    void load(const QString& filename) {
        mPath = filename;
        QFile file(filename);
        mTimer.stop();
        if(file.open(QIODevice::ReadOnly)) {
            QFileInfo fileInfo(file);
            const QString dirName = fileInfo.path();
            SkString skDirName;
            const auto stdDirName = dirName.toStdString();
            skDirName.set(stdDirName.c_str());
//            auto rp = skresources::CachingResourceProvider::Make(
//                        skresources::DataURIResourceProviderProxy::Make(
//                            skresources::FileResourceProvider::Make(skDirName, true), true));
//            auto logger = sk_make_sp<SkottieLogger>();
            QTextStream stream(&file);
            const auto data = stream.readAll();
            const auto stdData = data.toStdString();
            const auto stdFilename = filename.toStdString();
            mAnim = skottie::Animation::Builder()
//                    .setLogger(logger)
//                    .setResourceProvider(rp)
                    .makeFromFile(stdFilename.data());
            if(mAnim) {
                mFrame = 0;
                const double fps = mAnim->fps();
                mFrameCount = qCeil(fps*mAnim->duration());
                const int mSecInterval = qRound(1000/fps);
                mTimer.setInterval(mSecInterval);
                mTimer.start();
            } else clear();

            file.close();
        } else clear();
    }
protected:
    void contextMenuEvent(QContextMenuEvent *e) {
        QMenu* menu = new QMenu(this);
        const auto action = menu->addAction("Restart");
        action->setIcon(QIcon::fromTheme(QStringLiteral("view-refresh")));
        connect(action, &QAction::triggered, this, &JsonPreview::reload);
        menu->popup(e->globalPos());
        connect(menu, &QMenu::aboutToHide, menu, &QObject::deleteLater);
    }
private:
    void clear() {
        mAnim.reset();
        mTimer.stop();
        QLabel::clear();
    }

    void incFrame() {
        if(!mAnim) return;
        mFrame++;
        if(mFrame >= mFrameCount) mFrame = 0;
        mAnim->seekFrame(mFrame);

        const QSize size = this->size();
        QImage image(size, QImage::Format_RGBA8888_Premultiplied);
        SkBitmap bitmap;
        const auto info = SkiaHelpers::getPremulRGBAInfo(size.width(), size.height());
        bitmap.installPixels(info, image.bits(), image.bytesPerLine());
        bitmap.eraseColor(SK_ColorTRANSPARENT);
        SkCanvas canvas(bitmap);
        const SkRect dstRect = SkRect::MakeWH(size.width(), size.height());
        mAnim->render(&canvas, &dstRect);
        canvas.flush();
        QPixmap pixmap;
        pixmap.convertFromImage(image);
        setPixmap(pixmap);
    }

    QString mPath;
    int mFrame;
    int mFrameCount;
    sk_sp<skottie::Animation> mAnim;
    QTimer mTimer;
};


ExportJsonDialog::ExportJsonDialog(QWidget* const parent) :
    QDialog(parent) {

    setWindowTitle("Export JSON");

    const auto settingsLayout = new QVBoxLayout();

    const auto twoColLayout = new TwoColumnLayout();

    const auto document = Document::sInstance;
    mScene = new SceneChooser(*document, false, this);
    auto scene = *document->fActiveScene;
    if(!scene) {
        const auto& visScenes = document->fVisibleScenes;
        if(!visScenes.empty()) scene = visScenes.begin()->first;
        else {
            const auto& scenes = document->fScenes;
            if(!scenes.isEmpty()) scene = scenes.first().get();
        }
    }
    mScene->setCurrentScene(scene);

    const auto sceneButton = new QPushButton(mScene->title(), this);
    sceneButton->setMenu(mScene);

    mFirstFrame = new QSpinBox(this);
    mLastFrame = new QSpinBox(this);

    const int minFrame = scene ? scene->getMinFrame() : 0;
    const int maxFrame = scene ? scene->getMaxFrame() : 0;

    mFirstFrame->setRange(-99999, maxFrame);
    mFirstFrame->setValue(minFrame);
    mLastFrame->setRange(minFrame, 99999);
    mLastFrame->setValue(maxFrame);

    mBackground = new QCheckBox("Background", this);
    mBackground->setChecked(true);

    twoColLayout->addPair(new QLabel("Scene:"), sceneButton);
    twoColLayout->addSpacing(MIN_WIDGET_DIM);
    twoColLayout->addPair(new QLabel("First Frame:"), mFirstFrame);
    twoColLayout->addPair(new QLabel("Last Frame:"), mLastFrame);

    settingsLayout->addLayout(twoColLayout);
    settingsLayout->addSpacing(MIN_WIDGET_DIM);
    settingsLayout->addWidget(mBackground);
    settingsLayout->addSpacing(MIN_WIDGET_DIM);

    connect(mFirstFrame, qOverload<int>(&QSpinBox::valueChanged),
            mLastFrame, &QSpinBox::setMinimum);
    connect(mLastFrame, qOverload<int>(&QSpinBox::valueChanged),
            mFirstFrame, &QSpinBox::setMaximum);

    const auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok |
                                              QDialogButtonBox::Cancel);

    connect(mScene, &SceneChooser::currentChanged,
            this, [this, buttons, sceneButton](Canvas* const scene) {
        buttons->button(QDialogButtonBox::Ok)->setEnabled(scene);
        sceneButton->setText(mScene->title());
    });

    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        const QString dir = Document::sInstance->projectDirectory();
        const QString fileType = tr("JSON Files %1", "ExportDialog_FileType");
        QString saveAs = eDialogs::saveFile("Export JSON", dir,
                                            fileType.arg("(*.json)"));
        if(saveAs.isEmpty()) return;
        if(saveAs.right(4) != ".json") saveAs += ".json";
        const bool success = exportTo(saveAs);
        if(success) accept();
    });

    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    settingsLayout->addWidget(buttons, 0, Qt::AlignBottom);

    mPreview = new JsonPreview(this);
    mPreviewButton = new QPushButton("Preview", this);
    buttons->addButton(mPreviewButton, QDialogButtonBox::ActionRole);
    connect(mPreviewButton, &QPushButton::released, this, [this]() {
        if(!mPreviewFile) {
            const QString templ = "enve_json_preview_XXXXXX.json";
            mPreviewFile = qsptr<QTemporaryFile>::create(templ);
            mPreviewFile->open();
            mPreviewFile->close();
        }
        const auto task = exportTo(mPreviewFile->fileName());
        if(!task) return;
        QPointer<ExportJsonDialog> ptr = this;
        task->addDependent(
        {[ptr]() {
            if(ptr) {
                ptr->mPreview->load(ptr->mPreviewFile->fileName());
            }
        }, nullptr});
    });

    const auto settingsWidget = new QWidget(this);
    settingsWidget->setLayout(settingsLayout);
    settingsWidget->setSizePolicy(QSizePolicy::Fixed,
                                  QSizePolicy::MinimumExpanding);

    const auto mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(mPreview);
    mainLayout->addSpacing(MIN_WIDGET_DIM/2);
    mainLayout->addWidget(settingsWidget);
    setLayout(mainLayout);
}

ComplexTask* ExportJsonDialog::exportTo(const QString& file) {
    try {
        const auto scene = mScene->getCurrentScene();
        if(!scene) RuntimeThrow("No scene selected");
        const int firstFrame = mFirstFrame->value();
        const int lastFrame = mLastFrame->value();

        const bool background = mBackground->isChecked();

        const FrameRange frameRange{firstFrame, lastFrame};
        const qreal fps = scene->getFps();

        const auto task = new JsonExporter(file, scene, frameRange, fps, background);
        const auto taskSPtr = qsptr<JsonExporter>(task, &QObject::deleteLater);
        task->nextStep();
        TaskScheduler::instance()->addComplexTask(taskSPtr);
        return task;
    } catch(const std::exception& e) {
        gPrintExceptionCritical(e);
        return nullptr;
    }
}
