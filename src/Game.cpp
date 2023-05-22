#include <functional> 
#include "Game.h"
#include "CollisionManager.h"
#include "PortalManager.h"
#include "Config.h"
#include "Log.h"
#include "ScreenDrawer.h"


Game::Game()
	:_gameManager(new GameManager())
	,_Control()
	,_player(nullptr)
{
	LOG_INFO("\r\n\n启动游戏");
	loadControl();
	loadGameManager();
	loadGamePlayer();
	loadPortal();
}


Game::~Game()
{
}



void Game::start() {
	LOG_INFO("game start");
	_gameManager->GetCurrentMap()->Print();
	ScreenDrawer::getInstance().swapBuffers();
	while (1)
	{
		_Control.handleInput()->execute(_player);
		_gameManager->GetCurrentMap()->Print();
		ScreenDrawer::getInstance().swapBuffers();
	}
}

void Game::loadGamePlayer()
{
	//根据配置文件获取玩家当前所在关卡、地图、位置
	GameLevel* nowLevel = _gameManager->GetNonConstLevel(
						Config::instance().getConfigData().player.levelID);
	GameMap* nowMap = nowLevel->GetNonConstMap(
						Config::instance().getConfigData().player.mapID);
	Location nowLoc = {
		Config::instance().getConfigData().player.x,
		Config::instance().getConfigData().player.y,
	};
	AutoGameObject obj = GameObjectFactory::getInstance().createPlayerFromConf(nowLoc);
	//将玩家的obj添加到对应地图中
	nowMap->AddGameObject(obj,nowLoc);
	//创建玩家
	AutoGamePlayer player(new
		GamePlayer(
			Config::instance().getConfigData().player.name,
			nowLevel, nowMap, nowLoc, obj,
			AutoCollisionManager(new CollisionManager(_gameManager))
			));
	//绑定玩家
	_player = player;
	_gameManager->BindPlayer(_player);
	
}

void Game::loadGameManager()
{
	//新建一个关卡
	AutoGameLevel level(new GameLevel(
		Config::instance().getConfigData().game.levels[0].levelid));
	//添加地图
	level->AddMap(GameMapFactory::getInstance().CreatUnwhiteTown());
	level->AddMap(GameMapFactory::getInstance().CreatRoute101());
	//添加关卡
	_gameManager->AddLevel(std::move(level));
}

void Game::loadGameLevel()
{
}

void Game::loadGameMap()
{
}




void Game::loadPortal()
{
	PortalKey key = {
		Config::instance().getConfigData().game.portals[0].fromLevel,
		Config::instance().getConfigData().game.portals[0].fromMap,
		{Config::instance().getConfigData().game.portals[0].fromX,
		Config::instance().getConfigData().game.portals[0].fromY,},
		
	};
	PortalInfo info = {
		Config::instance().getConfigData().game.portals[0].toLevel,
		Config::instance().getConfigData().game.portals[0].toMap,
		{Config::instance().getConfigData().game.portals[0].toX,
		Config::instance().getConfigData().game.portals[0].toY},
	};
	Location locationKey (get<2>(key));
	Location locationInfo(get<2>(info));
	AutoGameObject portal_1 = GameObjectFactory::getInstance().createPortalFromConf(locationKey);
	AutoGameObject portal_2 = GameObjectFactory::getInstance().createPortalFromConf(locationInfo);

	//添加到对应的地图
	_gameManager->GetNonConstLevel(get<0>(key))
				->GetNonConstMap(get<1>(key))
				->AddGameObject(portal_1,locationKey);
	_gameManager->GetNonConstLevel(get<0>(info))
				->GetNonConstMap(get<1>(info))
				->AddGameObject(portal_2,locationInfo);
	//注册到PortalManager
	PORTAL_MANAGER.AddPortal(key,info);
	PORTAL_MANAGER.AddPortal(info,key);
}

void Game::loadControl() {
	_Control.setButtonA(std::make_unique<leftMove>());
	_Control.setButtonD(std::make_unique<rightMove>());
	_Control.setButtonW(std::make_unique<upMove>());
	_Control.setButtonS(std::make_unique<downMove>());
	_Control.setButtonQ(std::make_unique<Quit>());
	_Control.setNothing(std::make_unique<Nothing>());
}
