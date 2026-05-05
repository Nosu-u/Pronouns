#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/modify/ProfilePage.hpp>
#include <hiimjasmine00.user_data_api/include/Events.hpp>
#include <hiimjasmine00.user_data_api/include/UserDataAPI.hpp>

using namespace geode::prelude;

class $modify(PR, ProfilePage) {
	 void loadPageFromUserInfo(GJUserScore* score) {
		ProfilePage::loadPageFromUserInfo(score);

		user_data::handleProfilePage(this, [this](GJUserScore* score) {
			auto mod = Mod::get();
        	auto winSize = CCDirector::get()->getWinSize();
			auto layer = m_mainLayer;
			auto playerStatus = layer->getChildByID("xjotabelikex.player-status/player-status-label");
			auto buttonText = CCLabelBMFont::create(mod->getSettingValue<std::string>("pronouns-text").c_str(), "bigFont.fnt");
			auto myID = GJAccountManager::get()->m_accountID;

			if (score->m_accountID == myID) {
				auto button = CCMenuItemSpriteExtra::create(buttonText, this, menu_selector(PR::onPrSettings));
				auto usernameMenu = layer->getChildByIDRecursive("username-menu");
				auto menu = CCMenu::create();

				buttonText->setColor(mod->getSettingValue<ccColor3B>("text-color"));
				buttonText->setString(mod->getSettingValue<std::string>("pronouns-text").c_str());

				menu->setPosition({ 0.0f, 0.0f });
				menu->setID("pr-menu"_spr);

				if (usernameMenu && playerStatus) {
					menu->setZOrder(usernameMenu->getZOrder() + 1);
				}

				if (playerStatus) {
        			button->setPosition({ 0.5f * winSize.width, 0.52f * winSize.height });
					button->setScale(0.4);
					button->m_baseScale = 0.4; // dont go to 1.0 you bitch
				} else {
					button->setPosition({ 0.5f * winSize.width, 0.83f * winSize.height });
					button->setScale(0.5);
					button->m_baseScale = 0.5; // dont go to 1.0 you bitch
				}

				button->setID("pr-button"_spr);

				menu->addChild(button);
				menu->updateLayout();

				layer->addChild(menu);
			} else {
				auto result = user_data::get<matjson::Value>(score);

    			if (result.isOk()) {
        			auto val = result.unwrap();
        			auto text = val["text"].asString().unwrapOr("");
        			auto colorStr = val["color"].asString().unwrapOr("");
        			if (!text.empty()) {
            			int r, g, b;
            			sscanf(colorStr.c_str(), "%d,%d,%d", &r, &g, &b);
            			buttonText->setString(text.c_str());
            			buttonText->setColor({ (GLubyte)r, (GLubyte)g, (GLubyte)b });
        			}
				}

				if (playerStatus) {
        			buttonText->setPosition({ 0.5f * winSize.width, 0.52f * winSize.height });
					buttonText->setScale(0.4);
				} else {
					buttonText->setPosition({ 0.5f * winSize.width, 0.83f * winSize.height });
					buttonText->setScale(0.5);
				}

				if (usernameMenu && playerStatus) {
					buttonText->setZOrder(usernameMenu->getZOrder() + 1);
				}

				layer->addChild(buttonText);
			}

			buttonText->setID("pr-button-text"_spr);
		});
	}

	void onPrSettings(CCObject*) {
        openSettingsPopup(Mod::get(), false);
    }
};

$on_mod(Loaded) {
	auto pronouns = Mod::get()->getSettingValue<std::string>("pronouns-text");
	auto c = Mod::get()->getSettingValue<cocos2d::ccColor3B>("text-color");
	matjson::Value data = matjson::makeObject({
    	{"text", pronouns},
    	{"color", fmt::format("{},{},{}", c.r, c.g, c.b)}
	});
	user_data::upload(data);
	
	listenForAllSettingChanges([](std::string_view key, std::shared_ptr<SettingV3> setting) {
		auto pronouns = Mod::get()->getSettingValue<std::string>("pronouns-text");
		auto c = Mod::get()->getSettingValue<cocos2d::ccColor3B>("text-color");
		matjson::Value data = matjson::makeObject({
    		{"text", pronouns},
    		{"color", fmt::format("{},{},{}", c.r, c.g, c.b)}
		});
		user_data::upload(data);
	});
}
