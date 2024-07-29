
#include <fstream>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include "../src/serializer/serialize_overloads.h"
#include "../src/json/json_loader.h"

// SCENARIO("Dog Recover")
// {
//     using namespace model;
//     NSWE_Direction dir = NSWE_Direction::EAST;
//     DogSpeed spd{5., 5.};

//     DogCoordinates coord{10., 10.};
//     std::vector<Loot> loot{{1, 1}, {2, 2}, {3, 3}};
//     int scores = 50;
//     Dog scooby("scooby");
//     scooby.SetDogSpeedAndDirection(spd, dir);
//     scooby.SetDogCoordinates(coord);
//     for (auto i : loot)
//     {
//         scooby.PickUpLoot(i);
//     }
//     scooby.TakeScores(scores);

//     DogRepr scooby_ser(scooby);
//     Dog scooby_copy = scooby_ser.Restore();
//     CHECK(scooby_copy.GetId() == 0);
//     CHECK(scooby_copy.DogsCount() == 1);
//     CHECK(scooby_copy.GetName() == "scooby");
//     CHECK(scooby_copy.Score() == scores);
//     CHECK(scooby_copy.GetLootBag().size() == 3);
//     CHECK(scooby_copy.GetLootBag()[0] == Loot{1, 1});
//     CHECK(scooby_copy.GetLootBag()[1] == Loot{2, 2});
//     CHECK(scooby_copy.GetLootBag()[2] == Loot{3, 3});

//     /////////////////////////////////////////////////////////
//     {
//         std::ofstream ss("try.txt", std::ios::binary);
//         boost::archive::text_oarchive oa{ss};
//         const DogRepr scooby_ser(scooby);
//         oa << scooby_ser;
//         ss.close();
//     }
//     {
//         std::ifstream is("try.txt", std::ios::binary);

//         boost::archive::text_iarchive ia{is};
//         DogRepr scoob;
//         ia >> scoob;
//         Dog scooby_copy1 = scoob.Restore();

//         CHECK(scooby_copy1.GetId() == 0);
//         CHECK(scooby_copy1.DogsCount() == 1);
//         CHECK(scooby_copy1.GetName() == "scooby");
//         CHECK(scooby_copy1.Score() == scores);
//         CHECK(scooby_copy1.GetLootBag().size() == 3);
//         CHECK(scooby_copy1.GetLootBag()[0] == Loot{1, 1});
//         CHECK(scooby_copy1.GetLootBag()[1] == Loot{2, 2});
//         CHECK(scooby_copy1.GetLootBag()[2] == Loot{3, 3});
//     }
// }

// SCENARIO("Session Recover WithStream")
// {
//     const std::filesystem::path pth = "/home/user/cpp-backend/sprint4/problems/state_serialization/solution/data/config.json";
//     api::Play game = json_loader::LoadPlay(pth, 1, 0);
//     api::PlayObserver G1(game);
    
//     CHECK(G1.GameSessions().size() == 2);
//     CHECK(G1.GetSession("map1") != nullptr);
//     CHECK(G1.GetSession("town") != nullptr);

//     game.JoinGame("one", "map1");
//     game.JoinGame("two", "map1");
//     game.JoinGame("three", "map1");

//     game.ManualTick(25);
//     game.ManualTick(25);
//     game.ManualTick(25);

//     size_t items_on_map = G1.GetSession("map1")->GetLootList().size();
//     CHECK(G1.GetSession("map1")->GetDogs().size() == 3);
//     ArchiveSession arcsess(*G1.GetSession("map1"));

//     /////////////////////////////////////////////////////////
//     {
//         std::ofstream ss("tryx.txt", std::ios::binary);
//         boost::archive::text_oarchive oa{ss};
//         oa << arcsess;
//         ss.close();
//     }

//     ArchiveSession arcsess2;

//     {
//         std::ifstream is("tryx.txt", std::ios::binary);
//         boost::archive::text_iarchive ia{is};
//         ia >> arcsess2;
//     }

//     ///////////////////////////////////////////////////////////

//     api::Play game2 = json_loader::LoadPlay(pth, 1, 0);
//     api::PlayObserver G2(game2);
//     DearchivePlay dearchive(game2);
    
//     auto &g2s = *G2.GetSession("map1");
//     arcsess2.RestoreSession(dearchive);
//     CHECK(g2s.GetDogs().size() == 3);
//     CHECK(g2s.GetLootList().size() == items_on_map);

