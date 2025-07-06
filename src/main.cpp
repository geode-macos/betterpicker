#include <Geode/Geode.hpp>
#include <algorithm>

using namespace geode::prelude;

#include <Color.hpp>
#include <Geode/modify/CCControlColourPicker.hpp>
#include <Geode/modify/ColorSelectPopup.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <TouchArea.hpp>

struct ConvCbs {
    std::function<Vec3(float, float, float)> fromRgb;
    std::function<Vec3(float, float, float)> toRgb;
};

ConvCbs currentConvCbs() {
    auto space = Mod::get()->getSettingValue<std::string>("color-space");

    if (space == "HSV") {
        return {rgb_to_hsv, hsv_to_rgb};
    } else if (space == "HSL") {
        return {rgb_to_hsl, hsl_to_rgb};
    } else if (space == "OkHSV") {
        return {srgb_to_okhsv, okhsv_to_srgb};
    } else if (space == "OkHSL") {
        return {srgb_to_okhsl, okhsl_to_srgb};
    }

    return {rgb_to_hsv, hsv_to_rgb};
}

ccColor3B tripleToB(Vec3 v) {
    return ccColor3B(
        (GLubyte)(std::clamp(v.x, 0.0f, 1.0f) * 255.f),
        (GLubyte)(std::clamp(v.y, 0.0f, 1.0f) * 255.f),
        (GLubyte)(std::clamp(v.z, 0.0f, 1.0f) * 255.f)
    );
}

class $modify(MyCCControlColourPicker, CCControlColourPicker) {
    struct Fields {
        CCDrawNode* squareDraw;
        CCDrawNode* sliderDraw;
        CCSprite* pickerDot;
        CCSprite* sliderHandle;
        Vec3 hsvl;
        int squareDrawThrottle;

        EventListener<SettingChangedFilterV3> m_settingListener = {
            [this](std::shared_ptr<SettingV3> setting) { return ListenerResult::Propagate; },
            SettingChangedFilterV3(Mod::get(), "color-space")
        };
    };

    virtual bool init();
    ccColor3B const& getColorValue() const;
    virtual void setColorValue(ccColor3B const& v);

    void hsvlChanged(bool setRgb, bool redrawSquare);

    void onOptions(CCObject* sender);
};

bool MyCCControlColourPicker::init() {
    if (!CCControlColourPicker::init()) {
        return false;
    }

    getChildByType<CCSpriteBatchNode>(0)->setVisible(false);
    getChildByType<CCControlHuePicker>(0)->setVisible(false);
    getChildByType<CCControlSaturationBrightnessPicker>(0)->setVisible(false);

    m_fields->squareDrawThrottle = 0;
    m_fields->m_settingListener = {
        [this](std::shared_ptr<SettingV3> setting) {
            setColorValue(m_rgb);

            return ListenerResult::Propagate;
        },
        SettingChangedFilterV3(Mod::get(), "color-space")
    };

    m_fields->squareDraw = CCDrawNode::create();
    addChild(m_fields->squareDraw);
    m_fields->squareDraw->setPosition(-80, -45);

    m_fields->sliderDraw = CCDrawNode::create();
    addChild(m_fields->sliderDraw);
    m_fields->sliderDraw->setPosition(55, -45);

    auto squareOutline = CCSprite::create("ColorSquareOutline.png"_spr);
    addChild(squareOutline);
    squareOutline->setPosition(CCPoint(-20, 15));
    auto sliderOutline = CCSprite::create("ColorSliderOutline.png"_spr);
    addChild(sliderOutline);
    sliderOutline->setPosition(CCPoint(65, 15));

    auto pickerDotOutline = CCSprite::create("PickerDotOutline.png"_spr);
    m_fields->pickerDot = CCSprite::create("PickerDot.png"_spr);
    addChild(m_fields->pickerDot);
    m_fields->pickerDot->addChild(pickerDotOutline);
    m_fields->pickerDot->setPosition(CCPoint(-80, -45));
    pickerDotOutline->setAnchorPoint(CCPoint(0, 0));

    auto sliderHandleOutline = CCSprite::create("SliderHandleOutline.png"_spr);
    m_fields->sliderHandle = CCSprite::create("SliderHandle.png"_spr);
    addChild(m_fields->sliderHandle);
    m_fields->sliderHandle->addChild(sliderHandleOutline);
    m_fields->sliderHandle->setPosition(CCPoint(65, -45));
    sliderHandleOutline->setAnchorPoint(CCPoint(0, 0));

    auto squareTouch = TouchArea::create(
        [this](CCPoint v) {
            if (v.x >= 0.0 && v.x <= 120.0 && v.y >= 0.0 && v.y <= 120.0) {
                m_fields->hsvl.y = v.x / 120.0;
                m_fields->hsvl.z = v.y / 120.0;
                hsvlChanged(true, false);

                return true;
            }
            return false;
        },
        [this](CCPoint v) {
            m_fields->hsvl.y = std::clamp(v.x, 0.f, 120.f) / 120.0;
            m_fields->hsvl.z = std::clamp(v.y, 0.f, 120.f) / 120.0;
            hsvlChanged(true, false);
        },
        [this](CCPoint v) {}
    );
    auto sliderTouch = TouchArea::create(
        [this](CCPoint v) {
            if (v.x >= 0.0 && v.x <= 20.0 && v.y >= 0.0 && v.y <= 120.0) {
                m_fields->hsvl.x = v.y / 120.0;
                m_fields->squareDrawThrottle = 0;
                hsvlChanged(true, true);

                return true;
            }
            return false;
        },
        [this](CCPoint v) {
            m_fields->hsvl.x = std::clamp(v.y, 0.f, 120.f) / 120.0;
            hsvlChanged(true, m_fields->squareDrawThrottle == 0);
            m_fields->squareDrawThrottle = (m_fields->squareDrawThrottle + 1) % 8;
        },
        [this](CCPoint v) {
            m_fields->hsvl.x = std::clamp(v.y, 0.f, 120.f) / 120.0;
            hsvlChanged(true, true);
        }
    );

    addChild(squareTouch);
    addChild(sliderTouch);
    squareTouch->setPosition(CCPoint(-80, -45));
    sliderTouch->setPosition(CCPoint(55, -45));

    auto menu = CCMenu::create();

    auto btnSprite = CCSprite::create("ColorOptions.png"_spr);
    btnSprite->setScale(0.6);
    auto btn =
        CCMenuItemSpriteExtra::create(btnSprite, this, menu_selector(MyCCControlColourPicker::onOptions));
    btn->setPosition(65.0, -68.0);
    menu->addChild(btn);
    menu->setPosition(CCPoint(0, 0));
    menu->setTouchPriority(CCTouchDispatcher::get()->getForcePrio() - 1);

    addChild(menu);

    return true;
}

