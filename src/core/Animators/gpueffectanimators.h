#ifndef GPUEFFECTANIMATORS_H
#define GPUEFFECTANIMATORS_H
#include "Animators/dynamiccomplexanimator.h"
#include "smartPointers/sharedpointerdefs.h"
#include "GPUEffects/gpueffect.h"
#include "ShaderEffects/shadereffect.h"

class BoundingBox;
struct BoxRenderData;
qsptr<GpuEffect> readIdCreateGPURasterEffect(QIODevice * const src);
typedef DynamicComplexAnimator<
    GpuEffect, &GpuEffect::writeIdentifier,
    &readIdCreateGPURasterEffect> GPUEffectAnimatorsBase;
class GPUEffectAnimators : public GPUEffectAnimatorsBase {
    friend class SelfRef;
protected:
    GPUEffectAnimators(BoundingBox * const parentBox);
public:
    bool SWT_isRasterGPUEffectAnimators() const { return true; }

    BoundingBox *getParentBox() { return mParentBox_k; }
    bool hasEffects();
    QMargins getEffectsMargin(const qreal relFrame) const;
    bool unbound() const;

    void addEffects(const qreal relFrame,
                    BoxRenderData * const data);

    void updateIfUsesProgram(const ShaderEffectProgram * const program);
    //void readRasterEffect(QIODevice *target);
private:
    void updateUnbound();

    bool mUnbound = false;
    BoundingBox * const mParentBox_k;
};

#endif // GPUEFFECTANIMATORS_H