//     auto &or_loot = G1.GetSession("map1")->GetLootList();
//     auto &res_loot = g2s.GetLootList();
//     for (const auto &i : or_loot)
//     {
//         auto key = i.first;
//         CHECK(or_loot.at(key) == res_loot.at(key));
//     }

//     auto &or_dogs = G1.GetSession("map1")->GetDogs();
//     auto &res_dogs = g2s.GetDogs();
//     for (auto &&i : or_dogs)
//     {
//         auto key = i.first;
//         CHECK(or_dogs.at(key)->GetId() == res_dogs.at(key)->GetId());
//         CHECK(or_dogs.at(key)->GetLootBag().size() == res_dogs.at(key)->GetLootBag().size());
//         CHECK(or_dogs.at(key)->GetDodSpd().speed_x == res_dogs.at(key)->GetDodSpd().speed_x);
//         CHECK(or_dogs.at(key)->GetDodSpd().speed_y == res_dogs.at(key)->GetDodSpd().speed_y);
//         CHECK(or_dogs.at(key)->GetOrientation() == res_dogs.at(key)->GetOrientation());
//         CHECK(or_dogs.at(key)->GetDogCoord() == res_dogs.at(key)->GetDogCoord());
//         CHECK(or_dogs.at(key)->Score() == res_dogs.at(key)->Score());
//     }
// }

// SCENARIO("Player Recover")
// {
//     const std::filesystem::path pth = "/home/user/cpp-backend/sprint4/problems/state_serialization/solution/data/config.json";
//     api::Play game = json_loader::LoadPlay(pth, 1, 0);
//     game.JoinGame("one", "map1");
//     game.JoinGame("two", "map1");
//     game.JoinGame("three", "map1");

//     game.ManualTick(25);
//     game.ManualTick(25);
//     game.ManualTick(25);

//     api::PlayObserver game_obs(game);
//     DearchivePlay dearchive(game);
    
//     const auto &tok_pl = game_obs.GetTokenPlayers();
//     std::vector<std::shared_ptr<api::Player>> players;
//     // ПОМЕЩАЕМ ПЛЕЕРОВ В ВЕКТОР
//     for (auto &&player : tok_pl)
//     {
//         players.push_back(player.second);
//         std::cout << player.second->GetId() << " DOG: " << player.second->GetDogId() << std::endl;
//     }

//     // КОДИРУЕМ ПЛЕЕРОВ
//     std::vector<ArchivePlayer> archive_pl;
//     for (auto &&pl : players)
//     {
//         archive_pl.push_back(ArchivePlayer(*pl));
//     }

//     // ЗАГОНЯЕМ В ФАЙЛ
//     {
//         std::ofstream sso("players.txt", std::ios::binary);
//         boost::archive::text_oarchive oa{sso};
//         oa << archive_pl;
//         sso.close();
//     }

//     // ДОСТАЕМ ИЗ ФАЙЛА
//     std::vector<ArchivePlayer> archive_pl_as;
//     {
//         std::ifstream iss("players.txt", std::ios::binary);
//         boost::archive::text_iarchive ia{iss};
//         ia >> archive_pl_as;
//     }

//     // ДЕКОДИРУЕМ ИГРОКОВ
//     std::vector<api::Player> players_decoded;
//     for (auto &&pl : archive_pl_as)
//     {
//         players_decoded.push_back(pl.RestorePlayer(dearchive));
//     }
//     CHECK(players_decoded.size() == players.size());
//     for (size_t i = 0; i < players.size(); ++i)
//     {
//         CHECK(players_decoded[i].GetDogId() == players[i]->GetDogId());
//         CHECK(players_decoded[i].GetToken() == players[i]->GetToken());
//         CHECK(players_decoded[i].GetId() == players[i]->GetId());
//         CHECK(players_decoded[i].PlayersSession() ==
//               players[i]->PlayersSession());
//         CHECK(players_decoded[i].PlayersDog() == players[i]->PlayersDog());
//         CHECK(players_decoded[i].GetPlayersCount() == players[i]->GetPlayersCount());
//         CHECK(players_decoded[i].NeedRandom() == players[i]->NeedRandom());
//     }
// }

// SCENARIO("GAME TOTAL Recover")
// {

//     const std::filesystem::path pth = "/home/user/cpp-backend/sprint4/problems/state_serialization/solution/data/config.json";
//     api::Play game = json_loader::LoadPlay(pth, 1, 0);
//     game.JoinGame("one", "map1");
//     game.JoinGame("two", "map1");
//     game.JoinGame("three", "map1");

