#ifndef RENDERHANDLER_H
#define RENDERHANDLER_H
#include "framerange.h"
#include "GUI/audiohandler.h"
#include "smartPointers/sharedpointerdefs.h"

class Canvas;
class RenderInstanceSettings;
class SoundComposition;
class Document;

class RenderHandler : public QObject {
    Q_OBJECT
public:
    RenderHandler(Document& document,
                  AudioHandler &audioHandler);

    void interruptPreview();
    void outOfMemory();
    void interruptPreviewRendering();
    void interruptOutputRendering();

    void playPreview();
    void stopPreview();
    void pausePreview();
    void resumePreview();
    void renderPreview();
    void renderFromSettings(RenderInstanceSettings * const settings);
signals:
    void previewBeingRendered();
    void previewPaused();
    void previewBeingPlayed();
    void previewFinished();

    void queTasksAndUpdate();
private:
    void setFrameAction(const int frame);
    void setCurrentScene(Canvas * const scene);

    void playPreviewAfterAllTasksCompleted();

    void nextSaveOutputFrame();
    void nextPreviewRenderFrame();
    void nextPreviewFrame();
    void nextCurrentRenderFrame();

    void setRenderingPreview(const bool bT);
    void setPreviewing(const bool bT);
    void clearPreview();

    void startAudio();
    void audioPushTimerExpired();
    void stopAudio();

    Document& mDocument;

    // AUDIO
    AudioHandler& mAudioHandler;
    qptr<SoundComposition> mCurrentSoundComposition;
    // AUDIO

    Canvas* mCurrentScene = nullptr;
    QTimer *mPreviewFPSTimer = nullptr;
    RenderInstanceSettings *mCurrentRenderSettings = nullptr;

    int mCurrentPreviewFrame;
    int mMaxPreviewFrame;

    //! @brief true if preview is currently playing
    bool mPreviewing = false;
    //! @brief true if currently preview is being rendered
    bool mRenderingPreview = false;

    int mCurrentEncodeFrame;
    int mCurrentEncodeSoundSecond;
    int mFirstEncodeSoundSecond;

    FrameRange mCurrRenderRange;
    int mCurrentRenderFrame;
    int mMinRenderFrame = 0;
    int mMaxRenderFrame = 0;

    int mSavedCurrentFrame = 0;
    qreal mSavedResolutionFraction = 100;
};

#endif // RENDERHANDLER_H
