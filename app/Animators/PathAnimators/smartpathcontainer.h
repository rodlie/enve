﻿#ifndef SMARTPATHCONTAINER_H
#define SMARTPATHCONTAINER_H
#include "simplemath.h"
#include "skia/skiaincludes.h"
#include "exceptions.h"
#include "pointhelpers.h"

struct Node {
    enum Type : char {
        DUMMY, DISSOLVED, NORMAL, MOVE
    };

    Node() : Node(DUMMY) {}

    Node(const Type& type) { mType = type; }

    Node(const QPointF& c0, const QPointF& p1, const QPointF& c2) {
        fC0 = c0;
        fP1 = p1;
        fC2 = c2;
        mType = NORMAL;
    }

    Node(const qreal& t) {
        fT = t;
        mType = DISSOLVED;
    }

    bool isMove() const { return mType == MOVE; }

    bool isNormal() const { return mType == NORMAL; }

    bool isDummy() const { return mType == DUMMY; }

    bool isDissolved() const { return mType == DISSOLVED; }

    void switchPrevAndNext() {
        const int prevT = mPrevNodeId;
        mPrevNodeId = mNextNodeId;
        mNextNodeId = prevT;
    }

    int getNextNodeId() const {
        if(isDummy() || isMove()) return -1;
        return mNextNodeId;
    }
    int getPrevNodeId() const {
        if(isDummy() || isMove()) return -1;
        return mPrevNodeId;
    }

    bool hasPreviousNode() const {
        return getPrevNodeId() >= 0;
    }

    bool hasNextNode() const {
        return getNextNodeId() >= 0;
    }

    void setNextNodeId(const int& nextNodeId) {
        mNextNodeId = nextNodeId;
    }

    void setPrevNodeId(const int& prevNodeId) {
        mPrevNodeId = prevNodeId;
    }

    QPointF fC0;
    QPointF fP1;
    QPointF fC2;

    //! @brief T value for segment defined by previous and next normal node
    qreal fT;

    void setType(const Type& type) {
        mType = type;
        if(isDummy() || isMove()) {
            mNextNodeId = -1;
            mPrevNodeId = -1;
        }
    }

    const Type& getType() const { return mType; }

    void shiftIdsGreaterThan(const int& greater, const int& shiftBy) {
        if(mNextNodeId) if(mNextNodeId > greater) mNextNodeId += shiftBy;
        if(mPrevNodeId) if(mPrevNodeId > greater) mPrevNodeId += shiftBy;
    }

    void shiftIdsSmallerThan(const int& smaller, const int& shiftBy) {
        if(mNextNodeId) if(mNextNodeId < smaller) mNextNodeId += shiftBy;
        if(mPrevNodeId) if(mPrevNodeId < smaller) mPrevNodeId += shiftBy;
    }
private:
    Type mType;

    //! @brief Next connected node id in the list.
    //! Used with dissolved node and all normal nodes
    int mNextNodeId = -1;

    //! @brief Previous connected node id in the list.
    //! Used with dissolved node and all normal nodes
    int mPrevNodeId = -1;
};

#include "framerange.h"
#include "smartPointers/stdselfref.h"
#include "smartPointers/stdpointer.h"

class SmartPath : public StdSelfRef {
    friend class StdSelfRef;
    enum Neighbour { NONE, NEXT, PREV, BOTH = NEXT | PREV };
public:
    void removeNodeFromList(const int& nodeId);

    Node& insertNodeToList(const int& nodeId, const Node& node);

    void actionRemoveNormalNode(const int& nodeId);

    void actionAddFirstNode(const QPointF& c0,
                            const QPointF& p1,
                            const QPointF& c2);

    void actionInsertNormalNodeAfter(const int& afterId, const qreal& t);

    void actionAddNormalNodeAtEnd(const int& nodeId,
                                  const QPointF& c0,
                                  const QPointF& p1,
                                  const QPointF& c2);

    void actionPromoteDissolvedNodeToNormal(const int& nodeId);

    void actionDisconnectNodes(const int& node1Id, const int& node2Id);

    void actionConnectNodes(const int& node1Id, const int& node2Id);

    void normalOrMoveNodeInsertedToPrev(const int& insertedNodeId);

    void dissolvedOrDummyNodeInsertedToPrev(const int& insertedNodeId);

    void normalOrMoveNodeInsertedToNext(const int& insertedNodeId);

    void dissolvedOrDummyNodeInsertedToNext(const int& insertedNodeId);

    void removeNodeWithIdAndTellPrevToDoSame(const int& nodeId);

    void removeNodeWithIdAndTellNextToDoSame(const int& nodeId);

    const Node::Type& nodeType(const int& nodeId) const;

    bool isNodeNormal(const int& nodeId) const;

    bool isNodeDissolved(const int& nodeId) const;

    bool isNodeDummy(const int& nodeId) const;

    QList<int> updateAllNodesTypeAfterNeighbourChanged();

    qreal prevT(const int& nodeId) const;

    qreal nextT(const int& nodeId) const;

    //! @brief Returns true if changed.
    bool updateNodeTypeAfterNeighbourChanged(const int& nodeId);

    void setPrev(SmartPath * const prev);

    void setNext(SmartPath * const next);

    const QList<Node>& getNodes() const;

    SkPath getPathAt() const;
    SkPath getPathForPrev() const;
    SkPath getPathForNext() const;

    SkPath interpolateWithNext(const qreal& nextWeight) const;
    SkPath interpolateWithPrev(const qreal& nextWeight) const;
protected:
    SmartPath();
    SmartPath(const QList<Node>& nodes);
private:
    SkPath getPathFor(SmartPath * const neighbour) const;

    stdptr<SmartPath> mPrev;
    stdptr<SmartPath> mNext;
    QList<Node> mNodes;
};

#endif // SMARTPATHCONTAINER_H
