#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/utils/cocos.hpp>
using namespace geode::prelude;

constexpr float STATS_X = 40.0f;
constexpr float Y_START = 240.0f;
constexpr float DELTA_Y = 35.0f;

// Funciones para guardar/cargar completions por nivel
void loadCompletions(const std::string& key, int& completions) {
    completions = Mod::get()->getSavedValue<int>("level_completions_" + key, 0);
}
void saveCompletions(const std::string& key, int completions) {
    Mod::get()->setSavedValue<int>("level_completions_" + key, completions);
}

// PLAYLAYER: Guarda completions
class $modify(PlayLayerHook, PlayLayer) {
    struct Fields {
        bool completed = false;
    };

    void resetLevel() {
        m_fields->completed = false;
        PlayLayer::resetLevel();
    }

    void levelComplete() {
        PlayLayer::levelComplete();
        if (!m_fields->completed) {
            m_fields->completed = true;
            // Guardar completions
            auto level = m_level;
            if (level) {
                std::string levelKey = fmt::format("{}", level->m_levelID.value());
                int completions = Mod::get()->getSavedValue<int>("level_completions_" + levelKey, 0);
                completions++;
                Mod::get()->setSavedValue<int>("level_completions_" + levelKey, completions);
            }
        }
    }
};

// PAUSELAYER: Muestra las estadísticas según settings y opacidad
class $modify(PauseLayerHook, PauseLayer) {
    struct Fields {
        CCLabelBMFont* attemptsLabel = nullptr;
        CCLabelBMFont* attemptsValue = nullptr;
        CCLabelBMFont* jumpsLabel = nullptr;
        CCLabelBMFont* jumpsValue = nullptr;
        CCLabelBMFont* completionsLabel = nullptr;
        CCLabelBMFont* completionsValue = nullptr;
        CCLabelBMFont* objectsLabel = nullptr;
        CCLabelBMFont* objectsValue = nullptr;
        CCLabelBMFont* deviceLabel = nullptr;
        CCLabelBMFont* deviceValue = nullptr;
        CCLabelBMFont* gamemodeLabel = nullptr;
        CCLabelBMFont* gamemodeValue = nullptr;
    };
public:
    void customSetup() override {
        PauseLayer::customSetup();

        auto playLayer = GameManager::sharedState()->getPlayLayer();
        if (!playLayer) return;
        auto level = playLayer->m_level;
        if (!level) return;

        float textOpacity = Mod::get()->getSettingValue<float>("text_opacity");
        float statsScale = Mod::get()->getSettingValue<float>("stats_scale");

        // Configuración para mostrar u ocultar estadísticas
        bool showAttempts = Mod::get()->getSettingValue<bool>("show_attempts");
        bool showJumps = Mod::get()->getSettingValue<bool>("show_jumps");
        bool showCompletions = Mod::get()->getSettingValue<bool>("show_completions");
        bool showObjectCount = Mod::get()->getSettingValue<bool>("show_object_count");
        bool showDevice = Mod::get()->getSettingValue<bool>("show_device");
        bool showGamemode = Mod::get()->getSettingValue<bool>("show_gamemode");

        float y = Y_START;

        // ATTEMPTS
        if (showAttempts) {
            int totalAttempts = level->m_attempts;
            m_fields->attemptsLabel = CCLabelBMFont::create("Attemps", "goldFont.fnt");
            m_fields->attemptsLabel->setAnchorPoint({ 0.5f, 0.5f });
            m_fields->attemptsLabel->setScale(0.33f * statsScale);
            m_fields->attemptsLabel->setColor({ 200,200,200 });
            m_fields->attemptsLabel->setOpacity(static_cast<GLubyte>(textOpacity * 255));
            m_fields->attemptsLabel->setPosition({ STATS_X, y });
            this->addChild(m_fields->attemptsLabel, 100);

            m_fields->attemptsValue = CCLabelBMFont::create(fmt::format("{}", totalAttempts).c_str(), "bigFont.fnt");
            m_fields->attemptsValue->setAnchorPoint({ 0.5f, 0.5f });
            m_fields->attemptsValue->setScale(0.44f * statsScale);
            m_fields->attemptsValue->setColor({ 255,255,255 });
            m_fields->attemptsValue->setOpacity(static_cast<GLubyte>(textOpacity * 255));
            m_fields->attemptsValue->setPosition({ STATS_X, y - 13.0f * statsScale });
            this->addChild(m_fields->attemptsValue, 100);

            y -= DELTA_Y * statsScale;
        }

        // JUMPS
        if (showJumps) {
            int totalJumps = level->m_jumps;
            m_fields->jumpsLabel = CCLabelBMFont::create("Jumps", "goldFont.fnt");
            m_fields->jumpsLabel->setAnchorPoint({ 0.5f, 0.5f });
            m_fields->jumpsLabel->setScale(0.33f * statsScale);
            m_fields->jumpsLabel->setColor({ 200,200,200 });
            m_fields->jumpsLabel->setOpacity(static_cast<GLubyte>(textOpacity * 255));
            m_fields->jumpsLabel->setPosition({ STATS_X, y });
            this->addChild(m_fields->jumpsLabel, 100);

            m_fields->jumpsValue = CCLabelBMFont::create(fmt::format("{}", totalJumps).c_str(), "bigFont.fnt");
            m_fields->jumpsValue->setAnchorPoint({ 0.5f, 0.5f });
            m_fields->jumpsValue->setScale(0.44f * statsScale);
            m_fields->jumpsValue->setColor({ 255,255,255 });
            m_fields->jumpsValue->setOpacity(static_cast<GLubyte>(textOpacity * 255));
            m_fields->jumpsValue->setPosition({ STATS_X, y - 13.0f * statsScale });
            this->addChild(m_fields->jumpsValue, 100);

            y -= DELTA_Y * statsScale;
        }

        // COMPLETIONS
        if (showCompletions) {
            std::string levelKey = fmt::format("{}", level->m_levelID.value());
            int completions = 0;
            loadCompletions(levelKey, completions);

            m_fields->completionsLabel = CCLabelBMFont::create("Completed", "goldFont.fnt");
            m_fields->completionsLabel->setAnchorPoint({ 0.5f, 0.5f });
            m_fields->completionsLabel->setScale(0.33f * statsScale);
            m_fields->completionsLabel->setColor({ 200,200,200 });
            m_fields->completionsLabel->setOpacity(static_cast<GLubyte>(textOpacity * 255));
            m_fields->completionsLabel->setPosition({ STATS_X, y });
            this->addChild(m_fields->completionsLabel, 100);

            m_fields->completionsValue = CCLabelBMFont::create(fmt::format("{}", completions).c_str(), "bigFont.fnt");
            m_fields->completionsValue->setAnchorPoint({ 0.5f, 0.5f });
            m_fields->completionsValue->setScale(0.44f * statsScale);
            m_fields->completionsValue->setColor({ 255,255,255 });
            m_fields->completionsValue->setOpacity(static_cast<GLubyte>(textOpacity * 255));
            m_fields->completionsValue->setPosition({ STATS_X, y - 13.0f * statsScale });
            this->addChild(m_fields->completionsValue, 100);

            y -= DELTA_Y * statsScale;
        }

        // OBJECT COUNT
        if (showObjectCount) {
            int objectCount = level->m_objectCount;
            m_fields->objectsLabel = CCLabelBMFont::create("All Objs", "goldFont.fnt");
            m_fields->objectsLabel->setAnchorPoint({ 0.5f, 0.5f });
            m_fields->objectsLabel->setScale(0.33f * statsScale);
            m_fields->objectsLabel->setColor({ 200,200,200 });
            m_fields->objectsLabel->setOpacity(static_cast<GLubyte>(textOpacity * 255));
            m_fields->objectsLabel->setPosition({ STATS_X, y });
            this->addChild(m_fields->objectsLabel, 100);

            m_fields->objectsValue = CCLabelBMFont::create(fmt::format("{}", objectCount).c_str(), "bigFont.fnt");
            m_fields->objectsValue->setAnchorPoint({ 0.5f, 0.5f });
            m_fields->objectsValue->setScale(0.44f * statsScale);
            m_fields->objectsValue->setColor({ 255,255,255 });
            m_fields->objectsValue->setOpacity(static_cast<GLubyte>(textOpacity * 255));
            m_fields->objectsValue->setPosition({ STATS_X, y - 13.0f * statsScale });
            this->addChild(m_fields->objectsValue, 100);

            y -= DELTA_Y * statsScale;
        }

        // DEVICE
        if (showDevice) {
            std::string device = "PC";
            switch (CCApplication::sharedApplication()->getTargetPlatform()) {
            case kTargetIphone:
            case kTargetIpad:
            case kTargetAndroid: device = "Mobile"; break;
            default: device = "PC";
            }
            m_fields->deviceLabel = CCLabelBMFont::create("DEVICE", "goldFont.fnt");
            m_fields->deviceLabel->setAnchorPoint({ 0.5f, 0.5f });
            m_fields->deviceLabel->setScale(0.33f * statsScale);
            m_fields->deviceLabel->setColor({ 200,200,200 });
            m_fields->deviceLabel->setOpacity(static_cast<GLubyte>(textOpacity * 255));
            m_fields->deviceLabel->setPosition({ STATS_X, y });
            this->addChild(m_fields->deviceLabel, 100);

            m_fields->deviceValue = CCLabelBMFont::create(device.c_str(), "bigFont.fnt");
            m_fields->deviceValue->setAnchorPoint({ 0.5f, 0.5f });
            m_fields->deviceValue->setScale(0.44f * statsScale);
            m_fields->deviceValue->setColor({ 255,255,255 });
            m_fields->deviceValue->setOpacity(static_cast<GLubyte>(textOpacity * 255));
            m_fields->deviceValue->setPosition({ STATS_X, y - 13.0f * statsScale });
            this->addChild(m_fields->deviceValue, 100);

            y -= DELTA_Y * statsScale;
        }

        // GAMEMODE (Clasic/Plat)
        if (showGamemode) {
            std::string gamemode = playLayer->m_isPlatformer ? "Plat" : "Clasic";
            m_fields->gamemodeLabel = CCLabelBMFont::create("Gamemode", "goldFont.fnt");
            m_fields->gamemodeLabel->setAnchorPoint({ 0.5f, 0.5f });
            m_fields->gamemodeLabel->setScale(0.33f * statsScale);
            m_fields->gamemodeLabel->setColor({ 200,200,200 });
            m_fields->gamemodeLabel->setOpacity(static_cast<GLubyte>(textOpacity * 255));
            m_fields->gamemodeLabel->setPosition({ STATS_X, y });
            this->addChild(m_fields->gamemodeLabel, 100);

            m_fields->gamemodeValue = CCLabelBMFont::create(gamemode.c_str(), "bigFont.fnt");
            m_fields->gamemodeValue->setAnchorPoint({ 0.5f, 0.5f });
            m_fields->gamemodeValue->setScale(0.44f * statsScale);
            m_fields->gamemodeValue->setColor({ 255,255,255 });
            m_fields->gamemodeValue->setOpacity(static_cast<GLubyte>(textOpacity * 255));
            m_fields->gamemodeValue->setPosition({ STATS_X, y - 13.0f * statsScale });
            this->addChild(m_fields->gamemodeValue, 100);

            y -= DELTA_Y * statsScale;
        }
    }
};