ccColor3B const& MyCCControlColourPicker::getColorValue() const {
    return m_rgb;
}

void MyCCControlColourPicker::setColorValue(ccColor3B const& v) {
    m_rgb = v;
    m_delegate->colorValueChanged(m_rgb);

    m_fields->hsvl = currentConvCbs().fromRgb((float)v.r / 255.f, (float)v.g / 255.f, (float)v.b / 255.f);
    hsvlChanged(false, true);
}

void MyCCControlColourPicker::hsvlChanged(bool setRgb, bool redrawSquare) {
    auto conv = currentConvCbs();

    if (setRgb) {
        m_rgb = tripleToB(conv.toRgb(m_fields->hsvl.x, m_fields->hsvl.y, m_fields->hsvl.z));
        m_delegate->colorValueChanged(m_rgb);
    }

    float sliderVL = 1.0;
    {
        auto space = Mod::get()->getSettingValue<std::string>("color-space");
        if (space == "HSL") {
            sliderVL = 0.5;
        } else if (space == "OkHSL") {
            sliderVL = 0.6;
        }
    }

    m_fields->pickerDot->setPosition(CCPoint(-80 + m_fields->hsvl.y * 120.0, -45 + m_fields->hsvl.z * 120.0));
    m_fields->pickerDot->setColor(m_rgb);

    m_fields->sliderHandle->setPositionY(-45 + m_fields->hsvl.x * 120.0);
    m_fields->sliderHandle->setColor(tripleToB(conv.toRgb(m_fields->hsvl.x, 1.0, sliderVL)));

    if (redrawSquare) {
        m_fields->squareDraw->clear();
        for (int y = 0; y < 120; y += 2) {
            for (int x = 0; x < 120; x += 2) {
                auto v = conv.toRgb(m_fields->hsvl.x, x / 120.f, y / 120.f);
                m_fields->squareDraw->drawRect(
                    CCPoint(x, y),
                    CCPoint(x + 2, y + 2),
                    ccColor4F(v.x, v.y, v.z, 1.0),
                    0.0,
                    ccColor4F(0.0, 0.0, 0.0, 0.0)
                );
            }
        }
    }
    m_fields->sliderDraw->clear();
    for (int y = 0; y < 120; y += 1) {
        auto v = conv.toRgb(y / 120.f, 1.0, sliderVL);
        m_fields->sliderDraw->drawRect(
            CCPoint(0, y),
            CCPoint(20, y + 1),
            ccColor4F(v.x, v.y, v.z, 1.0),
            0.0,
            ccColor4F(0.0, 0.0, 0.0, 0.0)
        );
    }
}

void MyCCControlColourPicker::onOptions(CCObject* sender) {
    geode::openSettingsPopup(Mod::get());
}