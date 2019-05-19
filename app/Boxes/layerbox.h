﻿#ifndef LAYERBOX_H
#define LAYERBOX_H
#include "Boxes/groupbox.h"

class PathBox;
class PathEffectAnimators;

class LayerBox : public GroupBox {
    friend class SelfRef;
protected:
    LayerBox(const BoundingBoxType& type = TYPE_LAYER);
public:
    bool SWT_isLayerBox() const;
    void anim_setAbsFrame(const int &frame);
    //MovablePoint *getPointAt(const QPointF &absPos, const CanvasMode &currentMode);


//    QPointF getRelCenterPosition();
    void drawPixmapSk(SkCanvas * const canvas,
                      GrContext* const grContext);
    bool shouldPaintOnImage() const;

    stdsptr<BoundingBoxRenderData> createRenderData();

    void setupRenderData(const qreal &relFrame,
                         BoundingBoxRenderData * const data);

    qsptr<BoundingBox> createLink();


    void schedulerProccessed();
protected:
    static bool mCtrlsAlwaysVisible;
};

#endif // LAYERBOX_H
