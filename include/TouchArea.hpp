
using namespace geode::prelude;

using TouchAreaStartCB = std::function<bool(CCPoint)>;
using TouchAreaMoveCB = std::function<void(CCPoint)>;
using TouchAreaEndCB = std::function<void(CCPoint)>;

class TouchArea: public CCLayer {
  public:
    static TouchArea* create(TouchAreaStartCB, TouchAreaMoveCB, TouchAreaEndCB);
    virtual bool init(TouchAreaStartCB, TouchAreaMoveCB, TouchAreaEndCB);
    virtual bool ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent);
    virtual void ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent);
    virtual void ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent);

    TouchAreaStartCB m_startCB;
    TouchAreaMoveCB m_moveCB;
    TouchAreaEndCB m_endCB;
};