//     game.ManualTick(25);
//     game.ManualTick(25);
//     game.ManualTick(25);

//     api::PlayObserver game_obs(game);
//     DearchivePlay dearch(game);
//     auto& m1s = dearch.GetSessionListNotConst().at("map1");
//     m1s->AddLoot({0.0, 0.0}, 0);
//     m1s->AddLoot({10.0, 0.0}, 0);
//     auto& ddd = m1s->GetDogs();
     
//     for(auto & d : ddd){
//         d.second->PickUpLoot({1,1});
//     }
    
//     ArchiveGame gamearch(game_obs);
//     // ЗАГОНЯЕМ В ФАЙЛ
//     {
//         std::ofstream sso("game.txt", std::ios::binary);
//         boost::archive::text_oarchive oa{sso};
//         oa << gamearch;
//         sso.close();
//     }

//     // ДОСТАЕМ ИЗ ФАЙЛА
//     ArchiveGame gamearch2;
//     {
//         std::ifstream iss("game.txt", std::ios::binary);
//         boost::archive::text_iarchive ia{iss};
//         ia >> gamearch2;
//     }

//     api::Play game2 = json_loader::LoadPlay(pth, 1, 0);
//     api::PlayObserver game_obs2(game2);
    
//     DearchivePlay dearchive(game2);
//     gamearch2.RestoreGAME(dearchive);

//     const auto &sessions = game_obs.GetSessionList();
//     const auto &sess_restored = game_obs2.GetSessionList();
//     CHECK(sessions.size() == sess_restored.size());
//     for (auto &&sess : sessions)
//     {
//         const auto &sess_d = sessions.at(sess.first)->GetDogs();
//         const auto &restored_d = sess_restored.at(sess.first)->GetDogs();

//         CHECK(sess_d.size() == restored_d.size());
//         for (auto &&dog_pair : sess_d)
//         {
//             CHECK(sess_d.at(dog_pair.first)->GetId() == restored_d.at(dog_pair.first)->GetId());
//             CHECK(sess_d.at(dog_pair.first)->GetDodSpd().speed_x ==
//                   restored_d.at(dog_pair.first)->GetDodSpd().speed_x);
//             CHECK(sess_d.at(dog_pair.first)->GetDodSpd().speed_y ==
//                   restored_d.at(dog_pair.first)->GetDodSpd().speed_y);
//             CHECK(sess_d.at(dog_pair.first)->GetDogCoord() ==
//                   restored_d.at(dog_pair.first)->GetDogCoord());
//             CHECK(sess_d.at(dog_pair.first)->GetOrientation() ==
//                   restored_d.at(dog_pair.first)->GetOrientation());
//             CHECK(sess_d.at(dog_pair.first)->GetName() ==
//                   restored_d.at(dog_pair.first)->GetName());

//             auto &or_bag = sess_d.at(dog_pair.first)->GetLootBag();
//             auto &restored_bag = restored_d.at(dog_pair.first)->GetLootBag();
//             CHECK(or_bag.size() == restored_bag.size());
//             CHECK(!or_bag.empty()); 
//             CHECK(!restored_bag.empty());
//             for(size_t i = 0; i< or_bag.size(); ++i){
//               CHECK(or_bag[i] == restored_bag[i]);  
//             }


//         }
    
    
    
    
//     }
// }

SCENARIO("Play FILE ARCH")
{
  const std::filesystem::path pth = "/home/user/cpp-backend/sprint4/problems/state_serialization/solution/data/config.json";
    api::Play game = json_loader::LoadPlay(pth, 1, 0);
    api::PlayObserver obs1(game);
    
    game.JoinGame("one", "map1");
    game.ManualTick(25);
    game.ManualTick(25);
    game.ManualTick(25);
   
    std::filesystem::path pthx = "/home/user/filetest.txt";
    pthx = pthx.lexically_normal();
     
    serialization::ArchivePlay arch(game, pthx);
    arch.Archive();
     
    api::Play game2 = json_loader::LoadPlay(pth, 1, 0);
    api::PlayObserver obs2(game2);

    std::optional<serialization::ArchiveGame> restored = serialization::LoadOldGame(pthx);
    CHECK(restored.has_value());
    serialization::DearchivePlay dearch2(game2);
    (*restored).RestoreGAME(dearch2);




    obs1.PrintPlayers();
    std::cout<<"---------------------------------------<<"<<std::endl;
    obs2.PrintPlayers();

}

int main(int argc, char *argv[])
{
    int result = Catch::Session().run(argc, argv);
    return result;
}