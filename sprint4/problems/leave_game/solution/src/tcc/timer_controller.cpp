#include "timer_controller.h"

namespace api
{
    //ВРЕМЯ УДАЛЕНИЯ ИГРОКА
    std::chrono::milliseconds TCC::retire_time_;
    //МОМЕНТ ВРЕМЕНИ ВХОДА ИГРОКА
    std::unordered_map<std::string , std::unordered_map<size_t, std::chrono::system_clock::time_point >> TCC::time_entered_;
    //СЧЕТЧИК БЕЗДЕЙСТВИЯ
    std::unordered_map<std::string , std::unordered_map<size_t, IdToken>> TCC::info_;

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
      
      auto tcp = servertime;
      
      IdToken params;
      params.id = id_pl;
      params.token = token_pl;
      params.map_name = map;
      
      time_entered_[map][id_pl] = tcp;
      activity_[map][id_pl] = {MoveStautus::STANDING, tcp};
      //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
       info_[map][id_pl] = std::move(params);
    
    }

   

    std::optional<api::IdTWithDurSc> TCC::StructToDeleteOrNull(const std::string &map, size_t id_pl,
    const std::chrono::system_clock::time_point& servertime, std::chrono::milliseconds delta)
    {

       if(activity_.at(map).at(id_pl).movestatus == MoveStautus::MOVING){return std::nullopt;}
       
       //РАЗНИЦА ВРЕМЕНИ ТЕКУЩЕГО МОМЕНТА И ВРЕМЕНИ ПОСЛЕДНЕЙ АКТИВНОСТИ
       std::chrono::milliseconds difftime = duration_cast<std::chrono::milliseconds> 
       (servertime - activity_.at(map).at(id_pl).time_point);
       
       if(difftime < retire_time_){
           return std::nullopt;
       }
      
        // ВЫЧИСЛЯЕМ ВРЕМЯ В ИГРЕ
        auto playtime = std::chrono::duration_cast<std::chrono::milliseconds>(servertime - time_entered_.at(map).at(id_pl)).count();

        
        // ФОРМИРУЕМ СТРУКТУРУ С ПАРАМЕТРАМИ токена, айди, имени карты, времени в игре
        IdTWithDurSc ret{std::move(info_.at(map).at(id_pl)), playtime};

        // УДАЛЯЕМ ИНФО ОБ ОБЬЕКТЕ В ЭТОМ КЛАССЕ
        info_.at(map).erase(id_pl);
        time_entered_.at(map).erase(id_pl);
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