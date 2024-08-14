#include "timer_controller.h"

namespace api
{
    //ВРЕМЯ УДАЛЕНИЯ ИГРОКА
    std::chrono::milliseconds TCC::retire_time_;
    //МОМЕНТ ВРЕМЕНИ ВХОДА ИГРОКА
    std::unordered_map<std::string , std::unordered_map<size_t, std::chrono::system_clock::time_point >> TCC::time_entered_;
    //ИНФО О ИГРОКЕ
    std::unordered_map<std::string , std::unordered_map<size_t, IdToken>> TCC::info_;
    //СЧЕТЧИК БЕЗДЕЙСТВИЯ
    std::unordered_map<std::string, std::unordered_map<size_t, Activity>> TCC::activity_;

    TCC::TCC() = default;
    // УСТАНОВКА ВРЕМЕНИ ПРОСТОЯ ДО КИКА
    void TCC::SetRetireTime(std::chrono::milliseconds time)
    {
      retire_time_ = time;
    }

   
    void TCC::JoinToTimeObserve(const std::string &map, size_t id_pl, const std::string &token_pl,
    const std::chrono::system_clock::time_point& servertime)
    {
      
      IdToken params;
      //УСТАНАВЛИВАЕМ АЙДИ ИГРОКА
      params.id = id_pl;
      //УСТАНАВЛИВАЕМ ТОКЕН ИГРОКА
      params.token = token_pl;
      //УСТАНАВЛИВАЕМ НА КАКОЙ КАРТЕ ИГРОК
      params.map_name = map;
      
      //НА КАРТЕ Х ИГРОК У вошел в servertime ТОЧКУ ВРЕМЕНИ
      time_entered_[map][id_pl] = servertime;
      
      //НА КАРТЕ Х ИГРОК У состояние без движения
      activity_[map][id_pl] = {MoveStautus::STANDING, servertime};
      //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
      //НА КАРТЕ Х ИГРОКА ДАННЫЕ О ИГРОКЕ 
      info_[map][id_pl] = std::move(params);
    
    }

   

    std::optional<api::IdTWithDurSc> TCC::StructToDeleteOrNull(const std::string &map, size_t id_pl,
    const std::chrono::system_clock::time_point& servertime, std::chrono::milliseconds delta)
    {

       //ЕСЛИ ИГРОК В ДВИЖЕНИИ ЕГО УДАЛЯТЬ НЕ НАДО
       if(activity_.at(map).at(id_pl).movestatus == MoveStautus::MOVING){return std::nullopt;}
       
       //РАЗНИЦА ВРЕМЕНИ ТЕКУЩЕГО МОМЕНТА И ВРЕМЕНИ ПОСЛЕДНЕЙ АКТИВНОСТИ
       std::chrono::milliseconds difftime = duration_cast<std::chrono::milliseconds> 
       (servertime - activity_.at(map).at(id_pl).time_point);
       
       //ЕСЛИ ВРЕМЯ НЕ ПРИШЛО - НЕ ТРОГАЕМ ИГРОКА
       if(difftime < retire_time_){
           return std::nullopt;
       }
      
        // ВЫЧИСЛЯЕМ ВРЕМЯ В ИГРЕ КОТОРОЕ ПРОВЕЛ ИГРОК
        auto playtime = std::chrono::duration_cast<std::chrono::milliseconds>(servertime - time_entered_.at(map).at(id_pl)).count();

        // ФОРМИРУЕМ СТРУКТУРУ С ПАРАМЕТРАМИ токена, айди, имени карты, времени в игре
        IdTWithDurSc ret{std::move(info_.at(map).at(id_pl)), playtime};

        // УДАЛЯЕМ ИНФО ОБ ОБЬЕКТЕ В ЭТОМ КЛАССЕ:
        //В ИНФОРМАЦИИ О ИГРОКЕ
        info_.at(map).erase(id_pl);
        //В БАЗЕ ВРЕМЕНИ ВХОДА
        time_entered_.at(map).erase(id_pl);
        //В СЧЕТЧИКЕ АКТИВНОСТИ
        activity_.at(map).erase(id_pl);
        return ret;
    }

    std::chrono::system_clock::time_point TCC::GetNow()
    {
      return std::chrono::system_clock::now();
    }
}


namespace sql
{
  char* SQL::adress_;

}